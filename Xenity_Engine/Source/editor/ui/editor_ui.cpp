#if defined(_WIN32) || defined(_WIN64)
#include "editor_ui.h"
#include "../../xenity.h"
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>
#include "../../engine/tools/shape_spawner.h"

Engine* EditorUI::engine = nullptr;
int EditorUI::uiId = 0;
float EditorUI::nextFpsUpdate = 0;
float EditorUI::lastFps = 0;

void EditorUI::Init()
{
	ImGui::GetStyle().WindowRounding = 10;

	Debug::Print("---- Editor UI initiated ----");
}

void EditorUI::NewFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
	EditorUI::uiId = 0;
}

void EditorUI::DrawInspector()
{
	ImGuiIO& io = ImGui::GetIO();

	ImGui::Begin("Inspector");
	auto selectedGameObject = engine->selectedGameObject.lock();
	if (selectedGameObject)
	{
		char str0[128] = "";
		sprintf_s(str0, selectedGameObject->name.c_str());


		bool active = selectedGameObject->GetActive();
		ImGui::Checkbox("##Active", &active);
		ImGui::SameLine();
		ImGui::InputText("##Name ", str0, IM_ARRAYSIZE(str0));
		if (strcmp(str0, selectedGameObject->name.c_str()) != 0 && (InputSystem::GetKeyDown(RETURN) || InputSystem::GetKeyDown(MOUSE_LEFT)))
		{
			selectedGameObject->name = str0;
		}

		if (active != selectedGameObject->GetActive())
		{
			selectedGameObject->SetActive(active);
		}

		ImGui::Spacing();
		ImGui::Spacing();
		Vector3 localPos = selectedGameObject->GetTransform()->GetLocalPosition();
		bool changed = DrawVector3Input("Local Position", "X", "Y", "Z", localPos);

		if (changed && (InputSystem::GetKeyDown(RETURN) || InputSystem::GetKeyDown(MOUSE_LEFT)))
		{
			selectedGameObject->GetTransform()->SetLocalPosition(localPos);
		}
		ImGui::Text("World Position: %f %f %f", selectedGameObject->GetTransform()->GetPosition().x, selectedGameObject->GetTransform()->GetPosition().y, selectedGameObject->GetTransform()->GetPosition().z);
		ImGui::Text("World Matrix: \n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f", selectedGameObject->GetTransform()->transformationMatrix[0].x, selectedGameObject->GetTransform()->transformationMatrix[0].y, selectedGameObject->GetTransform()->transformationMatrix[0].z, selectedGameObject->GetTransform()->transformationMatrix[0].w,
			selectedGameObject->GetTransform()->transformationMatrix[1].x, selectedGameObject->GetTransform()->transformationMatrix[1].y, selectedGameObject->GetTransform()->transformationMatrix[1].z, selectedGameObject->GetTransform()->transformationMatrix[1].w,
			selectedGameObject->GetTransform()->transformationMatrix[2].x, selectedGameObject->GetTransform()->transformationMatrix[2].y, selectedGameObject->GetTransform()->transformationMatrix[2].z, selectedGameObject->GetTransform()->transformationMatrix[2].w,
			selectedGameObject->GetTransform()->transformationMatrix[3].x, selectedGameObject->GetTransform()->transformationMatrix[3].y, selectedGameObject->GetTransform()->transformationMatrix[3].z, selectedGameObject->GetTransform()->transformationMatrix[3].w);

		ImGui::Spacing();
		ImGui::Spacing();
		Vector3 localRot = selectedGameObject->GetTransform()->GetLocalRotation();
		changed = DrawVector3Input("Local Rotation", "X", "Y", "Z", localRot);
		if (changed && (InputSystem::GetKeyDown(RETURN) || InputSystem::GetKeyDown(MOUSE_LEFT))) {
			selectedGameObject->GetTransform()->SetLocalRotation(localRot);
		}
		ImGui::Text("World Rotation: %f %f %f", selectedGameObject->GetTransform()->GetRotation().x, selectedGameObject->GetTransform()->GetRotation().y, selectedGameObject->GetTransform()->GetRotation().z);
		ImGui::Spacing();
		ImGui::Spacing();

		Vector3 localScale = selectedGameObject->GetTransform()->GetLocalScale();
		changed = DrawVector3Input("Local Scale", "X", "Y", "Z", localScale);
		if (changed && (InputSystem::GetKeyDown(RETURN) || InputSystem::GetKeyDown(MOUSE_LEFT))) {
			selectedGameObject->GetTransform()->SetLocalScale(localScale);
		}
		ImGui::Text("World Scale: %f %f %f", selectedGameObject->GetTransform()->GetScale().x, selectedGameObject->GetTransform()->GetScale().y, selectedGameObject->GetTransform()->GetScale().z);

		ImGui::Spacing();
		ImGui::Spacing();
		int componentCount = selectedGameObject->GetComponentCount();

		for (int i = 0; i < componentCount; i++)
		{
			ImGui::Spacing();
			ImGui::Spacing();
			auto comp = selectedGameObject->components[i];
			std::string componentName = "- " + comp->componentName;
			//std::string componentName = typeid(comp).name();
			/*int nameLenght = componentName.size();
			bool firstDelete = true;
			for (int strI = 0; strI < nameLenght; strI++)
			{
				if (componentName[strI] == ' ')
				{
					if (firstDelete)
					{
						componentName.erase(0, strI + 1);
						strI = 0;
						firstDelete = false;
					}
					else {
						componentName.erase(strI);
						break;
					}
				}
			}*/
			ImGui::Text("%s", componentName.c_str());
			//int intVariableCount = comp->reflectedInts.size();
			//int floatVariableCount = comp->reflectedFloats.size();
			for (const auto& kv : comp->reflectedFloats)
			{
				DrawFloatInput(kv.first, *kv.second);
			}
			for (const auto& kv : comp->reflectedInts)
			{
				DrawIntInput(kv.first, *kv.second);
			}
		}
	}
	ImGui::End();
}

void EditorUI::DrawTreeItem(std::weak_ptr<GameObject> child)
{
	auto childLock = child.lock();
	if (childLock)
	{
		int childCount = childLock->children.size();
		int flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;
		if (Engine::selectedGameObject.lock() == childLock)
			flags |= ImGuiTreeNodeFlags_Selected;

		if (childCount == 0)
			flags |= ImGuiTreeNodeFlags_Leaf;

		if (childLock->GetLocalActive())
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1.0f));
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5, 0.5f, 0.5f, 1.0f));
		}

		bool opened = ImGui::TreeNodeEx(childLock->name.c_str(), flags);
		ImGui::PopStyleColor();
		if (ImGui::IsItemActivated() && ImGui::IsItemClicked())
		{
			engine->SetSelectedGameObject(child);
		}
		if (opened)
		{
			for (int i = 0; i < childCount; i++)
			{
				DrawTreeItem(childLock->children[i]);
			}
			ImGui::TreePop();
		}
	}
}

void EditorUI::Render()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void EditorUI::DrawHierarchy()
{
	ImGui::Begin("Hierarchy");
	//ImGui::SetWindowFontScale(2);
	if (!ImGui::IsWindowCollapsed())
	{
		ImGui::BeginChild("Hierarchy list", ImVec2(0, 0), true);

		for (int i = 0; i < engine->gameObjectCount; i++)
		{
			if (engine->gameObjects[i]->parent.lock() == nullptr)
			{
				DrawTreeItem(engine->gameObjects[i]);
			}
		}
		ImGui::EndChild();
	}


	ImGui::End();
}

void EditorUI::DrawMainMenuBar()
{
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("File"))
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
			Debug::PrintWarning("(File/Save Scene) Unimplemented button");
		}
		if (ImGui::MenuItem("Build Game"))
		{
			Debug::PrintWarning("(File/Build Game) Unimplemented button");
		}
		if (ImGui::MenuItem("Exit"))
		{
			Debug::PrintWarning("(File/Exit) Unimplemented button");
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("GameObjects"))
	{
		if (ImGui::MenuItem("Create Empty Parent"))
		{
			Debug::PrintWarning("(GameObjects/Create Empty Parent) Unimplemented button");
		}
		if (ImGui::MenuItem("Create Empty Child"))
		{
			Debug::PrintWarning("(GameObjects/Create Empty Child) Unimplemented button");
		}
		if (ImGui::MenuItem("Create Empty"))
		{
			Debug::PrintWarning("(GameObjects/Create Empty) Unimplemented button");
		}
		if (ImGui::MenuItem("Cube"))
		{
			ShapeSpawner::SpawnCube();
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
	if (ImGui::BeginMenu("Component"))
	{
		if (ImGui::BeginMenu("Mesh"))
		{
			if (ImGui::MenuItem("Mesh Renderer"))
			{
				Debug::PrintWarning("(Component/Mesh/Mesh Renderer) Unimplemented button");
			}
			if (ImGui::MenuItem("Text Mesh"))
			{
				Debug::PrintWarning("(Component/Mesh/Text Mesh) Unimplemented button");
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Audio"))
		{
			if (ImGui::MenuItem("Audio Source"))
			{
				Debug::PrintWarning("(Component/Audio/Audio Source) Unimplemented button");
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Rendering"))
		{
			if (ImGui::MenuItem("Camera"))
			{
				Debug::PrintWarning("(Component/Rendering/Camera) Unimplemented button");
			}
			if (ImGui::MenuItem("Light"))
			{
				Debug::PrintWarning("(Component/Rendering/Light) Unimplemented button");
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Tilemap"))
		{
			if (ImGui::MenuItem("Tilemap"))
			{
				Debug::PrintWarning("(Component/Tilemap/Tilemap) Unimplemented button");
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("UI"))
		{
			if (ImGui::MenuItem("Text Renderer"))
			{
				Debug::PrintWarning("(Component/UI/Text Renderer) Unimplemented button");
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Game"))
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

	ImGui::EndMainMenuBar();
}

void EditorUI::DrawTextCentered(std::string text)
{
	float windowWidth = ImGui::GetWindowSize().x;
	float textWidth = ImGui::CalcTextSize(text.c_str()).x;

	ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
	ImGui::Text(text.c_str());
}

bool EditorUI::DrawFloatInput(std::string inputName, float& value)
{
	std::string inputName1 = "##" + std::to_string(uiId);
	uiId += 1;

	ImGui::Text(inputName.c_str());
	float titleWidth = ImGui::GetItemRectSize().x;
	ImGui::SameLine();
	float oldValue = float(value);
	float startAvailSize = ImGui::GetContentRegionAvail().x;
	ImGui::SameLine();
	ImGui::SetNextItemWidth(startAvailSize);
	ImGui::InputFloat(inputName1.c_str(), &value, 0, 0, "%f");

	return value != oldValue;
}

bool EditorUI::DrawIntInput(std::string inputName, int& value)
{
	std::string inputName1 = "##" + std::to_string(uiId);
	uiId += 1;

	ImGui::Text(inputName.c_str());
	float titleWidth = ImGui::GetItemRectSize().x;
	ImGui::SameLine();
	int oldValue = int(value);
	float startAvailSize = ImGui::GetContentRegionAvail().x;
	ImGui::SameLine();
	ImGui::SetNextItemWidth(startAvailSize);
	ImGui::InputInt(inputName1.c_str(), &value);

	return value != oldValue;
}

bool EditorUI::DrawVector3Input(std::string inputName, std::string name1, std::string name2, std::string name3, Vector3& value)
{
	std::string inputName1 = "##" + std::to_string(uiId);
	std::string inputName2 = "##" + std::to_string(uiId + 1);
	std::string inputName3 = "##" + std::to_string(uiId + 2);
	uiId += 3;

	ImGui::Text(inputName.c_str());
	//float titleWidth = ImGui::GetItemRectSize().x;
	ImGui::SameLine();
	ImGui::SetCursorPosX(150);
	Vector3 oldValue = Vector3(value);
	ImGui::Text(name1.c_str());
	float textWidth = ImGui::GetItemRectSize().x * 3;
	float startAvailSize = ImGui::GetContentRegionAvail().x - 150;
	ImGui::SameLine();
	ImGui::SetNextItemWidth(startAvailSize / 3.0f - textWidth);
	ImGui::InputFloat(inputName1.c_str(), &value.x, 0, 0, "%f");
	ImGui::SameLine();
	ImGui::Text(name2.c_str());
	ImGui::SameLine();
	ImGui::SetNextItemWidth(startAvailSize / 3.0f - textWidth);
	ImGui::InputFloat(inputName2.c_str(), &value.y, 0, 0, "%f");
	ImGui::SameLine();
	ImGui::Text(name3.c_str());
	ImGui::SameLine();
	ImGui::SetNextItemWidth(startAvailSize / 3.0f - textWidth);
	ImGui::InputFloat(inputName3.c_str(), &value.z, 0, 0, "%f");

	return value != oldValue;
}

void EditorUI::DrawProfiler()
{
	ImGuiIO& io = ImGui::GetIO();

	nextFpsUpdate += Time::GetUnscaledDeltaTime();

	if (nextFpsUpdate >= 0.06f) {
		nextFpsUpdate = 0;
		lastFps = io.Framerate;
	}

	ImGui::Begin("Debug");
	ImGui::Text("FPS: %.1f", lastFps);
	ImGui::Text("DrawCall Count: %d", Performance::GetDrawCallCount());
	ImGui::Text("Updated Materials: %d", Performance::GetUpdatedMaterialCount());

	if (EngineSettings::useProfiler)
	{
		//Add profiler texts
		for (const auto& kv : Performance::profilerList)
		{
			ImGui::Text("%s: %ld, avg %ld", kv.first.c_str(), kv.second->GetValue(), kv.second->average);
		}
	}

	//ImGui::SliderFloat("float", &cameraZoom, 1.0f, 2.8f);

	ImGui::End();
}
#endif