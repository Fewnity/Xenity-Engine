#include "editor.h"
#include "../xenity.h"
#include "../xenity_editor.h"

#include "ui/menus/engine_settings_menu.h"
#include "ui/menus/file_explorer_menu.h"
#include "ui/menus/hierarchy_menu.h"
#include "ui/menus/inspector_menu.h"
#include "ui/menus/main_bar_menu.h"
#include "ui/menus/profiler_menu.h"

#include <functional>
#include "../engine/class_registry/class_registry.h"
#include "../engine/file_system/file_reference.h"
#include "../engine/file_system/mesh_loader/wavefront_loader.h";
#include "../engine/asset_management/project_manager.h"

using json = nlohmann::json;

std::weak_ptr<GameObject> Editor::cameraGO;

EngineSettingsMenu* engineSettings = nullptr;
FileExplorerMenu* fileExplorer = nullptr;
HierarchyMenu* hierarchy = nullptr;
InspectorMenu* inspector = nullptr;
MainBarMenu* mainBar = nullptr;
ProfilerMenu* profiler = nullptr;

void Editor::JsonToReflection(json j, Reflection& component)
{
	for (auto& kv : j["Values"].items())
	{
		auto t = component.GetReflection();
		if (t.contains(kv.key()))
		{
			Variable& variableRef = t.at(kv.key());
			if (kv.value().is_object())
			{
				//std::cout << kv.key() << " is an object!" << "\n";
				if (auto valuePtr = std::get_if<std::reference_wrapper<Reflection>>(&variableRef))
				{
					JsonToReflection(kv.value(), valuePtr->get());
				}
			}
			else
			{
				if (auto valuePtr = std::get_if< std::reference_wrapper<int>>(&variableRef))
					valuePtr->get() = kv.value();
				else if (auto valuePtr = std::get_if<std::reference_wrapper<float>>(&variableRef))
					valuePtr->get() = kv.value();
				else if (auto valuePtr = std::get_if< std::reference_wrapper<double>>(&variableRef))
					valuePtr->get() = kv.value();
				else if (auto valuePtr = std::get_if< std::reference_wrapper<std::string>>(&variableRef))
					valuePtr->get() = kv.value();
				else if (auto valuePtr = std::get_if< std::reference_wrapper<bool>>(&variableRef))
					valuePtr->get() = kv.value();
				else if (auto valuePtr = std::get_if<std::weak_ptr<GameObject>*>(&variableRef))
				{
					auto go = FindGameObjectById(kv.value());
					**valuePtr = go;
				}
				else if (auto valuePtr = std::get_if<std::weak_ptr<Transform>*>(&variableRef))
				{
					auto go = FindGameObjectById(kv.value());
					**valuePtr = go->GetTransform();
				}
				else if (auto valuePtr = std::get_if<std::reference_wrapper<Texture*>>(&variableRef))
				{
					int fileId = kv.value();
					FileReference* file = ProjectManager::GetFileReferenceById(fileId);
					if (file)
					{
						file->LoadFileReference();
						valuePtr->get() = (Texture*)file;
					}
				}
				else if (auto valuePtr = std::get_if<std::reference_wrapper<MeshData*>>(&variableRef))
				{
					int fileId = kv.value();
					FileReference* file = ProjectManager::GetFileReferenceById(fileId);
					if (file) 
					{
						file->LoadFileReference();
						valuePtr->get() = (MeshData*)file;
					}
				}
				/*else if (auto valuePtr = std::get_if<void*>(&variableRef))
				{
					std::weak_ptr<Component>* weakC = (std::weak_ptr<Component>*)(*valuePtr);

					for (int compI = 0; compI < allCreatedComponentsCount; compI++)
					{
						if (allCreatedComponents[compI]->GetUniqueId() == kv.value())
						{
							*weakC = allCreatedComponents[compI];
							break;
						}
					}
				}*/
			}
		}
		// std::cout << kv.key() << " : " << kv.value() << "\n";
	}
}

void Editor::Start()
{
	engineSettings = new EngineSettingsMenu();
	fileExplorer = new FileExplorerMenu();
	hierarchy = new HierarchyMenu();
	inspector = new InspectorMenu();
	mainBar = new MainBarMenu();
	profiler = new ProfilerMenu();

	engineSettings->Init();
	fileExplorer->Init();
	hierarchy->Init();
	inspector->Init();
	mainBar->Init();
	profiler->Init();

	ProjectManager::LoadProject();

	cameraGO = CreateGameObjectEditor("Camera");
	auto camera = cameraGO.lock()->AddComponent<Camera>();
	camera->SetNearClippingPlane(0.01f);
	camera->SetFarClippingPlane(500);
	camera->SetProjectionSize(5.0f);
	camera->SetFov(70);
}

void Editor::Update()
{
	auto cameraTrans = cameraGO.lock()->GetTransform();
	Vector3 rot = cameraTrans->GetRotation();
	Vector3 pos = cameraTrans->GetPosition();

	if (InputSystem::GetKey(MOUSE_RIGHT))
	{
		rot.x += -InputSystem::mouseSpeed.y * Time::GetDeltaTime() * 20000;
		rot.y += InputSystem::mouseSpeed.x * Time::GetDeltaTime() * 20000;
	}

	float fwd = 0;
	float side = 0;
	if (InputSystem::GetKey(UP))
		fwd = -1;
	else if (InputSystem::GetKey(DOWN))
		fwd = 1;

	if (InputSystem::GetKey(RIGHT))
		side = 1;
	else if (InputSystem::GetKey(LEFT))
		side = -1;

	pos -= cameraTrans->GetForward() * (fwd / 7.0f) * Time::GetDeltaTime() * 30;
	pos -= cameraTrans->GetLeft() * (side / 7.0f) * Time::GetDeltaTime() * 30;

	cameraTrans->SetPosition(pos);
	cameraTrans->SetRotation(rot);
}

void Editor::Draw()
{
	EditorUI::NewFrame();
	if (EditorUI::showEngineSettings)
	{
		engineSettings->Draw();
	}
	if (EditorUI::showEditor)
	{
		fileExplorer->Draw();
		hierarchy->Draw();
		inspector->Draw();
	}
	mainBar->Draw();
	if (EditorUI::showProfiler)
	{
		profiler->Draw();
	}
	EditorUI::Render();
}

void Editor::CreateEmpty()
{
	auto go = CreateGameObject();
	Engine::SetSelectedGameObject(go);
}

void Editor::CreateEmptyChild()
{
	auto go = CreateGameObject();
	go->SetParent(Engine::selectedGameObject);
	Engine::SetSelectedGameObject(go);
}

#pragma region Save

json Editor::ReflectiveToJson(Reflection& relection)
{
	json j2;
	auto t = relection.GetReflection();
	for (const auto& kv :t)
	{
		Variable& variableRef = t.at(kv.first);
		if (auto valuePtr = std::get_if< std::reference_wrapper<int>>(&variableRef))
			j2[kv.first] = valuePtr->get();
		else if (auto valuePtr = std::get_if<std::reference_wrapper<float>>(&variableRef))
			j2[kv.first] = valuePtr->get();
		else if (auto valuePtr = std::get_if<std::reference_wrapper<double>>(&variableRef))
			j2[kv.first] = valuePtr->get();
		else if (auto valuePtr = std::get_if< std::reference_wrapper<std::string>>(&variableRef))
			j2[kv.first] = valuePtr->get();
		else if (auto valuePtr = std::get_if< std::reference_wrapper<bool>>(&variableRef))
			j2[kv.first] = valuePtr->get();
		else if (auto valuePtr = std::get_if<std::weak_ptr<GameObject> *>(&variableRef))
		{
			if (auto lockValue = (**valuePtr).lock())
				j2[kv.first] = lockValue->GetUniqueId();
		}
		else if (auto valuePtr = std::get_if<std::weak_ptr<Transform> *>(&variableRef))
		{
			if (auto lockValue = (**valuePtr).lock())
				j2[kv.first] = lockValue->GetGameObject()->GetUniqueId();
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<Reflection>>(&variableRef))
		{
			j2[kv.first]["Values"] = ReflectiveToJson(valuePtr->get());
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<std::weak_ptr<Component>>>(&variableRef))
		{
			if (auto lockValue = (valuePtr->get()).lock())
				j2[kv.first]["Values"] = ReflectiveToJson(*lockValue.get());
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<MeshData*>>(&variableRef))
		{
			if (valuePtr->get() != nullptr)
				j2[kv.first] = (valuePtr->get())->fileId;
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<AudioClip*>>(&variableRef))
		{
			if (valuePtr->get() != nullptr)
				j2[kv.first] = (valuePtr->get())->fileId;
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<Texture*>>(&variableRef))
		{
			if (valuePtr->get() != nullptr)
				j2[kv.first] = (valuePtr->get())->fileId;
		}
		/*else if (auto valuePtr = std::get_if<void*>(&variableRef))
		{
			std::weak_ptr<Component>* weakC = (std::weak_ptr<Component>*)(*valuePtr);
			if (auto lockValue = weakC->lock())
			{
				j2[kv.first] = lockValue->GetUniqueId();
			}
		}*/
	}
	return j2;
}

void Editor::SaveScene()
{
	json j;
	int gameObjectCount = Engine::gameObjectCount;
	for (int goI = 0; goI < gameObjectCount; goI++)
	{
		auto go = Engine::gameObjects[goI];
		std::string goName = go->name;
		std::string goId = std::to_string(go->GetUniqueId());

		j["GameObjects"][goId]["Transform"]["Values"] = ReflectiveToJson(*go->GetTransform().get());
		j["GameObjects"][goId]["Values"] = ReflectiveToJson(*go.get());

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
			j["GameObjects"][goId]["Components"][compId]["Values"] = ReflectiveToJson((*component.get()));
		}
	}

	std::string s = j.dump(2);
	// std::cout << s << std::endl;
	FileSystem::fileSystem->DeleteFile("scene2.txt");
	File* file = new File("scene2.txt");
	file->Open(true);
	file->Write(s);
	file->Close();
	delete file;
}

#pragma endregion