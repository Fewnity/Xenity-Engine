#if defined(_WIN32) || defined(_WIN64)

#include "editor_ui.h"
#include "../editor.h"
#include "../../xenity.h"
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_stdlib.h>
#include "../../engine/tools/shape_spawner.h"
#include <variant>
#include <imgui/imgui_internal.h>

int EditorUI::uiId = 0;
float EditorUI::nextFpsUpdate = 0;
float EditorUI::lastFps = 0;
bool EditorUI::showProfiler = true;
bool EditorUI::showEditor = true;

#pragma region Initialisation

void EditorUI::Init()
{
	//Set round corners
	ImGui::GetStyle().WindowRounding = 10;

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("Roboto Regular.ttf", 15);

	Debug::Print("---- Editor UI initiated ----");
}

#pragma endregion

#pragma region Update

/**
* Create a new frame for the editor's UI
*/
void EditorUI::NewFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
	uiId = 0;
}

/**
* Render the editor's UI
*/
void EditorUI::Render()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

#pragma endregion

#pragma region Menus

void EditorUI::DrawInspector()
{
	if (showEditor)
	{
		ImGuiIO& io = ImGui::GetIO();

		//Create Window
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(viewport->Size.x, 20), 0, ImVec2(1, 0));
		ImGui::SetNextWindowSizeConstraints(ImVec2(50, viewport->Size.y - 20), ImVec2(viewport->Size.x / 2.0f, viewport->Size.y - 20));
		ImGui::Begin("Inspector", 0, ImGuiWindowFlags_NoCollapse);
		auto selectedGameObject = Engine::selectedGameObject.lock();
		if (selectedGameObject)
		{
			char str0[128] = "";
			sprintf_s(str0, selectedGameObject->name.c_str());

			//Active checkbox
			bool active = selectedGameObject->GetActive();
			ImGui::Checkbox("##Active", &active);

			//Name input
			ImGui::SameLine();
			ImGui::InputText("##Name ", str0, IM_ARRAYSIZE(str0));

			//Apply new values if changed
			if (strcmp(str0, selectedGameObject->name.c_str()) != 0 && (InputSystem::GetKeyDown(RETURN) || InputSystem::GetKeyDown(MOUSE_LEFT)))
			{
				selectedGameObject->name = str0;
			}
			if (active != selectedGameObject->GetActive())
			{
				selectedGameObject->SetActive(active);
			}

			//Local position input
			ImGui::Spacing();
			ImGui::Spacing();
			Vector3 localPos = selectedGameObject->GetTransform()->GetLocalPosition();
			bool changed = DrawInput("Local Position", localPos);

			if (changed && (InputSystem::GetKeyDown(RETURN) || InputSystem::GetKeyDown(MOUSE_LEFT)))
			{
				selectedGameObject->GetTransform()->SetLocalPosition(localPos);
			}
			ImGui::Text("World Position: %f %f %f", selectedGameObject->GetTransform()->GetPosition().x, selectedGameObject->GetTransform()->GetPosition().y, selectedGameObject->GetTransform()->GetPosition().z);

			//Local rotation input
			ImGui::Spacing();
			ImGui::Spacing();
			Vector3 localRot = selectedGameObject->GetTransform()->GetLocalRotation();
			changed = DrawInput("Local Rotation", localRot);
			if (changed && (InputSystem::GetKeyDown(RETURN) || InputSystem::GetKeyDown(MOUSE_LEFT)))
			{
				selectedGameObject->GetTransform()->SetLocalRotation(localRot);
			}
			ImGui::Text("World Rotation: %f %f %f", selectedGameObject->GetTransform()->GetRotation().x, selectedGameObject->GetTransform()->GetRotation().y, selectedGameObject->GetTransform()->GetRotation().z);

			//Local scale input
			ImGui::Spacing();
			ImGui::Spacing();
			Vector3 localScale = selectedGameObject->GetTransform()->GetLocalScale();
			changed = DrawInput("Local Scale", localScale);
			if (changed && (InputSystem::GetKeyDown(RETURN) || InputSystem::GetKeyDown(MOUSE_LEFT)))
			{
				selectedGameObject->GetTransform()->SetLocalScale(localScale);
			}
			ImGui::Text("World Scale: %f %f %f", selectedGameObject->GetTransform()->GetScale().x, selectedGameObject->GetTransform()->GetScale().y, selectedGameObject->GetTransform()->GetScale().z);

			//Component list
			ImGui::Spacing();
			ImGui::Separator();

			int componentCount = selectedGameObject->GetComponentCount();
			for (int i = 0; i < componentCount; i++)
			{
				auto comp = selectedGameObject->components[i];
				//Draw component title
				std::string componentName = "- " + comp->GetComponentName();
				ImGui::Text("%s", componentName.c_str());
				ImGui::Separator();

				//Draw component variables
				DrawReflection(*comp);

				ImGui::Separator();
			}
		}
		ImGui::End();
	}
}

void EditorUI::DrawHierarchy()
{
	if (showEditor)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(0, 20));
		ImGui::SetNextWindowSizeConstraints(ImVec2(50, viewport->Size.y - 20), ImVec2(viewport->Size.x / 2.0f, viewport->Size.y - 20));

		ImGui::Begin("Hierarchy", 0, ImGuiWindowFlags_NoCollapse);
		//ImGui::SetWindowFontScale(2);
		if (!ImGui::IsWindowCollapsed())
		{
			ImGui::BeginChild("Hierarchy list", ImVec2(0, 0), true);

			//Add in the list only gameobject without parent
			for (int i = 0; i < Engine::gameObjectCount; i++)
			{
				if (Engine::gameObjects[i]->parent.lock() == nullptr)
				{
					DrawTreeItem(Engine::gameObjects[i]);
				}
			}
			ImGui::EndChild();
		}

		ImGui::End();
	}
}

void EditorUI::DrawProfiler()
{
	if (showProfiler)
	{
		ImGuiIO& io = ImGui::GetIO();

		//Update timer to slowly update framerate
		nextFpsUpdate += Time::GetUnscaledDeltaTime();
		if (nextFpsUpdate >= 0.06f)
		{
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
		else
		{
			ImGui::Text("Profiler disabled");
		}

		ImGui::End();
	}
}

void EditorUI::DrawMainMenuBar()
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
			Debug::PrintWarning("(File/Exit) Unimplemented button");
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
		ImGui::Checkbox(GenerateItemId().c_str(), &showProfiler);
		ImGui::SameLine();
		ImGui::Text("Show Profiler");
		ImGui::Checkbox(GenerateItemId().c_str(), &showEditor);
		ImGui::SameLine();
		ImGui::Text("Show Editor");

		ImGui::EndMenu();
	}

	ImGui::EndMainMenuBar();
}

#pragma endregion

#pragma region Low Level Draw Functions

std::string EditorUI::GenerateItemId()
{
	std::string itemId = "##" + std::to_string(uiId);
	uiId++;
	return itemId;
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
			Engine::SetSelectedGameObject(child);
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

void EditorUI::DrawReflection(Reflection& reflection)
{
	auto t = reflection.GetReflection();
	for (const auto& kv : t)
	{
		Variable& variableRef = t[kv.first];
		if (auto valuePtr = std::get_if< std::reference_wrapper<int>>(&variableRef))
			DrawInput(kv.first, valuePtr->get());
		else if (auto valuePtr = std::get_if<std::reference_wrapper<float>>(&variableRef))
			DrawInput(kv.first, valuePtr->get());
		else if (auto valuePtr = std::get_if< std::reference_wrapper<double>>(&variableRef))
			DrawInput(kv.first, valuePtr->get());
		else if (auto valuePtr = std::get_if< std::reference_wrapper<std::string>>(&variableRef))
			DrawInput(kv.first, valuePtr->get());
		else if (auto valuePtr = std::get_if< std::reference_wrapper<bool>>(&variableRef))
			DrawInput(kv.first, valuePtr->get());
		else if (auto valuePtr = std::get_if<std::reference_wrapper<Reflection>>(&variableRef))
		{
			if (auto val = dynamic_cast<Vector2*>(&valuePtr->get()))
				DrawInput(kv.first, *val);
			else if (auto val = dynamic_cast<Vector2Int*>(&valuePtr->get()))
				DrawInput(kv.first, *val);
			else if (auto val = dynamic_cast<Vector3*>(&valuePtr->get()))
				DrawInput(kv.first, *val);
			else if (auto val = dynamic_cast<Vector4*>(&valuePtr->get()))
				DrawInput(kv.first, *val);
			else if (auto val = dynamic_cast<Color*>(&valuePtr->get()))
				DrawInput(kv.first, *val);
			else
			{
				DrawReflection(valuePtr->get());
			}
		}
	}
}

void EditorUI::DrawTableInput(std::string inputName, std::string inputId, int columnIndex, float& value)
{
	ImGui::TableSetColumnIndex(columnIndex);
	ImGui::Text(inputName.c_str());
	ImGui::SameLine();
	ImGui::SetNextItemWidth(-1);
	ImGui::InputFloat(inputId.c_str(), &value, 0, 0, "%.4f");
}

void EditorUI::DrawTableInput(std::string inputName, std::string inputId, int columnIndex, int& value)
{
	ImGui::TableSetColumnIndex(columnIndex);
	ImGui::Text(inputName.c_str());
	ImGui::SameLine();
	ImGui::SetNextItemWidth(-1);
	ImGui::InputInt(inputId.c_str(), &value, 0, 0);
}

void EditorUI::DrawTextCentered(std::string text)
{
	float windowWidth = ImGui::GetWindowSize().x;
	float textWidth = ImGui::CalcTextSize(text.c_str()).x;
	ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
	ImGui::Text(text.c_str());
}

void EditorUI::DrawInputTitle(std::string title)
{
	ImGui::Text(title.c_str());
	ImGui::SameLine();
	ImGui::SetCursorPosX(150);
	ImGui::SetNextItemWidth(-1);
}

bool EditorUI::DrawInput(std::string inputName, float& value)
{
	DrawInputTitle(inputName);
	float oldValue = float(value);
	ImGui::InputFloat(GenerateItemId().c_str(), &value, 0, 0, "%.4f");
	return value != oldValue;
}

bool EditorUI::DrawInput(std::string inputName, double& value)
{
	DrawInputTitle(inputName);
	double oldValue = double(value);
	ImGui::InputDouble(GenerateItemId().c_str(), &value, 0, 0, "%0.8f");
	return value != oldValue;
}

bool EditorUI::DrawInput(std::string inputName, std::string& value)
{
	DrawInputTitle(inputName);
	std::string oldValue = std::string(value);
	ImGui::InputText(GenerateItemId().c_str(), &value);
	return value != oldValue;
}

bool EditorUI::DrawInput(std::string inputName, int& value)
{
	DrawInputTitle(inputName);
	int oldValue = int(value);
	ImGui::InputInt(GenerateItemId().c_str(), &value);
	return value != oldValue;
}

bool EditorUI::DrawInput(std::string inputName, bool& value)
{
	DrawInputTitle(inputName);
	bool oldValue = bool(value);
	ImGui::Checkbox(GenerateItemId().c_str(), &value);
	return value != oldValue;
}

bool EditorUI::DrawInput(std::string inputName, Color& value)
{
	ImGui::Text(inputName.c_str());
	ImGui::SameLine();
	ImGui::SetCursorPosX(150);
	Vector4 vec4 = value.GetRGBA().ToVector4();
	ImVec4 color = ImVec4(vec4.x, vec4.y, vec4.z, vec4.w);
	float startAvailSize = ImGui::GetContentRegionAvail().x;
	ImGui::SetNextItemWidth(startAvailSize);

	ImGui::ColorEdit4(GenerateItemId().c_str(), (float*)&color, ImGuiColorEditFlags_NoInputs);
	value.SetFromRGBAfloat(color.x, color.y, color.z, color.w);

	bool valueChanged = false;
	if (vec4.x != color.x || vec4.y != color.y || vec4.z != color.z || vec4.w != color.w)
		valueChanged = true;

	return valueChanged;
}

bool EditorUI::DrawInput(std::string inputName, Vector2& value)
{
	Vector2 oldValue = Vector2(value);

	ImGui::Text(inputName.c_str());
	ImGui::SameLine();
	ImGui::SetCursorPosX(150);

	if (ImGui::BeginTable("table", 2, 0))
	{
		ImGui::TableNextRow();
		DrawTableInput("X", GenerateItemId(), 0, value.x);
		DrawTableInput("Y", GenerateItemId(), 1, value.y);
		ImGui::EndTable();
	}

	return value != oldValue;
}

bool EditorUI::DrawInput(std::string inputName, Vector2Int& value)
{
	Vector2Int oldValue = Vector2Int(value);

	ImGui::Text(inputName.c_str());
	ImGui::SameLine();
	ImGui::SetCursorPosX(150);

	if (ImGui::BeginTable("table", 2, 0))
	{
		ImGui::TableNextRow();
		DrawTableInput("X", GenerateItemId(), 0, value.x);
		DrawTableInput("Y", GenerateItemId(), 1, value.y);
		ImGui::EndTable();
	}

	return value != oldValue;
}

bool EditorUI::DrawInput(std::string inputName, Vector3& value)
{
	Vector3 oldValue = Vector3(value);

	ImGui::Text(inputName.c_str());
	ImGui::SameLine();
	ImGui::SetCursorPosX(150);

	if (ImGui::BeginTable("table", 3, 0))
	{
		ImGui::TableNextRow();
		DrawTableInput("X", GenerateItemId(), 0, value.x);
		DrawTableInput("Y", GenerateItemId(), 1, value.y);
		DrawTableInput("Z", GenerateItemId(), 2, value.z);
		ImGui::EndTable();
	}

	return value != oldValue;
}

bool EditorUI::DrawInput(std::string inputName, Vector4& value)
{
	Vector4 oldValue = Vector4(value);

	ImGui::Text(inputName.c_str());
	ImGui::SameLine();
	ImGui::SetCursorPosX(150);

	if (ImGui::BeginTable("table", 4, 0))
	{
		ImGui::TableNextRow();
		DrawTableInput("X", GenerateItemId(), 0, value.x);
		DrawTableInput("Y", GenerateItemId(), 1, value.y);
		DrawTableInput("Z", GenerateItemId(), 2, value.z);
		DrawTableInput("W", GenerateItemId(), 3, value.w);
		ImGui::EndTable();
	}

	return value != oldValue;
}

#pragma endregion

#endif