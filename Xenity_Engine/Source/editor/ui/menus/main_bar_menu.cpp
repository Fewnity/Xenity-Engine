#include "main_bar_menu.h"
#include "compiling_menu.h"

#include <imgui/imgui.h>

#include <Windows.h>
#include <Commdlg.h>
#include <ShObjIdl.h>

#include <editor/ui/editor_ui.h>
#include <editor/compiler.h>
#include <editor/command/commands/inspector_commands.h>
#include <editor/command/command_manager.h>
#include <editor/ui/menus/engine_settings_menu.h>
#include <editor/ui/menus/project_settings_menu.h>
#include <editor/ui/menus/lighting_menu.h>
#include <editor/ui/menus/profiler_menu.h>
#include <editor/ui/menus/game_menu.h>
#include <editor/ui/menus/scene_menu.h>
#include <editor/ui/menus/inspector_menu.h>
#include <editor/ui/menus/file_explorer_menu.h>
#include <editor/ui/menus/hierarchy_menu.h>

#include <engine/engine.h>
#include <engine/class_registry/class_registry.h>
#include <engine/dynamic_lib/dynamic_lib.h>
#include <engine/scene_management/scene_manager.h>
#include <engine/asset_management/project_manager.h>
#include <engine/tools/shape_spawner.h>
#include <engine/graphics/ui/text_renderer_canvas.h>
#include <engine/graphics/2d_graphics/sprite_renderer.h>
#include <engine/test_component.h>
#include <engine/graphics/camera.h>
#include <engine/graphics/2d_graphics/tile_map.h>
#include <engine/graphics/3d_graphics/mesh_renderer.h>
#include <engine/lighting/lighting.h>
#include <engine/graphics/ui/text_renderer.h>
#include <engine/audio/audio_source.h>
#include <engine/debug/debug.h>
#include <engine/physics/rigidbody.h>
#include <engine/physics/box_collider.h>
#include "about_menu.h"

void MainBarMenu::Init()
{
}

template<typename T>
inline void MainBarMenu::AddComponentToSelectedGameObject()
{
	auto command = std::make_shared<InspectorAddComponentCommand<T>>(Editor::GetSelectedGameObject());
	CommandManager::AddCommand(command);
	command->Execute();
}

template <typename T>
std::shared_ptr<T> CreateEmptyWithComponent(const std::string& name) 
{
	auto command = std::make_shared<InspectorCreateGameObjectCommand>(std::weak_ptr<GameObject>(), 0);
	CommandManager::AddCommand(command);
	command->Execute();
	command->createdGameObject.lock()->name = Editor::GetIncrementedGameObjectName(name);

	auto command2 = std::make_shared<InspectorAddComponentCommand<T>>(command->createdGameObject.lock());
	CommandManager::AddCommand(command2);
	command2->Execute();

	return command2->newComponent.lock();
}

void MainBarMenu::Draw()
{
	bool hasSelectedGameObject = Editor::GetSelectedGameObject() != nullptr;

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::BeginMainMenuBar();

	if (ImGui::BeginMenu("File")) // ----------------------------------- Draw File menu
	{
		if (ImGui::MenuItem("New Scene"))
		{
			SceneManager::CreateEmptyScene();
		}
		if (ImGui::MenuItem("Open Scene"))
		{
			Debug::PrintWarning("(File/Open Scene) Unimplemented button");
		}
		if (ImGui::MenuItem("Save Scene"))
		{
			SceneManager::SaveScene(SaveSceneToFile);
		}
		ImGui::Separator();
		if (ImGui::MenuItem("Build for Windows"))
		{
			std::string exportPath = EditorUI::OpenFolderDialog("Select an export folder", "");
			if (!exportPath.empty())
				Compiler::CompileGameThreaded(Platform::P_Windows, BuildType::BuildGame, exportPath);
		}
		if (ImGui::MenuItem("Build for PSP"))
		{
			std::string exportPath = EditorUI::OpenFolderDialog("Select an export folder", "");
			if (!exportPath.empty())
				Compiler::CompileGameThreaded(Platform::P_PSP, BuildType::BuildGame, exportPath);
		}
		if (ImGui::MenuItem("Build for PsVita"))
		{
			std::string exportPath = EditorUI::OpenFolderDialog("Select an export folder", "");
			if (!exportPath.empty())
				Compiler::CompileGameThreaded(Platform::P_PsVita, BuildType::BuildGame, exportPath);
		}
		if (ImGui::MenuItem("Build And Run on Windows"))
		{
			std::string exportPath = EditorUI::OpenFolderDialog("Select an export folder", "");
			if (!exportPath.empty())
				Compiler::CompileGameThreaded(Platform::P_Windows, BuildType::BuildAndRunGame, exportPath);
		}
		if (ImGui::MenuItem("Build And Run on PSP"))
		{
			std::string exportPath = EditorUI::OpenFolderDialog("Select an export folder", "");
			if (!exportPath.empty())
				Compiler::CompileGameThreaded(Platform::P_PSP, BuildType::BuildAndRunGame, exportPath);
		}
		ImGui::Separator();
		if (ImGui::MenuItem("Close project"))
		{
			ProjectManager::UnloadProject();
			Editor::currentMenu = Menu_Select_Project;
		}
		if (ImGui::MenuItem("Exit"))
		{
			Engine::Quit();
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Edit")) // ----------------------------------- Draw File menu
	{
		if (ImGui::MenuItem("Unselect"))
		{
			Editor::SetSelectedFileReference(nullptr);
			Editor::SetSelectedGameObject(nullptr);
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("GameObject")) // ----------------------------------- Draw GameObject menu
	{
		if (ImGui::MenuItem("Create Empty Parent", nullptr, nullptr, hasSelectedGameObject))
		{
			Editor::CreateEmptyParent();
		}
		if (ImGui::MenuItem("Create Empty Child", nullptr, nullptr, hasSelectedGameObject))
		{
			Editor::CreateEmptyChild();
		}
		if (ImGui::MenuItem("Create Empty"))
		{
			Editor::CreateEmpty();
		}
		/*if (ImGui::BeginMenu("3D Objects"))
		{
			if (ImGui::MenuItem("Cube"))
			{
				ShapeSpawner::SpawnCube();
			}
			if (ImGui::MenuItem("Sphere"))
			{
				ShapeSpawner::SpawnSphere();
			}
			if (ImGui::MenuItem("Cylinder"))
			{
				ShapeSpawner::SpawnCylinder();
			}
			if (ImGui::MenuItem("Plane"))
			{
				ShapeSpawner::SpawnPlane();
			}
			if (ImGui::MenuItem("Cone"))
			{
				ShapeSpawner::SpawnCone();
			}
			if (ImGui::MenuItem("Donut"))
			{
				ShapeSpawner::SpawnDonut();
			}
			ImGui::EndMenu();
		}*/
		if (ImGui::BeginMenu("Light"))
		{
			if (ImGui::MenuItem("Directional Light"))
			{
				std::shared_ptr<Light> light = CreateEmptyWithComponent<Light>("Directional Light");
				light->SetupDirectionalLight(Color::CreateFromRGBFloat(1, 1, 1), 1);
			}
			if (ImGui::MenuItem("Spot Light", 0, false, false))
			{
				std::shared_ptr<Light> light = CreateEmptyWithComponent<Light>("Spot Light");
				light->SetupSpotLight(Color::CreateFromRGBFloat(1, 1, 1), 1, 10, 60, 0.5f);
			}
			if (ImGui::MenuItem("Point Light"))
			{
				std::shared_ptr<Light> light = CreateEmptyWithComponent<Light>("Point Light");
				light->SetupPointLight(Color::CreateFromRGBFloat(1,1,1), 1, 10);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Audio"))
		{
			if (ImGui::MenuItem("Audio Source"))
			{
				std::shared_ptr<AudioSource> audioSource = CreateEmptyWithComponent<AudioSource>("Audio Source");
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Camera"))
		{
			if (ImGui::MenuItem("2D Camera"))
			{
				std::shared_ptr<Camera> camera = CreateEmptyWithComponent<Camera>("Camera");
				camera->SetProjectionType(ProjectionTypes::Orthographic);
			}
			if (ImGui::MenuItem("3D Camera"))
			{
				std::shared_ptr<Camera> camera = CreateEmptyWithComponent<Camera>("Camera");
				camera->SetProjectionType(ProjectionTypes::Perspective);
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Component")) // ----------------------------------- Draw Component menu
	{
		if (ImGui::BeginMenu("Mesh"))
		{
			if (ImGui::MenuItem("Mesh Renderer", nullptr, nullptr, hasSelectedGameObject))
			{
				AddComponentToSelectedGameObject<MeshRenderer>();
			}
			if (ImGui::MenuItem("Text Mesh", nullptr, nullptr, hasSelectedGameObject))
			{
				AddComponentToSelectedGameObject<TextRenderer>();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Audio"))
		{
			if (ImGui::MenuItem("Audio Source", nullptr, nullptr, hasSelectedGameObject))
			{
				AddComponentToSelectedGameObject<AudioSource>();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Rendering"))
		{
			if (ImGui::MenuItem("Camera", nullptr, nullptr, hasSelectedGameObject))
			{
				AddComponentToSelectedGameObject<Camera>();
			}
			if (ImGui::MenuItem("Light", nullptr, nullptr, hasSelectedGameObject))
			{
				AddComponentToSelectedGameObject<Light>();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Physics"))
		{
			if (ImGui::MenuItem("RigidBody", nullptr, nullptr, hasSelectedGameObject))
			{
				AddComponentToSelectedGameObject<RigidBody>();
			}
			if (ImGui::MenuItem("Box Collider", nullptr, nullptr, hasSelectedGameObject))
			{
				AddComponentToSelectedGameObject<BoxCollider>();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Tilemap"))
		{
			if (ImGui::MenuItem("Tilemap", nullptr, nullptr, hasSelectedGameObject))
			{
				AddComponentToSelectedGameObject<Tilemap>();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("UI"))
		{
			if (ImGui::MenuItem("Text Renderer", nullptr, nullptr, hasSelectedGameObject))
			{
				AddComponentToSelectedGameObject<TextRendererCanvas>();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("2D"))
		{
			if (ImGui::MenuItem("Sprite Renderer", nullptr, nullptr, hasSelectedGameObject))
			{
				AddComponentToSelectedGameObject<SpriteRenderer>();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Other"))
		{
			if (ImGui::MenuItem("Test Component", nullptr, nullptr, hasSelectedGameObject))
			{
				AddComponentToSelectedGameObject<TestComponent>();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("All"))
		{
			std::vector<std::string> componentNames = ClassRegistry::GetComponentNames();
			size_t componentCount = componentNames.size();
			for (size_t i = 0; i < componentCount; i++)
			{
				if (ImGui::MenuItem(componentNames[i].c_str(), nullptr, nullptr, hasSelectedGameObject))
				{
					ClassRegistry::AddComponentFromName(componentNames[i], Editor::GetSelectedGameObject());
				}
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Game")) // ----------------------------------- Draw Game menu
	{
		if (ImGui::MenuItem("Play Game", nullptr, nullptr, GameplayManager::GetGameState() != GameState::Playing))
		{
			GameplayManager::SetGameState(GameState::Playing, true);
		}
		if (ImGui::MenuItem("Pause Game", nullptr, nullptr, GameplayManager::GetGameState() != GameState::Stopped))
		{
			GameplayManager::SetGameState(GameState::Paused, true);
		}
		if (ImGui::MenuItem("Stop Game", nullptr, nullptr, GameplayManager::GetGameState() != GameState::Stopped))
		{
			GameplayManager::SetGameState(GameState::Stopped, true);
		}
		ImGui::Separator();
		if (ImGui::MenuItem("Compile Game"))
		{
			Compiler::HotReloadGame();
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Window")) // ----------------------------------- Draw Window menu
	{
		if (ImGui::BeginMenu("General"))
		{
			if (ImGui::MenuItem("Game"))
			{
				Editor::AddMenu<GameMenu>(true);
			}
			if (ImGui::MenuItem("Scene"))
			{
				Editor::AddMenu<SceneMenu>(true);
			}
			if (ImGui::MenuItem("Inspector"))
			{
				Editor::AddMenu<InspectorMenu>(true);
			}
			if (ImGui::MenuItem("Profiler"))
			{
				Editor::AddMenu<ProfilerMenu>(true);
			}
			if (ImGui::MenuItem("File Explorer"))
			{
				Editor::AddMenu<FileExplorerMenu>(true);
			}
			if (ImGui::MenuItem("Hierarchy"))
			{
				Editor::AddMenu<HierarchyMenu>(true);
			}
			ImGui::EndMenu();
		}
		ImGui::Separator();
		if (ImGui::MenuItem("Engine Settings"))
		{
			Editor::GetMenu<EngineSettingsMenu>()->SetActive(true);
			Editor::GetMenu<EngineSettingsMenu>()->Focus();
		}
		if (ImGui::MenuItem("Project Settings"))
		{
			Editor::GetMenu<ProjectSettingsMenu>()->SetActive(true);
		}
		if (ImGui::MenuItem("Lighting Settings"))
		{
			Editor::GetMenu<LightingMenu>()->SetActive(true);
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Help")) // ----------------------------------- Draw Window menu
	{
		if (ImGui::MenuItem("Documentation"))
		{
			ShellExecute(0, 0, L"https://github.com/Fewnity/Xenity-Engine/tree/crossplatform/Doc", 0, 0, SW_SHOW);
		}
		if (ImGui::MenuItem("Project's GitHub"))
		{
			ShellExecute(0, 0, L"https://github.com/Fewnity/Xenity-Engine", 0, 0, SW_SHOW);
		}
		if (ImGui::MenuItem("About Xenity Engine"))
		{
			Editor::GetMenu<AboutMenu>()->SetActive(true);
		}
		ImGui::EndMenu();
	}
	height = ImGui::GetWindowHeight();
	ImGui::EndMainMenuBar();
}
