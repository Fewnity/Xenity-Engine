// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "main_bar_menu.h"

#include <imgui/imgui.h>

#include <editor/ui/editor_ui.h>
#include <editor/compiler.h>
#include <editor/command/commands/create.h>
#include <editor/command/command_manager.h>
#include <editor/ui/menus/engine_settings_menu.h>
#include <editor/ui/menus/project_settings_menu.h>
#include <editor/ui/menus/lighting_menu.h>
#include <editor/ui/menus/docker_config_menu.h>
#include <editor/ui/menus/build_settings_menu.h>

#include <engine/engine.h>
#include <engine/class_registry/class_registry.h>
#include <engine/scene_management/scene_manager.h>
#include <engine/asset_management/project_manager.h>
#include <engine/tools/shape_spawner.h>
#include <engine/graphics/ui/text_renderer.h>
#include <engine/graphics/2d_graphics/sprite_renderer.h>
#include <engine/test_component.h>
#include <engine/graphics/camera.h>
#include <engine/graphics/2d_graphics/tile_map.h>
#include <engine/graphics/3d_graphics/mesh_renderer.h>
#include <engine/lighting/lighting.h>
#include <engine/graphics/ui/text_mesh.h>
#include <engine/graphics/ui/canvas.h>
#include <engine/audio/audio_source.h>
#include <engine/debug/debug.h>
#include <engine/physics/rigidbody.h>
#include <engine/physics/box_collider.h>
#include <engine/particle_system/particle_system.h>
#include "about_menu.h"
#include <engine/graphics/2d_graphics/billboard_renderer.h>
#include <engine/graphics/3d_graphics/lod.h>
#include <engine/game_elements/rect_transform.h>
#include "engine_asset_manager_menu.h"

void MainBarMenu::Init()
{
}

template<typename T>
inline void MainBarMenu::AddComponentToSelectedGameObject()
{
	const std::vector<std::weak_ptr<GameObject>>& selectedGameObjects = Editor::GetSelectedGameObjects();
	for (const std::weak_ptr<GameObject>& currentGameObject : selectedGameObjects)
	{
		if(!currentGameObject.lock())
			continue;

		auto command = std::make_shared<InspectorAddComponentCommand>(*currentGameObject.lock(), ClassRegistry::GetClassInfo<T>()->name);
		CommandManager::AddCommandAndExecute(command);

		std::shared_ptr<Component> newComponent = FindComponentById(command->componentId);

		if (std::shared_ptr<Collider> boxCollider = std::dynamic_pointer_cast<Collider>(newComponent))
			boxCollider->SetDefaultSize();
	}
}

template <typename T>
std::shared_ptr<T> MainBarMenu::CreateGameObjectWithComponent(const std::string& gameObjectName)
{
	auto command = std::make_shared<InspectorCreateGameObjectCommand>(std::vector<std::weak_ptr<GameObject>>(), 0);
	CommandManager::AddCommandAndExecute(command);
	std::shared_ptr<GameObject> createdGameObject = FindGameObjectById(command->createdGameObjects[0]);
	createdGameObject->SetName(Editor::GetIncrementedGameObjectName(gameObjectName));

	auto componentCommand = std::make_shared<InspectorAddComponentCommand>(*createdGameObject, ClassRegistry::GetClassInfo<T>()->name);
	CommandManager::AddCommandAndExecute(componentCommand);
	std::shared_ptr<Component> newComponent = FindComponentById(componentCommand->componentId);

	/*auto command2 = std::make_shared<InspectorAddComponentCommand<T>>(command->createdGameObjects[0].lock());
	CommandManager::AddCommand(command2);
	command2->Execute();

	return command2->newComponent.lock();*/
	return std::dynamic_pointer_cast<T>(newComponent);
}

bool MainBarMenu::DrawImageButton(const bool enabled, const Texture& texture)
{
	if (!enabled)
		ImGui::BeginDisabled();
	const bool clicked = ImGui::ImageButton(EditorUI::GenerateItemId().c_str(), (ImTextureID)(size_t)EditorUI::GetTextureId(texture), ImVec2(18, 18), ImVec2(0.005f, 0.005f), ImVec2(0.995f, 0.995f));
	if (!enabled)
		ImGui::EndDisabled();
	return clicked;
}

void MainBarMenu::Draw()
{
	const size_t selectedGameObjectCount = Editor::GetSelectedGameObjects().size();
	const bool hasSelectedGameObject = selectedGameObjectCount != 0;
	const bool hasOneSelectedGameObject = selectedGameObjectCount == 1;

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::BeginMainMenuBar();
	bool isGameStopped = GameplayManager::GetGameState() == GameState::Stopped;
	if (ImGui::BeginMenu("File")) // ----------------------------------- Draw File menu
	{
		if (ImGui::MenuItem("New Scene", nullptr, nullptr, isGameStopped))
		{
			SceneManager::CreateEmptyScene();
		}
		if (ImGui::MenuItem("Open Scene", nullptr, nullptr, isGameStopped))
		{
			Debug::PrintWarning("(File/Open Scene) Unimplemented button", true);
		}
		if (ImGui::MenuItem("Save Scene", nullptr, nullptr, isGameStopped))
		{
			SceneManager::SaveScene(SaveSceneType::SaveSceneToFile);
		}
		ImGui::Separator();
		if (ImGui::MenuItem("Build Settings", nullptr, nullptr))
		{
			Editor::GetMenu<BuildSettingsMenu>()->SetActive(true);
			Editor::GetMenu<BuildSettingsMenu>()->Focus();
		}
		ImGui::Separator();
		if (ImGui::MenuItem("Close project", nullptr, nullptr, isGameStopped))
		{
			ProjectManager::UnloadProject();
			Editor::currentMenu = MenuGroup::Menu_Select_Project;
		}
		if (ImGui::MenuItem("Exit", nullptr, nullptr, isGameStopped))
		{
			Engine::Quit();
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Edit")) // ----------------------------------- Draw Edit menu
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
		if (ImGui::MenuItem("Create Empty Parent", nullptr, nullptr, hasOneSelectedGameObject))
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
		if (ImGui::BeginMenu("3D Objects"))
		{
			if (ImGui::MenuItem("Cube"))
			{
				Editor::SetSelectedGameObject(ShapeSpawner::SpawnCube());
			}
			if (ImGui::MenuItem("Sphere"))
			{
				Editor::SetSelectedGameObject(ShapeSpawner::SpawnSphere());
			}
			if (ImGui::MenuItem("Cylinder"))
			{
				Editor::SetSelectedGameObject(ShapeSpawner::SpawnCylinder());
			}
			if (ImGui::MenuItem("Plane"))
			{
				Editor::SetSelectedGameObject(ShapeSpawner::SpawnPlane());
			}
			if (ImGui::MenuItem("Cone"))
			{
				Editor::SetSelectedGameObject(ShapeSpawner::SpawnCone());
			}
			if (ImGui::MenuItem("Donut"))
			{
				Editor::SetSelectedGameObject(ShapeSpawner::SpawnDonut());
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("2D"))
		{
			if (ImGui::MenuItem("Sprite Renderer"))
			{
				std::shared_ptr<SpriteRenderer> spriteRenderer = CreateGameObjectWithComponent<SpriteRenderer>("Sprite");
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Light"))
		{
			if (ImGui::MenuItem("Ambient Light"))
			{
				std::shared_ptr<Light> light = CreateGameObjectWithComponent<Light>("Ambient Light");
				light->SetupAmbientLight(Color::CreateFromRGBFloat(1, 1, 1), 0.2f);
			}
			if (ImGui::MenuItem("Directional Light"))
			{
				std::shared_ptr<Light> light = CreateGameObjectWithComponent<Light>("Directional Light");
				light->SetupDirectionalLight(Color::CreateFromRGBFloat(1, 1, 1), 1);
			}
			if (ImGui::MenuItem("Spot Light"))
			{
				std::shared_ptr<Light> light = CreateGameObjectWithComponent<Light>("Spot Light");
				light->SetupSpotLight(Color::CreateFromRGBFloat(1, 1, 1), 1, 10, 60, 0.5f);
			}
			if (ImGui::MenuItem("Point Light"))
			{
				std::shared_ptr<Light> light = CreateGameObjectWithComponent<Light>("Point Light");
				light->SetupPointLight(Color::CreateFromRGBFloat(1, 1, 1), 1, 10);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Audio"))
		{
			if (ImGui::MenuItem("Audio Source"))
			{
				std::shared_ptr<AudioSource> audioSource = CreateGameObjectWithComponent<AudioSource>("Audio Source");
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Camera"))
		{
			if (ImGui::MenuItem("2D Camera"))
			{
				std::shared_ptr<Camera> camera = CreateGameObjectWithComponent<Camera>("Camera");
				camera->SetProjectionType(ProjectionTypes::Orthographic);
			}
			if (ImGui::MenuItem("3D Camera"))
			{
				std::shared_ptr<Camera> camera = CreateGameObjectWithComponent<Camera>("Camera");
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
				AddComponentToSelectedGameObject<TextMesh>();
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
			if (ImGui::MenuItem("Lod", nullptr, nullptr, hasSelectedGameObject))
			{
				AddComponentToSelectedGameObject<Lod>();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Effects"))
		{
			if (ImGui::MenuItem("Particle System", nullptr, nullptr, hasSelectedGameObject))
			{
				AddComponentToSelectedGameObject<ParticleSystem>();
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
			if (ImGui::MenuItem("Canvas", nullptr, nullptr, hasSelectedGameObject))
			{
				AddComponentToSelectedGameObject<Canvas>();
			}
			if (ImGui::MenuItem("Rect Transform", nullptr, nullptr, hasSelectedGameObject))
			{
				AddComponentToSelectedGameObject<RectTransform>();
			}
			if (ImGui::MenuItem("Text Renderer", nullptr, nullptr, hasSelectedGameObject))
			{
				AddComponentToSelectedGameObject<TextRenderer>();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("2D"))
		{
			if (ImGui::MenuItem("Sprite Renderer", nullptr, nullptr, hasSelectedGameObject))
			{
				AddComponentToSelectedGameObject<SpriteRenderer>();
			}
			if (ImGui::MenuItem("Billboard Renderer", nullptr, nullptr, hasSelectedGameObject))
			{
				AddComponentToSelectedGameObject<BillboardRenderer>();
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
			const std::vector<std::string> componentNames = ClassRegistry::GetComponentNames();
			const size_t componentCount = componentNames.size();
			for (size_t i = 0; i < componentCount; i++)
			{
				if (ImGui::MenuItem(componentNames[i].c_str(), nullptr, nullptr, hasSelectedGameObject))
				{
					const std::vector<std::weak_ptr<GameObject>>& selectedGameObjects = Editor::GetSelectedGameObjects();
					for (const std::weak_ptr<GameObject>& currentGameObject : selectedGameObjects)
					{
						if (currentGameObject.lock()) 
						{
							std::shared_ptr<Component> newComponent = ClassRegistry::AddComponentFromName(componentNames[i], *currentGameObject.lock());
							if (std::shared_ptr<Collider> boxCollider = std::dynamic_pointer_cast<Collider>(newComponent))
								boxCollider->SetDefaultSize();
						}
					}
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
				Editor::AddMenu("GameMenu", true);
			}
			if (ImGui::MenuItem("Scene"))
			{
				Editor::AddMenu("SceneMenu", true);
			}
			if (ImGui::MenuItem("Inspector"))
			{
				Editor::AddMenu("InspectorMenu", true);
			}
			if (ImGui::MenuItem("Profiler"))
			{
				Editor::AddMenu("ProfilerMenu", true);
			}
			if (ImGui::MenuItem("File Explorer"))
			{
				Editor::AddMenu("FileExplorerMenu", true);
			}
			if (ImGui::MenuItem("Hierarchy"))
			{
				Editor::AddMenu("HierarchyMenu", true);
			}
			if (ImGui::MenuItem("Console"))
			{
				Editor::AddMenu("ConsoleMenu", true);
			}
			/*if (ImGui::MenuItem("Sprite Editor"))
			{
				Editor::AddMenu("SpriteEditorMenu", true);
			}*/
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
			Editor::GetMenu<ProjectSettingsMenu>()->Focus();
		}
		if (ImGui::MenuItem("Lighting Settings"))
		{
			Editor::GetMenu<LightingMenu>()->SetActive(true);
			Editor::GetMenu<LightingMenu>()->Focus();
		}
		ImGui::Separator();
		if (ImGui::MenuItem("Docker Configuration"))
		{
			Editor::GetMenu<DockerConfigMenu>()->SetActive(true);
			Editor::GetMenu<DockerConfigMenu>()->Focus();
		}

		ImGui::EndMenu();
	}
#if defined(DEBUG)
	if (ImGui::BeginMenu("Engine Setup")) // ----------------------------------- Draw Other menu
	{
		if (ImGui::MenuItem("Engine Asset Manager"))
		{
			Editor::GetMenu<EngineAssetManagerMenu>()->SetActive(true);
			Editor::GetMenu<EngineAssetManagerMenu>()->Focus();
		}
		ImGui::EndMenu();
	}
#endif
	if (ImGui::BeginMenu("Help")) // ----------------------------------- Draw Help menu
	{
		if (ImGui::MenuItem("Documentation"))
		{
			Editor::OpenLinkInWebBrowser("https://github.com/Fewnity/Xenity-Engine/tree/crossplatform/Doc");
		}
		if (ImGui::MenuItem("Project's GitHub"))
		{
			Editor::OpenLinkInWebBrowser("https://github.com/Fewnity/Xenity-Engine");
		}
		if (ImGui::MenuItem("About Xenity Engine"))
		{
			Editor::GetMenu<AboutMenu>()->SetActive(true);
			Editor::GetMenu<AboutMenu>()->Focus();
		}
		ImGui::EndMenu();
	}
	height = ImGui::GetWindowHeight();
	ImGui::EndMainMenuBar();

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + height));
	ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 4));
	ImGuiStyle& style = ImGui::GetStyle();
	const float oldBorderSize = style.WindowBorderSize;
	style.WindowBorderSize = 0;
	ImGui::Begin("undermainbar", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBringToFrontOnFocus);

	height += ImGui::GetWindowHeight();
	
	const float oldFramePadding = style.FramePadding.x;
	style.FramePadding.x = 14;
	ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x / 2.0f - (18 * 3 + style.ItemSpacing.x * 2 + style.FramePadding.x * 6) / 2.0f);
	ImGui::BeginGroup();
	const bool playClicked = DrawImageButton(GameplayManager::GetGameState() != GameState::Playing, *EditorUI::icons[(int)IconName::Icon_Play]);
	ImGui::SameLine();
	const bool pauseClicked = DrawImageButton(GameplayManager::GetGameState() != GameState::Stopped, *EditorUI::icons[(int)IconName::Icon_Pause]);
	ImGui::SameLine();
	const bool stopClicked = DrawImageButton(GameplayManager::GetGameState() != GameState::Stopped, *EditorUI::icons[(int)IconName::Icon_Stop]);
	ImGui::EndGroup();
	style.FramePadding.x = oldFramePadding;

	if (playClicked)
	{
		GameplayManager::SetGameState(GameState::Playing, true);
	}
	else if (pauseClicked)
	{
		GameplayManager::SetGameState(GameState::Paused, true);
	}
	else if (stopClicked)
	{
		GameplayManager::SetGameState(GameState::Stopped, true);
	}

	ImGui::End();
	style.WindowBorderSize = oldBorderSize;
	ImGui::PopStyleVar();
}
