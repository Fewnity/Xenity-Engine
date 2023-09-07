#include "scene_manager.h"
#include "../../xenity.h"
#include "../../xenity_editor.h"
#include "../class_registry/class_registry.h"
#include "../reflection/reflection_utils.h"

using json = nlohmann::json;

std::shared_ptr<Scene> SceneManager::openedScene = nullptr;

json savedSceneData;
json savedSceneDataHotReloading;

#if defined(EDITOR)
void SceneManager::SaveScene(SaveSceneType saveType)
{
	json j;
	int gameObjectCount = Engine::gameObjectCount;
	for (int goI = 0; goI < gameObjectCount; goI++)
	{
		auto &go = Engine::gameObjects[goI];
		std::string goId = std::to_string(go->GetUniqueId());

		// Save GameObject's and Transform's values
		j["GameObjects"][goId]["Transform"]["Values"] = ReflectionUtils::ReflectionToJson(*go->GetTransform().get());
		j["GameObjects"][goId]["Values"] = ReflectionUtils::ReflectionToJson(*go.get());

		// Save GameObject's childs ids
		std::vector<uint64_t> ids;
		int childCount = go->GetChildrenCount();
		for (int childI = 0; childI < childCount; childI++)
		{
			ids.push_back(go->children[childI].lock()->GetUniqueId());
		}
		j["GameObjects"][goId]["Childs"] = ids;

		// Save components values
		int componentCount = go->GetComponentCount();
		for (int componentI = 0; componentI < componentCount; componentI++)
		{
			auto &component = go->components[componentI];
			std::string compId = std::to_string(component->GetUniqueId());
			j["GameObjects"][goId]["Components"][compId]["Type"] = component->GetComponentName();
			j["GameObjects"][goId]["Components"][compId]["Values"] = ReflectionUtils::ReflectionToJson((*component.get()));
		}

		j["Lighting"]["Values"] = ReflectionUtils::MapToJson(Graphics::GetLightingSettingsReflection());
	}

	if (saveType == SaveSceneForPlayState)
	{
		savedSceneData = j;
	}
	else if (saveType == SaveSceneForHotReloading)
	{
		savedSceneDataHotReloading = j;
	}
	else
	{
		std::string jsonData = j.dump(2);

		// Get scene path
		std::string path = "";
		if (SceneManager::openedScene)
		{
			path = SceneManager::openedScene->file->GetPath();
		}
		else 
		{
			path = EditorUI::SaveFileDialog("Save scene");
		}

		// If there is no error, save the file
		if (path != "") 
		{
			FileSystem::fileSystem->DeleteFile(path);
			File* file = new File(path);
			file->Open(true);
			file->Write(jsonData);
			file->Close();
			delete file;
		}
	}
}
#endif

void SceneManager::RestoreScene()
{
	LoadScene(savedSceneData);
}

void SceneManager::RestoreSceneHotReloading()
{
	LoadScene(savedSceneDataHotReloading);
}

void SceneManager::LoadScene(json jsonData)
{
#if !defined(EDITOR)
	Engine::SetGameState(Starting);
#endif
	
	EmptyScene();

	/*while (AssetManager::GetFileReferenceCount() != 0)
	{
		FileReference* fileRef = AssetManager::GetFileReference(0);
		fileRef->UnloadFileReference();
	}*/

	std::vector<std::shared_ptr<Component>> allComponents;

	// Create all GameObjects and Components
	for (auto& gameObjectKV : jsonData["GameObjects"].items())
	{
		std::shared_ptr<GameObject> newGameObject = CreateGameObject();
		newGameObject->SetUniqueId(std::stoull(gameObjectKV.key()));
		ReflectionUtils::JsonToReflection(gameObjectKV.value(), *newGameObject.get());
		newGameObject->OnReflectionUpdated();

		// Create components
		for (auto& componentKV : gameObjectKV.value()["Components"].items())
		{
			std::string componentName = componentKV.value()["Type"];
			std::shared_ptr<Component> comp = ClassRegistry::AddComponentFromName(componentName, newGameObject);
			allComponents.push_back(comp);
			if (comp)
			{
				comp->SetUniqueId(std::stoull(componentKV.key()));
			}
			else
			{
				Debug::PrintWarning("Class " + componentName + " not found in the scene");
			}
		}
	}

	// Bind Components values and GameObjects childs
	for (auto& kv : jsonData["GameObjects"].items())
	{
		auto go = FindGameObjectById(std::stoull(kv.key()));
		if (go)
		{
			for (auto& kv2 : kv.value()["Childs"].items())
			{
				auto goChild = FindGameObjectById(kv2.value());
				if (goChild)
				{
					goChild->SetParent(go);
				}
			}
		}

		ReflectionUtils::JsonToReflection(kv.value()["Transform"], *go->GetTransform().get());
		go->GetTransform()->isTransformationMatrixDirty = true;
		go->GetTransform()->UpdateWorldValues();

		for (auto& kv2 : kv.value()["Components"].items())
		{
			int componentCount = go->GetComponentCount();
			for (int compI = 0; compI < componentCount; compI++)
			{
				std::shared_ptr<Component> component = go->components[compI];
				if (component->GetUniqueId() == std::stoull(kv2.key()))
				{
					ReflectionUtils::JsonToReflection(kv2.value(), *component.get());
					component->OnReflectionUpdated();
					break;
				}
			}
		}
	}

	// Call Awake on Components
	if (Engine::GetGameState() == Starting) 
	{
		std::vector<std::shared_ptr<Component>> orderedComponentsToInit;
		int componentsCount = allComponents.size();
		int componentsToInitCount = 0;

		// Find uninitiated components and order them
		for (int i = 0; i < componentsCount; i++)
		{
			if (auto componentToCheck = allComponents[i])
			{
				if (!componentToCheck->initiated)
				{
					bool placeFound = false;
					for (int componentToInitIndex = 0; componentToInitIndex < componentsToInitCount; componentToInitIndex++)
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
			if (!orderedComponentsToInit[i]->isAwakeCalled && orderedComponentsToInit[i]->GetGameObject()->GetLocalActive())
			{
				orderedComponentsToInit[i]->Awake();
				orderedComponentsToInit[i]->isAwakeCalled = true;
			}
		}
	}

	ReflectionUtils::JsonToMap(jsonData["Lighting"], Graphics::GetLightingSettingsReflection());

#if !defined(EDITOR)
	Engine::SetGameState(Playing);
#endif
}

void SceneManager::LoadScene(std::shared_ptr<Scene> scene)
{
	Debug::Print("Loading scene...");
	File* jsonFile = scene->file;
	bool isOpen = jsonFile->Open(false);
	if (isOpen)
	{
		std::string jsonString = jsonFile->ReadAll();
		jsonFile->Close();

		json data;
		try
		{
			if(jsonString != "")
				data = json::parse(jsonString);
			LoadScene(data);
			openedScene = scene;
			Window::UpdateWindowTitle();
		}
		catch (const std::exception&)
		{
			Debug::PrintError("Scene file error");
			return;
		}
	}
}

void SceneManager::EmptyScene()
{
	Graphics::orderedIDrawable.clear();
	Graphics::usedCamera.reset();
	int cameraCount = Graphics::cameras.size();
	for (int i = 0; i < cameraCount; i++)
	{
		if (Graphics::cameras[i].expired() || !Graphics::cameras[i].lock()->isEditor)
		{
			Graphics::cameras.erase(Graphics::cameras.begin() + i);
			i--;
			cameraCount--;
		}
	}
	Engine::orderedComponents.clear();
	Engine::gameObjectsToDestroy.clear();
	Engine::componentsToDestroy.clear();
	Engine::gameObjects.clear();
	Engine::componentsCount = 0;
	Engine::gameObjectCount = 0;
	Engine::selectedGameObject.reset();
}