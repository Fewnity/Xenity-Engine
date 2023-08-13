#if defined(EDITOR)
#include "editor.h"
#include "../xenity.h"
#include "../xenity_editor.h"

#include "ui/menus/project_settings_menu.h"
#include "ui/menus/engine_settings_menu.h"
#include "ui/menus/file_explorer_menu.h"
#include "ui/menus/hierarchy_menu.h"
#include "ui/menus/inspector_menu.h"
#include "ui/menus/main_bar_menu.h"
#include "ui/menus/profiler_menu.h"
#include "ui/menus/game_menu.h"
#include "ui/menus/scene_menu.h"
#include "ui/menus/compiling_menu.h"

#include <functional>
#include "../engine/class_registry/class_registry.h"
#include "../engine/file_system/file_reference.h"
#include "../engine/file_system/mesh_loader/wavefront_loader.h"
#include "../engine/asset_management/project_manager.h"
#include "../engine/reflection/reflection_utils.h"

using json = nlohmann::json;

std::weak_ptr<GameObject> Editor::cameraGO;

ProjectSettingsMenu* Editor::projectSettings = nullptr;
EngineSettingsMenu* Editor::engineSettings = nullptr;
FileExplorerMenu* Editor::fileExplorer = nullptr;
HierarchyMenu* Editor::hierarchy = nullptr;
InspectorMenu* Editor::inspector = nullptr;
MainBarMenu* Editor::mainBar = nullptr;
ProfilerMenu* Editor::profiler = nullptr;
GameMenu* Editor::gameMenu = nullptr;
SceneMenu* Editor::sceneMenu = nullptr;
CompilingMenu* Editor::compilingMenu = nullptr;

void Editor::Start()
{
	projectSettings = new ProjectSettingsMenu();
	engineSettings = new EngineSettingsMenu();
	fileExplorer = new FileExplorerMenu();
	hierarchy = new HierarchyMenu();
	inspector = new InspectorMenu();
	mainBar = new MainBarMenu();
	profiler = new ProfilerMenu();
	gameMenu = new GameMenu();
	sceneMenu = new SceneMenu();
	compilingMenu = new CompilingMenu();

	projectSettings->Init();
	engineSettings->Init();
	fileExplorer->Init();
	hierarchy->Init();
	inspector->Init();
	mainBar->Init();
	profiler->Init();
	gameMenu->Init();
	sceneMenu->Init();

	cameraGO = CreateGameObjectEditor("Camera");
	auto camera = cameraGO.lock()->AddComponent<Camera>();
	camera->SetNearClippingPlane(0.01f);
	camera->SetFarClippingPlane(500);
	camera->SetProjectionSize(5.0f);
	camera->SetFov(70);
	camera->isEditor = true;
	camera->GetTransform()->SetPosition(Vector3(0, 1, 0));
}

void Editor::Update()
{
	auto cameraTrans = cameraGO.lock()->GetTransform();
	Vector3 rot = cameraTrans->GetRotation();
	Vector3 pos = cameraTrans->GetPosition();

	if (InputSystem::GetKey(MOUSE_RIGHT) && sceneMenu->isHovered)
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

	if ((InputSystem::GetKey(LEFT_CONTROL) && (/*InputSystem::GetKeyDown(C) || */ InputSystem::GetKeyDown(D))))
	{
		if (Engine::selectedGameObject.lock())
			DuplicateGameObject(Engine::selectedGameObject.lock());
	}
}

void Editor::Draw()
{
	EditorUI::NewFrame();
	if (EditorUI::showProjectsSettings) 
	{
		projectSettings->Draw();
	}
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
	compilingMenu->Draw();
	gameMenu->Draw();
	sceneMenu->Draw();
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

void Editor::SaveScene()
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
	FileSystem::fileSystem->DeleteFile("scene2.txt");
	File* file = new File("scene2.txt");
	file->Open(true);
	file->Write(s);
	file->Close();
	delete file;
}

void Editor::DuplicateGameObject(std::shared_ptr<GameObject> gameObject)
{
	std::shared_ptr<GameObject> newGameObject = CreateGameObject(gameObject->name);

	auto selectedGO = Engine::selectedGameObject.lock();
	if (selectedGO)
	{
		if(selectedGO->parent.lock())
			newGameObject->SetParent(selectedGO->parent);
	}

	int componentCount = gameObject->components.size();
	for (int i = 0; i < componentCount; i++)
	{
		std::shared_ptr<Component> newComponent = ClassRegistry::AddComponentFromName(gameObject->components[i]->GetComponentName(), newGameObject);
		auto newReflection = newComponent->GetReflection();
		auto reflectionToCopy = gameObject->components[i]->GetReflection();

		json copiedValues;
		copiedValues["Values"] = ReflectionUtils::MapToJson(reflectionToCopy);
		ReflectionUtils::JsonToMap(newReflection, copiedValues);
	}
}

#pragma endregion
#endif