#include "main_bar_menu.h"
#include <imgui/imgui.h>
#include "../../../xenity.h"
#include "../editor_ui.h"
#include "../../editor.h"
#include "../../../engine/class_registry/class_registry.h"
#include "../../../engine/dynamic_lib/dynamic_lib.h"
#include "compiling_menu.h"
#include "../../compiler.h"

#include <Windows.h>
#include <Commdlg.h>
#include <ShObjIdl.h>
#include "../../../engine/scene_management/scene_manager.h"
#include "../../../engine/asset_management/project_manager.h"

void MainBarMenu::Init()
{
}

static bool IsOpen = true;
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
		if (ImGui::MenuItem("Build for Windows"))
		{
			std::string exportPath = EditorUI::OpenFolderDialog("Select an export folder");
			if (!exportPath.empty())
				Compiler::CompileGameThreaded(Platform::P_Windows, BuildType::BuildGame, exportPath);
		}
		if (ImGui::MenuItem("Build for PSP"))
		{
			std::string exportPath = EditorUI::OpenFolderDialog("Select an export folder");
			if (!exportPath.empty())
				Compiler::CompileGameThreaded(Platform::P_PSP, BuildType::BuildGame, exportPath);
		}
		if (ImGui::MenuItem("Build for PsVita"))
		{
			std::string exportPath = EditorUI::OpenFolderDialog("Select an export folder");
			if (!exportPath.empty())
				Compiler::CompileGameThreaded(Platform::P_PsVita, BuildType::BuildGame, exportPath);
		}
		if (ImGui::MenuItem("Build And Run on Windows"))
		{
			std::string exportPath = EditorUI::OpenFolderDialog("Select an export folder");
			if (!exportPath.empty())
				Compiler::CompileGameThreaded(Platform::P_Windows, BuildType::BuildAndRunGame, exportPath);
		}
		if (ImGui::MenuItem("Build And Run on PSP"))
		{
			std::string exportPath = EditorUI::OpenFolderDialog("Select an export folder");
			if (!exportPath.empty())
				Compiler::CompileGameThreaded(Platform::P_PSP, BuildType::BuildAndRunGame, exportPath);
		}
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
		if (ImGui::BeginMenu("3D Objects"))
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
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Component")) // ----------------------------------- Draw Component menu
	{
		if (ImGui::BeginMenu("Mesh"))
		{
			if (ImGui::MenuItem("Mesh Renderer", nullptr, nullptr, hasSelectedGameObject))
			{
				Editor::AddComponentToSelection<MeshRenderer>();
			}
			if (ImGui::MenuItem("Text Mesh", nullptr, nullptr, hasSelectedGameObject))
			{
				Editor::AddComponentToSelection<TextRenderer>();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Audio"))
		{
			if (ImGui::MenuItem("Audio Source", nullptr, nullptr, hasSelectedGameObject))
			{
				Editor::AddComponentToSelection<AudioSource>();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Rendering"))
		{
			if (ImGui::MenuItem("Camera", nullptr, nullptr, hasSelectedGameObject))
			{
				Editor::AddComponentToSelection<Camera>();
			}
			if (ImGui::MenuItem("Light", nullptr, nullptr, hasSelectedGameObject))
			{
				Editor::AddComponentToSelection<Light>();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Tilemap"))
		{
			if (ImGui::MenuItem("Tilemap", nullptr, nullptr, hasSelectedGameObject))
			{
				Editor::AddComponentToSelection<Tilemap>();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("UI"))
		{
			if (ImGui::MenuItem("Text Renderer", nullptr, nullptr, hasSelectedGameObject))
			{
				Editor::AddComponentToSelection<TextRendererCanvas>();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("2D"))
		{
			if (ImGui::MenuItem("Sprite Renderer", nullptr, nullptr, hasSelectedGameObject))
			{
				Editor::AddComponentToSelection<SpriteRenderer>();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Other"))
		{
			if (ImGui::MenuItem("Test Component", nullptr, nullptr, hasSelectedGameObject))
			{
				Editor::AddComponentToSelection<TestComponent>();
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
		if (ImGui::MenuItem("Play Game"))
		{
			GameplayManager::SetGameState(GameState::Playing);
		}
		if (ImGui::MenuItem("Pause Game"))
		{
			GameplayManager::SetGameState(GameState::Paused);
		}
		if (ImGui::MenuItem("Stop Game"))
		{
			GameplayManager::SetGameState(GameState::Stopped);
		}
		if (ImGui::MenuItem("Hot Reload Game"))
		{
			Compiler::HotReloadGame();
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Window")) // ----------------------------------- Draw Window menu
	{
		ImGui::Checkbox(EditorUI::GenerateItemId().c_str(), &EditorUI::showProfiler);
		ImGui::SameLine();
		ImGui::Text("Show Profiler");
		ImGui::Checkbox(EditorUI::GenerateItemId().c_str(), &EditorUI::showEditor);
		ImGui::SameLine();
		ImGui::Text("Show Editor");
		if (ImGui::MenuItem("Engine Settings"))
		{
			EditorUI::showEngineSettings = true;
		}
		if (ImGui::MenuItem("Project Settings"))
		{
			EditorUI::showProjectsSettings = true;
		}
		if (ImGui::MenuItem("Lighting Settings"))
		{
			EditorUI::showLightingSettings = true;
		}
		ImGui::EndMenu();
	}

	ImGui::EndMainMenuBar();
}
