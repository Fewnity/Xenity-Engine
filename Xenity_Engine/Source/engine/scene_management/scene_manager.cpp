#include "scene_manager.h"
#include "scene.h"

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

#include <engine/physics/physics_manager.h>
#include <engine/tools/template_utils.h>

#if defined(EDITOR)
#include <editor/ui/editor_ui.h>
#include <editor/file_reference_finder.h>
#endif
#include <engine/debug/debug.h>
#include <engine/missing_script.h>

using json = nlohmann::json;

std::shared_ptr<Scene> SceneManager::openedScene = nullptr;

json savedSceneData;
json savedSceneDataHotReloading;

bool SceneManager::sceneModified = false;

const int SceneManager::sceneVersion = 1;

#if defined(EDITOR)

void SceneManager::SaveScene(SaveSceneType saveType)
{
	std::unordered_map<uint64_t, bool> usedIds;
	std::vector<uint64_t> usedFilesIds;

	json j;

	j["Version"] = sceneVersion;

	const int gameObjectCount = GameplayManager::gameObjectCount;
	for (int goI = 0; goI < gameObjectCount; goI++)
	{
		std::shared_ptr<GameObject>& go = GameplayManager::gameObjects[goI];
		const std::string goId = std::to_string(go->GetUniqueId());

		// Save GameObject's and Transform's values
		j["GameObjects"][goId]["Transform"]["Values"] = ReflectionUtils::ReflectiveToJson(*go->GetTransform().get());
		j["GameObjects"][goId]["Values"] = ReflectionUtils::ReflectiveToJson(*go.get());

		// Save GameObject's childs ids
		std::vector<uint64_t> ids;
		const int childCount = go->GetChildrenCount();
		for (int childI = 0; childI < childCount; childI++)
		{
			const uint64_t id = go->children[childI].lock()->GetUniqueId();
			if (usedIds[id])
			{
				Debug::PrintError("[SceneManager::SaveScene] GameObject Id already used by another Component/GameObject! Id: " + std::to_string(id), true);
			}
			usedIds[id] = true;
			ids.push_back(id);
		}
		j["GameObjects"][goId]["Childs"] = ids;

		// Save components values
		const int componentCount = go->GetComponentCount();
		for (int componentI = 0; componentI < componentCount; componentI++)
		{
			std::shared_ptr<Component>& component = go->components[componentI];
			const uint64_t compId = component->GetUniqueId();
			const std::string compIdString = std::to_string(compId);
			if (usedIds[compId])
			{
				Debug::PrintError("[SceneManager::SaveScene] Component Id already used by another Component/GameObject! Id: " + std::to_string(compId), true);
			}
			usedIds[compId] = true;

			if (std::shared_ptr<MissingScript> missingScript = std::dynamic_pointer_cast<MissingScript>(component))
			{
				j["GameObjects"][goId]["Components"][compIdString] = missingScript->data;
			}
			else 
			{
				j["GameObjects"][goId]["Components"][compIdString]["Type"] = component->GetComponentName();
				j["GameObjects"][goId]["Components"][compIdString]["Values"] = ReflectionUtils::ReflectiveToJson((*component.get()));
				j["GameObjects"][goId]["Components"][compIdString]["Enabled"] = component->GetIsEnabled();
			}

			ReflectiveData componentData = component->GetReflectiveData();
			FileReferenceFinder::GetUsedFilesInReflectiveData(usedFilesIds, componentData);
		}
	}

	j["Lighting"]["Values"] = ReflectionUtils::ReflectiveDataToJson(Graphics::GetLightingSettingsReflection());

	if (Graphics::skybox != nullptr)
		usedFilesIds.push_back(Graphics::skybox->fileId);
	j["UsedFiles"]["Values"] = usedFilesIds;

	if (saveType == SaveSceneType::SaveSceneForPlayState)
	{
		savedSceneData = j;
	}
	else if (saveType == SaveSceneType::SaveSceneForHotReloading)
	{
		savedSceneDataHotReloading = j;
	}
	else
	{
		// Get scene path
		std::string path = "";
		if (SceneManager::openedScene)
		{
			path = SceneManager::openedScene->file->GetPath();
		}
		else
		{
			path = EditorUI::SaveFileDialog("Save scene", ProjectManager::GetAssetFolderPath());
		}

		// If there is no error, save the file
		if (!path.empty())
		{
			FileSystem::fileSystem->Delete(path);
			std::shared_ptr<File> file = FileSystem::MakeFile(path);
			if (file->Open(FileMode::WriteCreateFile))
			{
				const std::string jsonData = j.dump(2);
				file->Write(jsonData);
				file->Close();
				ProjectManager::RefreshProjectDirectory();
				SetSceneModified(false);
			}
			else
			{
				Debug::PrintError("[SceneManager::SaveScene] Fail to save the scene file: " + file->GetPath(), true);
			}
		}
	}
}

#endif

void SceneManager::ReloadScene()
{
	LoadScene(openedScene);
}

void SceneManager::RestoreScene()
{
	LoadScene(savedSceneData);
}

void SceneManager::RestoreSceneHotReloading()
{
	LoadScene(savedSceneDataHotReloading);
}

void SceneManager::SetSceneModified(bool value)
{
	sceneModified = value;
	Window::UpdateWindowTitle();
}

bool SceneManager::OnQuit()
{
	bool cancel = false;
#if defined(EDITOR)
	if (sceneModified)
	{
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
#endif
	return cancel;
}

void SceneManager::LoadScene(const json& jsonData)
{
#if !defined(EDITOR)
	GameplayManager::SetGameState(GameState::Starting, true);
#endif

	ClearScene();

	std::vector<std::shared_ptr<Component>> allComponents;
	uint64_t biggestId = 0;

	if (jsonData.contains("GameObjects"))
	{
		// Create all GameObjects and Components
		for (const auto& gameObjectKV : jsonData["GameObjects"].items())
		{
			std::shared_ptr<GameObject> newGameObject = CreateGameObject();
			const uint64_t id = std::stoull(gameObjectKV.key());
			newGameObject->SetUniqueId(id);
			if (id > biggestId)
			{
				biggestId = id;
			}
			ReflectionUtils::JsonToReflective(gameObjectKV.value(), *newGameObject.get());

			// Create components
			if (gameObjectKV.value().contains("Components"))
			{
				for (auto& componentKV : gameObjectKV.value()["Components"].items())
				{
					const std::string componentName = componentKV.value()["Type"];
					std::shared_ptr<Component> comp = ClassRegistry::AddComponentFromName(componentName, newGameObject);

					// Get and set component id
					const uint64_t compId = std::stoull(componentKV.key());
					if (compId > biggestId)
					{
						biggestId = compId;
					}
					if (comp)
					{
						// Enable or disable component
						if (componentKV.value().contains("Enabled"))
						{
							const bool isEnabled = componentKV.value()["Enabled"];
							comp->SetIsEnabled(isEnabled);
						}
					}
					else
					{
						// If the component is missing (the class doesn't exist anymore or the game is not compiled
						// Create a missing script and copy component data to avoid data loss
						std::shared_ptr<MissingScript> missingScript = std::make_shared<MissingScript>();
						comp = missingScript;
						missingScript->data = componentKV.value();
						newGameObject->components.push_back(missingScript);
						newGameObject->componentCount++;
						missingScript->SetGameObject(newGameObject);
					}
					allComponents.push_back(comp);
					comp->SetUniqueId(compId);
				}
			}
		}

		UniqueId::lastUniqueId = biggestId;

		// Set gameobjects parents
		for (auto& kv : jsonData["GameObjects"].items())
		{
			const std::shared_ptr<GameObject> go = FindGameObjectById(std::stoull(kv.key()));
			if (go)
			{
				if (kv.value().contains("Childs"))
				{
					for (const auto& kv2 : kv.value()["Childs"].items())
					{
						std::shared_ptr<GameObject> goChild = FindGameObjectById(kv2.value());
						if (goChild)
						{
							goChild->SetParent(go);
						}
					}
				}
			}
		}

		// Bind Components values
		for (auto& kv : jsonData["GameObjects"].items())
		{
			const std::shared_ptr<GameObject> go = FindGameObjectById(std::stoull(kv.key()));
			if (go)
			{
				std::shared_ptr<Transform> transform = go->GetTransform();
				ReflectionUtils::JsonToReflective(kv.value()["Transform"], *transform.get());
				transform->isTransformationMatrixDirty = true;
				transform->UpdateWorldValues();

				if (kv.value().contains("Components"))
				{
					for (const auto& kv2 : kv.value()["Components"].items())
					{
						const int componentCount = go->GetComponentCount();
						for (int compI = 0; compI < componentCount; compI++)
						{
							std::shared_ptr<Component> component = go->components[compI];
							if (component->GetUniqueId() == std::stoull(kv2.key()))
							{
								ReflectionUtils::JsonToReflective(kv2.value(), *component.get());
								break;
							}
						}
					}
				}
			}
		}

		// Call Awake on Components
		if (GameplayManager::GetGameState() == GameState::Starting)
		{
			std::vector<std::shared_ptr<Component>> orderedComponentsToInit;
			const size_t componentsCount = allComponents.size();
			int componentsToInitCount = 0;

			// Find uninitiated components and order them
			for (size_t i = 0; i < componentsCount; i++)
			{
				if (auto& componentToCheck = allComponents[i])
				{
					if (!componentToCheck->initiated)
					{
						bool placeFound = false;
						for (size_t componentToInitIndex = 0; componentToInitIndex < componentsToInitCount; componentToInitIndex++)
						{
							// Check if the checked has a higher priority (lower value) than the component in the list
							if (componentToCheck->updatePriority <= orderedComponentsToInit[componentToInitIndex]->updatePriority)
							{
								orderedComponentsToInit.insert(orderedComponentsToInit.begin() + componentToInitIndex, componentToCheck);
								placeFound = true;
								break;
							}
						}
						// if the priority is lower than all components's priorities in the list, add it the end of the list
						if (!placeFound)
						{
							orderedComponentsToInit.push_back(componentToCheck);
						}
						componentsToInitCount++;
					}
				}
			}

			for (int i = 0; i < componentsToInitCount; i++)
			{
				std::shared_ptr<Component> componentToInit = orderedComponentsToInit[i];
				if (!componentToInit->isAwakeCalled && componentToInit->GetGameObject()->GetLocalActive())
				{
					componentToInit->Awake();
					componentToInit->isAwakeCalled = true;
				}
			}
		}
	}
	else
	{
		UniqueId::lastUniqueId = 0;
	}

	if (jsonData.contains("Lighting"))
	{
		ReflectionUtils::JsonToReflectiveData(jsonData["Lighting"], Graphics::GetLightingSettingsReflection());
		Graphics::OnLightingSettingsReflectionUpdate();
	}

#if !defined(EDITOR)
	GameplayManager::SetGameState(GameState::Playing, true);
#endif
}

void SceneManager::LoadScene(const std::shared_ptr<Scene>& scene)
{
	bool canceled = OnQuit();
	if (canceled)
		return;

	Debug::Print("Loading scene...", true);
	std::shared_ptr<File> jsonFile = scene->file;
	const bool isOpen = jsonFile->Open(FileMode::ReadOnly);
	if (isOpen)
	{
		const std::string jsonString = jsonFile->ReadAll();
		jsonFile->Close();

		try
		{
			json data;
			if (!jsonString.empty())
				data = json::parse(jsonString);
			LoadScene(data);
			openedScene = scene;
			SetSceneModified(false);
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
	}
}

void SceneManager::ClearScene()
{
	Graphics::DeleteAllDrawables();
	Graphics::usedCamera.reset();
	size_t cameraCount = Graphics::cameras.size();
	for (size_t i = 0; i < cameraCount; i++)
	{
		if (Graphics::cameras[i].expired() || !Graphics::cameras[i].lock()->GetIsEditor())
		{
			Graphics::cameras.erase(Graphics::cameras.begin() + i);
			i--;
			cameraCount--;
		}
	}

	PhysicsManager::rigidBodies.clear();
	GameplayManager::orderedComponents.clear();
	GameplayManager::gameObjectsToDestroy.clear();
	GameplayManager::componentsToDestroy.clear();
	GameplayManager::gameObjects.clear();
	GameplayManager::componentsCount = 0;
	GameplayManager::gameObjectCount = 0;
#if defined(EDITOR)
	Editor::SetSelectedGameObject(nullptr);
#endif
	Window::UpdateWindowTitle();
}

void SceneManager::CreateEmptyScene()
{
	openedScene.reset();
	SceneManager::ClearScene();
}
