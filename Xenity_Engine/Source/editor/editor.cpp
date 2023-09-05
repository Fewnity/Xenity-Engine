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
#include "ui/menus/select_project_menu.h"
#include "ui/menus/create_project_menu.h"
#include "ui/menus/lighting_menu.h"

#include <functional>
#include "../engine/class_registry/class_registry.h"
#include "../engine/file_system/file_reference.h"
#include "../engine/file_system/mesh_loader/wavefront_loader.h"
#include "../engine/asset_management/project_manager.h"
#include "../engine/reflection/reflection_utils.h"
#include "../engine/scene_management/scene_manager.h"

using json = nlohmann::json;

std::weak_ptr<GameObject> Editor::cameraGO;

MenuNames Editor::currentMenu = Menu_Select_Project;

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
SelectProjectMenu* Editor::selectProjectMenu = nullptr;
CreateProjectMenu* Editor::createProjectMenu = nullptr;
LightingMenu* Editor::lightingMenu = nullptr;

void Editor::Init()
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
	selectProjectMenu = new SelectProjectMenu();
	createProjectMenu = new CreateProjectMenu();
	lightingMenu = new LightingMenu();

	projectSettings->Init();
	engineSettings->Init();
	fileExplorer->Init();
	hierarchy->Init();
	inspector->Init();
	mainBar->Init();
	profiler->Init();
	gameMenu->Init();
	sceneMenu->Init();
	selectProjectMenu->Init();
	createProjectMenu->Init();
	lightingMenu->Init();

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
	if (sceneMenu->isFocused)
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
		float up = 0;
		float side = 0;
		if (InputSystem::GetKey(UP))
			fwd = -1;
		else if (InputSystem::GetKey(DOWN))
			fwd = 1;

		if (InputSystem::GetKey(RIGHT))
			side = 1;
		else if (InputSystem::GetKey(LEFT))
			side = -1;

		if(sceneMenu->isHovered)
			fwd -= InputSystem::mouseWheel * 6;

		if (InputSystem::GetKey(MOUSE_MIDDLE)) 
		{
			up += InputSystem::InputSystem::mouseSpeed.y * 100;
			side -= InputSystem::InputSystem::mouseSpeed.x * 100;
		}

		pos -= cameraTrans->GetForward() * (fwd / 7.0f) * Time::GetDeltaTime() * 30;
		pos -= cameraTrans->GetLeft() * (side / 7.0f) * Time::GetDeltaTime() * 30;
		pos -= cameraTrans->GetUp() * (up / 7.0f) * Time::GetDeltaTime() * 30;

		cameraTrans->SetPosition(pos);
		cameraTrans->SetRotation(rot);
	}

	if ((InputSystem::GetKey(LEFT_CONTROL) && (/*InputSystem::GetKeyDown(C) || */ InputSystem::GetKeyDown(D))))
	{
		if (Engine::selectedGameObject.lock())
			DuplicateGameObject(Engine::selectedGameObject.lock());
	}

	if (InputSystem::GetKey(DELETE))
	{
		if (sceneMenu->isFocused || hierarchy->isFocused)
		{
			if (Engine::selectedGameObject.lock())
				Destroy(Engine::selectedGameObject.lock());
		}
	}

	if (Engine::GetGameState() == GameState::Stopped)
	{
		if ((InputSystem::GetKey(LEFT_CONTROL) && InputSystem::GetKeyDown(S)))
		{
			SceneManager::SaveScene(SaveSceneToFile);
		}
	}
}

void Editor::Draw()
{
	EditorUI::NewFrame();

	if (currentMenu == Menu_Create_Project) 
	{
		createProjectMenu->Draw();
	}
	else if (currentMenu == Menu_Select_Project) 
	{
		selectProjectMenu->Draw();
	}
	else 
	{
		EditorUI::SetRoundedCorner(10);
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
		lightingMenu->Draw();
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

void Editor::DuplicateGameObject(std::shared_ptr<GameObject> gameObject)
{
	std::shared_ptr<GameObject> newGameObject = CreateGameObject(gameObject->name);

	auto selectedGO = Engine::selectedGameObject.lock();
	if (selectedGO)
	{
		if (selectedGO->parent.lock())
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
		ReflectionUtils::JsonToMap(copiedValues, newReflection);
		newComponent->OnReflectionUpdated();
	}
}

#pragma endregion
#endif