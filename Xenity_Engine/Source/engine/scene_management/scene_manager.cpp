// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "scene_manager.h"

#if defined(EDITOR)
#include <editor/ui/editor_ui.h>
#include <editor/utils/file_reference_finder.h>
#endif

#include <engine/accessors/acc_gameobject.h>
#include <engine/file_system/file_system.h>
#include <engine/file_system/file.h>
#include <engine/class_registry/class_registry.h>
#include <engine/reflection/reflection_utils.h>
#include <engine/asset_management/project_manager.h>
#include <engine/game_elements/gameplay_manager.h>
#include <engine/game_elements/transform.h>
#include <engine/ui/window.h>
#include <engine/graphics/graphics.h>
#include <engine/graphics/camera.h>
#include <engine/game_elements/prefab.h>
#include <engine/physics/physics_manager.h>
#include <engine/tools/template_utils.h>
#include <engine/debug/debug.h>
#include <engine/missing_script.h>
#include "scene.h"
#include <engine/world_partitionner/world_partitionner.h>
#include <engine/debug/stack_debug_object.h>
#include <engine/tools/gameplay_utility.h>

using ordered_json = nlohmann::ordered_json;

std::shared_ptr<Scene> SceneManager::s_openedScene = nullptr;

ordered_json savedSceneData;
ordered_json savedSceneDataHotReloading;

bool SceneManager::s_sceneModified = false;

std::unordered_map<uint64_t, uint64_t> SceneManager::idRedirection;
std::vector<std::shared_ptr<GameObject>> SceneManager::tempGameobjects;
std::vector<std::shared_ptr<Component>> SceneManager::tempComponents;
#if defined(EDITOR)

void SceneManager::SetIsSceneDirty(bool value)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	if (GameplayManager::GetGameState() == GameState::Stopped)
	{
		s_sceneModified = value;
		Window::UpdateWindowTitle();
	}
}

nlohmann::ordered_json SceneManager::GameObjectToJson(GameObject& gameObject, std::set<uint64_t>& uniqueIds)
{
	ordered_json j;

	// Save GameObject's and Transform's values
	j["Transform"]["Values"] = ReflectionUtils::ReflectiveToJson(*gameObject.GetTransform().get());
	j["Values"] = ReflectionUtils::ReflectiveToJson(gameObject);

	// Save GameObject's children ids
	std::vector<uint64_t> ids;
	const int childCount = gameObject.GetChildrenCount();
	for (int childI = 0; childI < childCount; childI++)
	{
		const uint64_t id = gameObject.GetChildren()[childI].lock()->GetUniqueId();
		//if (usedIds[id])
		//{
		//	Debug::PrintError("[SceneManager::SaveScene] GameObject Id already used by another Component/GameObject! Id: " + std::to_string(id), true);
		//}
		//usedIds[id] = true;
		ids.push_back(id);
	}
	j["Children"] = ids;

	// Save components values
	for (const std::shared_ptr<Component>& component : gameObject.m_components)
	{
		const uint64_t compId = component->GetUniqueId();
		const std::string compIdString = std::to_string(compId);
		//if (usedIds[compId])
		//{
		//	Debug::PrintError("[SceneManager::SaveScene] Component Id already used by another Component/GameObject! Id: " + compIdString, true);
		//}
		//usedIds[compId] = true;

		const ReflectiveData componentData = component->GetReflectiveData();

		const std::shared_ptr<MissingScript> missingScript = std::dynamic_pointer_cast<MissingScript>(component);
		// If the component is valid, save values
		if (!missingScript)
		{
			j["Components"][compIdString]["Type"] = component->GetComponentName();
			j["Components"][compIdString]["Values"] = ReflectionUtils::ReflectiveDataToJson(componentData);
			j["Components"][compIdString]["Enabled"] = component->IsEnabled();
		}
		else
		{
			// Or save component raw values
			j["Components"][compIdString] = missingScript->data;
		}

		// Get all files ids used by the component
		FileReferenceFinder::GetUsedFilesInReflectiveData(uniqueIds, componentData);
	}

	return j;
}

void SceneManager::SaveScene(SaveSceneType saveType)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	//std::unordered_map<uint64_t, bool> usedIds;
	std::set<uint64_t> usedFilesIds;

	// Use ordered json to keep gameobject's order
	ordered_json j;
	ordered_json usedFilesJson;

	j["Version"] = s_sceneVersion;

	// For each gameobject:
	for (const std::shared_ptr<GameObject>& go : GameplayManager::gameObjects)
	{
		const std::string gameObjectId = std::to_string(go->GetUniqueId());
		j["GameObjects"][gameObjectId] = GameObjectToJson(*go, usedFilesIds);
	}

	// Save lighting data
	j["Lighting"]["Values"] = ReflectionUtils::ReflectiveDataToJson(Graphics::s_settings.GetReflectiveData());

	// Add skybox file to the usedFile list
	if (Graphics::s_settings.skybox != nullptr)
	{
		usedFilesIds.insert(Graphics::s_settings.skybox->m_fileId);
	}

	// Save the usedFilesIds list
	usedFilesJson["UsedFiles"]["Values"] = usedFilesIds;

	if (saveType == SaveSceneType::SaveSceneForPlayState) // Save Scene in a temporary json to restore it after quitting play mode
	{
		savedSceneData = j;
	}
	else if (saveType == SaveSceneType::SaveSceneForHotReloading)// Save Scene in a temporary json to restore it after compiling the game
	{
		savedSceneDataHotReloading = j;
	}
	else
	{
		// Get scene path
		std::string path = "";
		if (SceneManager::s_openedScene)
		{
			path = SceneManager::s_openedScene->m_file->GetPath();
			XASSERT(!path.empty(), "[SceneManager::SaveScene] Scene path is empty");
		}
		else
		{
			path = EditorUI::SaveFileDialog("Save Scene", ProjectManager::GetAssetFolderPath());
		}

		// If there is no error, save the file
		if (!path.empty())
		{
			FileSystem::Delete(path);
			const std::shared_ptr<File> file = FileSystem::MakeFile(path);
			if (file->Open(FileMode::WriteCreateFile))
			{
				const std::string usedFilesJsonData = usedFilesJson.dump(2);
				const std::string jsonData = j.dump(2);
				file->Write(usedFilesJsonData);
				file->Write("\n");
				file->Write(jsonData);
				file->Close();
				ProjectManager::RefreshProjectDirectory();
				SetIsSceneDirty(false);
			}
			else
			{
				Debug::PrintError("[SceneManager::SaveScene] Fail to save the scene file: " + file->GetPath(), true);
			}
		}
	}
}

#endif

std::shared_ptr<GameObject> SceneManager::FindGameObjectByIdAdvanced(const uint64_t id, bool searchInTempList)
{
	const std::vector<std::shared_ptr<GameObject>>& gameObjects = (!SceneManager::tempGameobjects.empty() && searchInTempList) ? SceneManager::tempGameobjects : GameplayManager::GetGameObjects();

	const size_t gameObjectCount = gameObjects.size();

	if (!SceneManager::idRedirection.empty())
	{
		auto v = SceneManager::idRedirection.find(id);
		if (v != SceneManager::idRedirection.end())
		{
			uint64_t realId = v->second;
			for (size_t i = 0; i < gameObjectCount; i++)
			{
				if (gameObjects[i]->GetUniqueId() == realId)
					return gameObjects[i];
			}
		}
	}

	for (size_t i = 0; i < gameObjectCount; i++)
	{
		if (gameObjects[i]->GetUniqueId() == id)
			return gameObjects[i];
	}

	return std::shared_ptr<GameObject>();
}

std::shared_ptr<Component> SceneManager::FindComponentByIdAdvanced(const uint64_t id, bool searchInTempList)
{
	uint64_t realId = -1;
	if (!SceneManager::idRedirection.empty())
	{
		auto v = SceneManager::idRedirection.find(id);
		if (v != SceneManager::idRedirection.end())
		{
			realId = v->second;
		}
	}

	const std::vector<std::shared_ptr<Component>>& components = (!SceneManager::tempComponents.empty() && searchInTempList) ? SceneManager::tempComponents : ComponentManager::GetAllComponents();
	for (const std::shared_ptr<Component>& component : components)
	{
		if ((component->GetUniqueId() == id || component->GetUniqueId() == realId) && IsValid(component))
		{
			return component;
		}
	}
	return std::shared_ptr<Component>();
}

void SceneManager::CreateObjectsFromJson(const nlohmann::ordered_json& jsonData, bool createNewIds, std::shared_ptr<GameObject>* rootGameObject)
{
	idRedirection.clear();
	tempGameobjects.clear();
	tempComponents.clear();

	std::vector<std::shared_ptr<Component>> allComponents;
	// Create all GameObjects and Components
	for (const auto& gameObjectKV : jsonData.items())
	{
		const std::shared_ptr<GameObject> newGameObject = CreateGameObject();
		tempGameobjects.push_back(newGameObject);

		// Set gameobject id
		const uint64_t gameObjectId = std::stoull(gameObjectKV.key());
		if (createNewIds)
		{
			idRedirection[gameObjectId] = newGameObject->GetUniqueId();
		}
		else
		{
			newGameObject->SetUniqueId(gameObjectId);
		}

		// Fill gameobjet's values from json
		ReflectionUtils::JsonToReflective(gameObjectKV.value(), *newGameObject.get());

		// Create components if there is any
		if (gameObjectKV.value().contains("Components"))
		{
			for (auto& componentKV : gameObjectKV.value()["Components"].items())
			{
				const std::string componentName = componentKV.value()["Type"];
				std::shared_ptr<Component> comp = ClassRegistry::AddComponentFromName(componentName, *newGameObject);

				if (comp)
				{
					tempComponents.push_back(comp);

					// Enable or disable component
					if (componentKV.value().contains("Enabled"))
					{
						const bool isEnabled = componentKV.value()["Enabled"];
						comp->SetIsEnabled(isEnabled);
					}
				}
#if defined(EDITOR)
				else
				{
					// If the component is missing (the class doesn't exist anymore or the game is not compiled)
					// Create a missing script and copy component data to avoid data loss
					const std::shared_ptr<MissingScript> missingScript = std::make_shared<MissingScript>();
					missingScript->data = componentKV.value();
					newGameObject->AddExistingComponent(missingScript);
					comp = missingScript;
				}
#endif
				if (comp)
				{
					allComponents.push_back(comp);

					// Get and set component id
					const uint64_t componentId = std::stoull(componentKV.key());
					if (createNewIds)
					{
						idRedirection[componentId] = comp->GetUniqueId();
					}
					else
					{
						comp->SetUniqueId(componentId);
					}
				}
			}
		}
	}

	// Set gameobjects parents
	for (auto& kv : jsonData.items())
	{
		const std::shared_ptr<GameObject> parentGameObject = FindGameObjectById(std::stoull(kv.key()));
		// Check if the parent has children
		if (parentGameObject && kv.value().contains("Children"))
		{
			// For each child, set his parent
			for (const auto& kv2 : kv.value()["Children"].items())
			{
				const std::shared_ptr<GameObject> goChild = FindGameObjectById(kv2.value());
				if (goChild)
				{
					goChild->SetParent(parentGameObject);
				}
			}
		}
	}

	// Bind Components values
	for (auto& kv : jsonData.items())
	{
		const std::shared_ptr<GameObject> go = FindGameObjectById(std::stoull(kv.key()));
		if (go)
		{
			// Get the root gameobject for prefabs
			if (rootGameObject && go->GetParent().lock() == nullptr)
			{
				*rootGameObject = go;
			}

			// Update transform
			const std::shared_ptr<Transform>& transform = go->GetTransform();
			ReflectionUtils::JsonToReflective(kv.value()["Transform"], *transform.get());
			transform->m_isTransformationMatrixDirty = true;
			transform->UpdateLocalRotation();
			transform->UpdateWorldValues();

			// If the gameobject has components
			if (kv.value().contains("Components"))
			{
				const int componentCount = go->GetComponentCount();
				// Find the component with the Id
				for (const auto& kv2 : kv.value()["Components"].items())
				{
					uint64_t componentId = std::stoull(kv2.key());
					if (createNewIds)
					{
						componentId = idRedirection[componentId];
					}
					for (int compI = 0; compI < componentCount; compI++)
					{
						const std::shared_ptr<Component>& component = go->m_components[compI];
						if ((createNewIds && component->GetUniqueId() == idRedirection[std::stoull(kv2.key())]) || (!createNewIds && component->GetUniqueId() == std::stoull(kv2.key())))
						{
							// Fill values
							ReflectionUtils::JsonToReflective(kv2.value(), *component.get());
							break;
						}
					}
				}
			}
		}
	}

	idRedirection.clear();
	tempGameobjects.clear();
	tempComponents.clear();

	// Call Awake on Components
	if (GameplayManager::GetGameState() == GameState::Starting)
	{
		const size_t componentsCount = allComponents.size();

		//TODO sort components by their update order

		// Call components Awake() function
		for (int i = 0; i < componentsCount; i++)
		{
			const std::shared_ptr<Component>& componentToInit = allComponents[i];
			if (componentToInit->GetGameObject()->IsLocalActive() && componentToInit->IsEnabled())
			{
				componentToInit->Awake();
				componentToInit->m_isAwakeCalled = true;
			}
		}
	}
}

size_t SceneManager::FindSceneDataPosition(const std::string& jsonString)
{
	size_t bracketCount = 0;
	size_t charI = 0;
	size_t jsonStringSize = jsonString.size();
	while (true)
	{
		if (charI == jsonStringSize)
		{
			charI = -1;
			break;
		}
		if (jsonString[charI] == '{')
		{
			bracketCount++;
		}
		else if (jsonString[charI] == '}')
		{
			bracketCount--;
			if (bracketCount == 0)
			{
				charI += 2;
				break;
			}
		}
		charI++;
	}

	return charI;
}

void SceneManager::ReloadScene()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	LoadScene(s_openedScene);
}

void SceneManager::RestoreScene()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	LoadScene(savedSceneData);
}

void SceneManager::RestoreSceneHotReloading()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	LoadScene(savedSceneDataHotReloading);
}

bool SceneManager::OnQuit()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	bool cancel = false;
#if defined(EDITOR)
	if (GameplayManager::GetGameState() != GameState::Stopped)
	{
		const DialogResult result = EditorUI::OpenDialog("You are in play mode", "Do you want to stop the game?", DialogType::Dialog_Type_YES_NO_CANCEL);
		if (result == DialogResult::Dialog_YES)
		{
			GameplayManager::SetGameState(GameState::Stopped, true);
		}
		cancel = true;
	}
	else
	{
		if (s_sceneModified)
		{
			// Ask if the user wants to save the scene or not if the quit the scene
			const DialogResult result = EditorUI::OpenDialog("The Scene Has Been Modified", "Do you want to save?", DialogType::Dialog_Type_YES_NO_CANCEL);
			if (result == DialogResult::Dialog_YES)
			{
				SaveScene(SaveSceneType::SaveSceneToFile);
			}
			else if (result == DialogResult::Dialog_CANCEL)
			{
				cancel = true;
			}
		}
	}

#endif
	return cancel;
}

void SceneManager::LoadScene(const ordered_json& jsonData)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	// Automaticaly start the game if built in engine mode
#if !defined(EDITOR)
	GameplayManager::SetGameState(GameState::Starting, true);
#else
	if (GameplayManager::GetGameState() == GameState::Playing)
	{
		GameplayManager::SetGameState(GameState::Starting, true);
	}
#endif

	ClearScene();

	if (jsonData.contains("GameObjects"))
	{
		CreateObjectsFromJson(jsonData["GameObjects"], false);
	}

	// Load lighting values
	if (jsonData.contains("Lighting"))
	{
		ReflectionUtils::JsonToReflectiveData(jsonData["Lighting"], Graphics::s_settings.GetReflectiveData());
		Graphics::OnLightingSettingsReflectionUpdate();
	}

	// Automaticaly set the game in play mode if built in engine mode
//#if !defined(EDITOR)
	if (GameplayManager::GetGameState() == GameState::Starting)
	{
		GameplayManager::SetGameState(GameState::Playing, true);
	}
	//#endif
}

void SceneManager::LoadScene(const std::shared_ptr<Scene>& scene)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	XASSERT(scene != nullptr, "[SceneManager::LoadScene] scene is nullptr");

	const bool canceled = OnQuit();
	if (canceled)
		return;

	Debug::Print("Loading scene...", true);

	// Read scene data
	const std::string jsonString = scene->ReadString();

	XASSERT(!jsonString.empty(), "[SceneManager::LoadScene] jsonString is empty");

	ordered_json data;
	try
	{
		if (!jsonString.empty())
		{
			const size_t sceneDataPosition = FindSceneDataPosition(jsonString);
			if (sceneDataPosition != -1)
			{
				const std::string sceneStr = jsonString.substr(sceneDataPosition);
				data = ordered_json::parse(sceneStr);
			}
		}
	}
	catch (const std::exception& e)
	{
		CreateEmptyScene();
#if defined(EDITOR)
		EditorUI::OpenDialog("Error", "Error while loading the scene. The file is probably corrupted.", DialogType::Dialog_Type_OK);
#endif
		Debug::PrintError("[SceneManager::LoadScene] Scene file error: " + std::string(e.what()), true);
		return;
	}

	LoadScene(data);
	s_openedScene = scene;
#if defined(EDITOR)
	SetIsSceneDirty(false);
#endif
}

void SceneManager::ClearScene()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	GameplayManager::gameObjectsToDestroy.clear();
	GameplayManager::componentsToDestroy.clear();
	GameplayManager::gameObjects.clear();
	GameplayManager::gameObjectCount = 0;
	WorldPartitionner::ClearWorld();
	Graphics::DeleteAllDrawables();
	Graphics::usedCamera.reset();
	size_t cameraCount = Graphics::cameras.size();
	for (size_t i = 0; i < cameraCount; i++)
	{
		if (Graphics::cameras[i].expired() || !Graphics::cameras[i].lock()->IsEditor())
		{
			Graphics::cameras.erase(Graphics::cameras.begin() + i);
			i--;
			cameraCount--;
		}
	}

	PhysicsManager::Clear();
#if defined(EDITOR)
	Editor::SetSelectedGameObject(nullptr);
#endif
	Window::UpdateWindowTitle();
}

void SceneManager::CreateEmptyScene()
{
	s_openedScene.reset();
	SceneManager::ClearScene();
}
