#include "scene_manager.h"
#include "../../xenity.h"
#include "../../editor/editor.h"
#include "../class_registry/class_registry.h"
#include "../reflection/reflection_utils.h"

using json = nlohmann::json;

std::vector<std::shared_ptr<Component>> SceneManager::allCreatedComponents;
int SceneManager::allCreatedComponentsCount = 0;
Scene* SceneManager::openedScene = nullptr;

json savedSceneData;

void SceneManager::SaveScene()
{
	json j;
	int gameObjectCount = Engine::gameObjectCount;
	for (int goI = 0; goI < gameObjectCount; goI++)
	{
		auto go = Engine::gameObjects[goI];
		std::string goName = go->name;
		std::string goId = std::to_string(go->GetUniqueId());

		j["GameObjects"][goId]["Transform"]["Values"] = ReflectionUtils::ReflectiveToJson(*go->GetTransform().get());
		j["GameObjects"][goId]["Values"] = ReflectionUtils::ReflectiveToJson(*go.get());

		std::vector<uint64_t> ids;
		int childCount = go->GetChildrenCount();
		for (int childI = 0; childI < childCount; childI++)
		{
			ids.push_back(go->children[childI].lock()->GetUniqueId());
		}
		j["GameObjects"][goId]["Childs"] = ids;

		int componentCount = go->GetComponentCount();
		for (int componentI = 0; componentI < componentCount; componentI++)
		{
			auto component = go->components[componentI];
			std::string compName = component->GetComponentName();
			std::string compId = std::to_string(component->GetUniqueId());
			j["GameObjects"][goId]["Components"][compId]["Type"] = compName;
			j["GameObjects"][goId]["Components"][compId]["Values"] = ReflectionUtils::ReflectiveToJson((*component.get()));
		}
	}

	std::string s = j.dump(2);
	// std::cout << s << std::endl;
	std::string openedScenePath = SceneManager::openedScene->file->GetPath();
	FileSystem::fileSystem->DeleteFile(openedScenePath);
	File* file = new File(openedScenePath);
	file->Open(true);
	file->Write(s);
	file->Close();
	delete file;
}

void SceneManager::LoadScene(Scene* scene)
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
			data = json::parse(jsonString);
		}
		catch (const std::exception&)
		{
			Debug::PrintError("Scene file error");
			return;
		}

		Engine::EmptyScene();

		// Create all GameObjects and Components
		for (auto& kv : data["GameObjects"].items())
		{
			auto go = CreateGameObject();
			go->SetUniqueId(std::stoull(kv.key()));
			ReflectionUtils::JsonToReflection(kv.value(), *go.get());
			for (auto& kv2 : kv.value()["Components"].items())
			{
				std::string className = kv2.value()["Type"];
				auto comp = ClassRegistry::AddComponentFromName(className, go);
				if (comp) 
				{
					comp->SetUniqueId(std::stoull(kv2.key()));
					allCreatedComponents.push_back(comp);
				}
				else 
				{
					Debug::PrintWarning("Class " + className + " not found in the scene");
				}
			}
		}

		allCreatedComponentsCount = (int)allCreatedComponents.size();

		// Bind Components values and GameObjects childs
		for (auto& kv : data["GameObjects"].items())
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
					if (go->components[compI]->GetUniqueId() == std::stoull(kv2.key()))
					{
						ReflectionUtils::JsonToReflection(kv2.value(), *go->components[compI].get());
						go->components[compI]->OnReflectionUpdated();
						break;
					}
				}
			}
		}
		openedScene = scene;
		allCreatedComponents.clear();
		allCreatedComponentsCount = 0;
	}
	Window::UpdateWindowTitle();
}
