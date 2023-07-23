#include "main_bar_menu.h"
#include <imgui/imgui.h>
#include "../../../xenity.h"
#include "../editor_ui.h"
#include "../../editor.h"

void MainBarMenu::Init()
{
}

void MainBarMenu::Draw()
{
	bool hasSelectedGameObject = !Engine::selectedGameObject.expired();

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("File")) //Draw File menu
	{
		if (ImGui::MenuItem("New Scene"))
		{
			Debug::PrintWarning("(File/New Scene) Unimplemented button");
		}
		if (ImGui::MenuItem("Open Scene"))
		{
			Debug::PrintWarning("(File/Open Scene) Unimplemented button");
		}
		if (ImGui::MenuItem("Save Scene"))
		{
			Editor::SaveScene();
		}
		if (ImGui::MenuItem("Build Game"))
		{
			Debug::PrintWarning("(File/Build Game) Unimplemented button");
		}
		if (ImGui::MenuItem("Exit"))
		{
			Engine::Quit();
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("GameObject")) //Draw GameObject menu
	{
		if (ImGui::MenuItem("Create Empty Parent", nullptr, nullptr, hasSelectedGameObject))
		{
			Debug::PrintWarning("(GameObject/Create Empty Parent) Unimplemented button");
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
	if (ImGui::BeginMenu("Component")) //Draw Component menu
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
		if (ImGui::BeginMenu("Other"))
		{
			if (ImGui::MenuItem("Test Component", nullptr, nullptr, hasSelectedGameObject))
			{
				Editor::AddComponentToSelection<TestComponent>();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Game")) //Draw Game menu
	{
		if (ImGui::MenuItem("Play Game"))
		{
			Debug::PrintWarning("(File/Play Game) Unimplemented button");
		}
		if (ImGui::MenuItem("Pause Game"))
		{
			Debug::PrintWarning("(File/Pause Game) Unimplemented button");
		}
		if (ImGui::MenuItem("Stop Game"))
		{
			Debug::PrintWarning("(File/Stop Game) Unimplemented button");
		}
		if (ImGui::MenuItem("Hot Reload Game"))
		{
			Debug::PrintWarning("(Game/Hot Reload Game) Unimplemented button");
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Window")) //Draw Window menu
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
		ImGui::EndMenu();
	}

	ImGui::EndMainMenuBar();
}
