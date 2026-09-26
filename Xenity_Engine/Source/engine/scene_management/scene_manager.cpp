// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2026 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "scene_manager.h"

#if defined(EDITOR)
#include <editor/ui/editor_ui.h>
#include <editor/utils/file_reference_finder.h>
#include <editor/command/command_manager.h>
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

#if defined(EDITOR)
#include <fstream>
#include <filesystem>
#endif
#include <engine/world_partitionner/world_partitionner.h>
#include <engine/debug/stack_debug_object.h>
#include <engine/tools/gameplay_utility.h>

using ordered_json = nlohmann::ordered_json;

std::shared_ptr<Scene> SceneManager::s_openedScene = nullptr;
std::shared_ptr<Scene> SceneManager::s_nextSceneToLoad = nullptr;

ordered_json savedSceneData;
ordered_json savedSceneUsedFileListData;
ordered_json savedSceneDataHotReloading;
ordered_json savedSceneUsedFileListDataHotReloading;

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
		const std::shared_ptr<GameObject> child = gameObject.GetChildren()[childI].lock();
		if (!child || child.get() == &gameObject)
			continue;
		const uint64_t id = child->GetUniqueId();
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
			// Get all files ids used by the component
			FileReferenceFinder::GetUsedFilesInReflectiveData(uniqueIds, componentData);
		}
		else
		{
			// Or save component raw values
			j["Components"][compIdString] = missingScript->data;
			// Get all files ids used by the component (not optimal but prevents missing ids when saving a scene without compiling the game)
			FileReferenceFinder::GetUsedFilesInJson(uniqueIds, missingScript->data);
		}
	}

	return j;
}

/**
* @brief Write the scene file without ever leaving a partially written or mixed file on disk:
* the data is written to a temporary file, checked, then moved over the original file.
* The previous version of the scene is kept as a .bak file
* @return True if the scene file has been fully written
*/
static bool WriteSceneFileSafely(const std::string& path, const std::string& data)
{
	const std::string tempPath = path + ".tmp";
	const std::string backupPath = path + ".bak";

	try
	{
		{
			std::ofstream tempFile(tempPath, std::ios::binary | std::ios::trunc);
			if (!tempFile.is_open())
			{
				Debug::PrintError("[SceneManager::SaveScene] Fail to create the temporary scene file: " + tempPath, true);
				return false;
			}
			tempFile.write(data.data(), static_cast<std::streamsize>(data.size()));
			tempFile.flush();
			if (!tempFile.good())
			{
				tempFile.close();
				std::filesystem::remove(tempPath);
				Debug::PrintError("[SceneManager::SaveScene] Fail to write the temporary scene file: " + tempPath, true);
				return false;
			}
		}

		// Make sure the whole data is on disk before replacing the original file
		if (std::filesystem::file_size(tempPath) != data.size())
		{
			std::filesystem::remove(tempPath);
			Debug::PrintError("[SceneManager::SaveScene] The temporary scene file is incomplete: " + tempPath, true);
			return false;
		}

		// Keep the previous version of the scene
		if (std::filesystem::exists(path))
		{
			std::filesystem::copy_file(path, backupPath, std::filesystem::copy_options::overwrite_existing);
		}

		// Replace the original file (atomic on the same drive)
		std::filesystem::rename(tempPath, path);
	}
	catch (const std::exception& e)
	{
		std::error_code ec;
		std::filesystem::remove(tempPath, ec);
		Debug::PrintError("[SceneManager::SaveScene] Fail to save the scene file: " + path + " (" + e.what() + ")", true);
		return false;
	}

	return true;
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
		savedSceneUsedFileListData = usedFilesJson;
	}
	else if (saveType == SaveSceneType::SaveSceneForHotReloading)// Save Scene in a temporary json to restore it after compiling the game
	{
		savedSceneDataHotReloading = j;
		savedSceneUsedFileListDataHotReloading = usedFilesJson;
	}
	else
	{
		// Get scene path
		std::string path = "";
		if (s_openedScene)
		{
			path = s_openedScene->m_file->GetPath();
			XASSERT(!path.empty(), "[SceneManager::SaveScene] Scene path is empty");
		}
		else
		{
			path = EditorUI::SaveFileDialog("Save Scene", ProjectManager::GetAssetFolderPath());
		}

		// If there is no error, save the file
		if (!path.empty())
		{
			const std::string sceneData = usedFilesJson.dump(2) + "\n" + j.dump(2);
			if (WriteSceneFileSafely(path, sceneData))
			{
				ProjectManager::RefreshProjectDirectory();
				SetIsSceneDirty(false);
			}
			else
			{
				Debug::PrintError("[SceneManager::SaveScene] Fail to save the scene file: " + path, true);
			}
		}
	}
}

#endif

std::shared_ptr<GameObject> SceneManager::FindGameObjectByIdAdvanced(const uint64_t id, bool searchInTempList)
{
	const std::vector<std::shared_ptr<GameObject>>& gameObjects = (!tempGameobjects.empty() && searchInTempList) ? tempGameobjects : GameplayManager::GetGameObjects();

	const size_t gameObjectCount = gameObjects.size();

	if (!idRedirection.empty())
	{
		auto v = idRedirection.find(id);
		if (v != idRedirection.end())
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
	if (!idRedirection.empty())
	{
		auto v = idRedirection.find(id);
		if (v != idRedirection.end())
		{
			realId = v->second;
		}
	}

	const std::vector<std::shared_ptr<Component>>& components = (!tempComponents.empty() && searchInTempList) ? tempComponents : ComponentManager::GetAllComponents();
	for (const std::shared_ptr<Component>& component : components)
	{
		if ((component->GetUniqueId() == id || component->GetUniqueId() == realId) && IsValid(component))
		{
			return component;
		}
	}
	return std::shared_ptr<Component>();
}

void SceneManager::LoadScene(const std::shared_ptr<Scene>& scene)
{
	s_nextSceneToLoad = scene;
}

void SceneManager::CreateObjectsFromJson(const nlohmann::ordered_json& jsonData, bool createNewIds, std::shared_ptr<GameObject>* rootGameObject)
{
	idRedirection.clear();
	tempGameobjects.clear();
	tempComponents.clear();

	// Always clear the temporary lists, even if an exception is thrown (invalid json).
	// Otherwise FindGameObjectById/FindComponentById would keep searching in these stale lists
	struct TempListsCleaner
	{
		~TempListsCleaner()
		{
			idRedirection.clear();
			tempGameobjects.clear();
			tempComponents.clear();
		}
	};
	TempListsCleaner tempListsCleaner;

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
				std::shared_ptr<Component> comp;
				if (componentKV.value().contains("Type") && componentKV.value()["Type"].is_string())
				{
					const std::string componentName = componentKV.value()["Type"];
					if (!componentName.empty())
					{
						comp = ClassRegistry::AddComponentFromName(componentName, *newGameObject);
					}
				}
				else
				{
					Debug::PrintError("[SceneManager::CreateObjectsFromJson] Component " + componentKV.key() + " has no valid Type", true);
				}

				if (comp)
				{
					tempComponents.push_back(comp);

					// Enable or disable component
					if (componentKV.value().contains("Enabled") && componentKV.value()["Enabled"].is_boolean())
					{
						const bool isEnabled = componentKV.value()["Enabled"];
						comp->SetIsEnabled(isEnabled);
					}
				}

#if defined(EDITOR)
				if (!comp)
				{
					// If the component is missing (the class doesn't exist anymore or the game is not compiled)
					// Create a missing script and copy component data to avoid data loss
					comp = ClassRegistry::AddComponentFromName("MissingScript", *newGameObject);
					std::dynamic_pointer_cast<MissingScript>(comp)->data = componentKV.value();
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
				if (!kv2.value().is_number_integer())
				{
					Debug::PrintError("[SceneManager::CreateObjectsFromJson] Invalid child id in GameObject " + kv.key(), true);
					continue;
				}
				const std::shared_ptr<GameObject> goChild = FindGameObjectById(kv2.value());
				if (goChild == parentGameObject)
				{
					// Scenes saved with a GameObject set as its own child: ignore the loop to be able to open the scene
					Debug::PrintWarning("[SceneManager::CreateObjectsFromJson] GameObject " + kv.key() + " is its own child, the link has been removed");
					continue;
				}
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
			if (kv.value().contains("Transform"))
			{
				const std::shared_ptr<Transform>& transform = go->GetTransform();
				ReflectionUtils::JsonToReflective(kv.value()["Transform"], *transform.get());
				transform->m_isTransformationMatrixDirty = true;
				transform->UpdateLocalRotation();
				transform->UpdateWorldValues();
			}
			else
			{
				Debug::PrintError("[SceneManager::CreateObjectsFromJson] GameObject " + kv.key() + " has no Transform", true);
			}

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

	LoadSceneInternal(savedSceneData, savedSceneUsedFileListData);
}

void SceneManager::RestoreSceneHotReloading()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	LoadSceneInternal(savedSceneDataHotReloading, savedSceneUsedFileListDataHotReloading);
}

void SceneManager::ClearOpenedSceneFile()
{
	if (s_openedScene)
	{
		s_openedScene->m_fileReferenceList.clear();
		s_openedScene.reset();
	}
}

bool SceneManager::OnQuit(DialogMode dialogMode)
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
			if (dialogMode != DialogMode::ShowDialogAndLoadIfStop)
			{
				cancel = true;
			}
		}
		else 
		{
			cancel = true;
		}
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

void SceneManager::LoadSceneInternal(const ordered_json& jsonData, const ordered_json& jsonUsedFileListData)
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
	if (jsonUsedFileListData.contains("UsedFiles") && jsonUsedFileListData["UsedFiles"].contains("Values"))
	{
		for (const auto& idKv : jsonUsedFileListData["UsedFiles"]["Values"].items())
		{
			if (!idKv.value().is_number_integer())
			{
				Debug::PrintError("[SceneManager::LoadSceneInternal] Invalid file id in UsedFiles", true);
				continue;
			}
			const std::shared_ptr<FileReference> fileRef = ProjectManager::GetFileReferenceById(idKv.value());
			if (fileRef)
			{
				// No opened scene when the scene has never been saved (new scene, play mode, hot reloading)
				if (s_openedScene)
				{
					s_openedScene->m_fileReferenceList.push_back(fileRef);
				}

		#if !defined(EDITOR)
				FileReference::LoadOptions options;
				options.threaded = false;
				options.platform = Application::GetPlatform();
				fileRef->LoadFileReference(options);
		#endif
			}
		}
	}
	else
	{
		Debug::PrintError("[SceneManager::LoadSceneInternal] Scene has no UsedFiles list", true);
	}

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

void SceneManager::LoadSceneInternal(std::shared_ptr<Scene> scene, DialogMode dialogMode)
{
	s_nextSceneToLoad = nullptr;

	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	XASSERT(scene != nullptr, "[SceneManager::LoadScene] scene is nullptr");

	if (dialogMode != DialogMode::NoDialog)
	{
		const bool canceled = OnQuit(dialogMode);
		if (canceled)
			return;
	}

	Debug::Print("Loading scene...", true);

#if defined(EDITOR)
	// The undo history refers to the objects of the previous scene
	if (GameplayManager::GetGameState() == GameState::Stopped)
	{
		CommandManager::ClearCommands();
	}
#endif

	ClearOpenedSceneFile();
	s_openedScene = scene;

	// Read scene data
	const std::string jsonString = scene->ReadString();

	ordered_json data;
	ordered_json usedFileListData;
	try
	{
		// An empty or truncated file is an error, loading it as an empty scene would let the user overwrite the file with an empty scene
		if (jsonString.empty())
		{
			throw std::runtime_error("The file is empty");
		}

		const size_t sceneDataPosition = FindSceneDataPosition(jsonString);
		if (sceneDataPosition == -1)
		{
			throw std::runtime_error("Scene data not found");
		}

		const std::string sceneStr = jsonString.substr(sceneDataPosition);
		data = ordered_json::parse(sceneStr);

		const std::string sceneFileListStr = jsonString.substr(0, sceneDataPosition);
		usedFileListData = ordered_json::parse(sceneFileListStr);

		LoadSceneInternal(data, usedFileListData);
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
#if defined(EDITOR)
	// The undo history refers to the objects of the previous scene
	if (GameplayManager::GetGameState() == GameState::Stopped)
	{
		CommandManager::ClearCommands();
	}
#endif
	ClearOpenedSceneFile();
	ClearScene();
}
