#if defined(_WIN32) || defined(_WIN64)

#include "editor_ui.h"
#include "../editor.h"
#include "../../xenity.h"
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_stdlib.h>
#include "../../engine/tools/shape_spawner.h"
#include <variant>

Engine* EditorUI::engine = nullptr;
int EditorUI::uiId = 0;
float EditorUI::nextFpsUpdate = 0;
float EditorUI::lastFps = 0;

void EditorUI::Init()
{
	ImGui::GetStyle().WindowRounding = 10;

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("Roboto Regular.ttf", 15);

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
		bool changed = DrawInput("Local Position", localPos);

		if (changed && (InputSystem::GetKeyDown(RETURN) || InputSystem::GetKeyDown(MOUSE_LEFT)))
		{
			selectedGameObject->GetTransform()->SetLocalPosition(localPos);
		}
		ImGui::Text("World Position: %f %f %f", selectedGameObject->GetTransform()->GetPosition().x, selectedGameObject->GetTransform()->GetPosition().y, selectedGameObject->GetTransform()->GetPosition().z);
		/*ImGui::Text("World Matrix: \n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f", selectedGameObject->GetTransform()->transformationMatrix[0].x, selectedGameObject->GetTransform()->transformationMatrix[0].y, selectedGameObject->GetTransform()->transformationMatrix[0].z, selectedGameObject->GetTransform()->transformationMatrix[0].w,
			selectedGameObject->GetTransform()->transformationMatrix[1].x, selectedGameObject->GetTransform()->transformationMatrix[1].y, selectedGameObject->GetTransform()->transformationMatrix[1].z, selectedGameObject->GetTransform()->transformationMatrix[1].w,
			selectedGameObject->GetTransform()->transformationMatrix[2].x, selectedGameObject->GetTransform()->transformationMatrix[2].y, selectedGameObject->GetTransform()->transformationMatrix[2].z, selectedGameObject->GetTransform()->transformationMatrix[2].w,
			selectedGameObject->GetTransform()->transformationMatrix[3].x, selectedGameObject->GetTransform()->transformationMatrix[3].y, selectedGameObject->GetTransform()->transformationMatrix[3].z, selectedGameObject->GetTransform()->transformationMatrix[3].w);
			*/
		ImGui::Spacing();
		ImGui::Spacing();
		Vector3 localRot = selectedGameObject->GetTransform()->GetLocalRotation();
		changed = DrawInput("Local Rotation", localRot);
		if (changed && (InputSystem::GetKeyDown(RETURN) || InputSystem::GetKeyDown(MOUSE_LEFT))) {
			selectedGameObject->GetTransform()->SetLocalRotation(localRot);
		}
		ImGui::Text("World Rotation: %f %f %f", selectedGameObject->GetTransform()->GetRotation().x, selectedGameObject->GetTransform()->GetRotation().y, selectedGameObject->GetTransform()->GetRotation().z);
		ImGui::Spacing();
		ImGui::Spacing();

		Vector3 localScale = selectedGameObject->GetTransform()->GetLocalScale();
		changed = DrawInput("Local Scale", localScale);
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
			std::string componentName = "- " + comp->GetComponentName();

			ImGui::Text("%s", componentName.c_str());

			auto t = comp->GetReflection();
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
					//auto t2 = valuePtr->get().GetReflection();
					//for (const auto& kv2 : t2)
					//{
					//	Variable& variableRef2 = t2[kv2.first];
					//	if (auto val = std::get_if<std::reference_wrapper<float>>(&variableRef2))
					//	{
					//		DrawInput(kv.first, *val);
					//	}
					//}
					if (auto val = dynamic_cast<Vector2*>(&valuePtr->get()))
					{
						DrawInput(kv.first, *val);
					}else if (auto val = dynamic_cast<Vector2Int*>(&valuePtr->get()))
					{
						DrawInput(kv.first, *val);
					}
					else if (auto val = dynamic_cast<Vector3*>(&valuePtr->get()))
					{
						DrawInput(kv.first, *val);
					}
					else if (auto val = dynamic_cast<Vector4*>(&valuePtr->get()))
					{
						DrawInput(kv.first, *val);
					}
					else if (auto val = dynamic_cast<Color*>(&valuePtr->get()))
					{
						DrawInput(kv.first, *val);
					}
				}
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
	bool hasSelectedGameObject = !Engine::selectedGameObject.expired();

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
	if (ImGui::BeginMenu("GameObject"))
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
	if (ImGui::BeginMenu("Component"))
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

bool EditorUI::DrawInput(std::string inputName, Color& value)
{
	std::string inputName1 = "##" + std::to_string(uiId);
	uiId += 1;

	ImGui::Text(inputName.c_str());
	float titleWidth = ImGui::GetItemRectSize().x;
	ImGui::SameLine();
	ImGui::SetCursorPosX(150);
	Vector4 vec4 = value.GetRGBA().ToVector4();
	ImVec4 color = ImVec4(vec4.x, vec4.y, vec4.z, vec4.w);
	float startAvailSize = ImGui::GetContentRegionAvail().x;
	ImGui::SetNextItemWidth(startAvailSize);

	ImGui::ColorEdit4(inputName1.c_str(), (float*)&color, ImGuiColorEditFlags_NoInputs);
	value.SetFromRGBAfloat(color.x, color.y, color.z, color.w);
	bool valueChanged = false;
	if (vec4.x != color.x || vec4.y != color.y || vec4.z != color.z || vec4.w != color.w)
		valueChanged = true;

	return valueChanged;
}

bool EditorUI::DrawInput(std::string inputName, Vector2& value)
{
	std::string name1 = "X";
	std::string name2 = "Y";
	std::string inputName1 = "##" + std::to_string(uiId);
	std::string inputName2 = "##" + std::to_string(uiId + 1);
	uiId += 2;

	ImGui::Text(inputName.c_str());
	//float titleWidth = ImGui::GetItemRectSize().x;
	ImGui::SameLine();
	ImGui::SetCursorPosX(150);
	Vector2 oldValue = Vector2(value);
	ImGui::Text(name1.c_str());
	float textWidth = ImGui::GetItemRectSize().x * 2;
	float startAvailSize = ImGui::GetContentRegionAvail().x - 150;
	ImGui::SameLine();
	ImGui::SetNextItemWidth(startAvailSize / 2.0f - textWidth);
	ImGui::InputFloat(inputName1.c_str(), &value.x, 0, 0, "%.4f");

	ImGui::SameLine();
	ImGui::Text(name2.c_str());
	ImGui::SameLine();
	ImGui::SetNextItemWidth(startAvailSize / 2.0f - textWidth);
	ImGui::InputFloat(inputName2.c_str(), &value.y, 0, 0, "%.4f");

	return value != oldValue;
}

bool EditorUI::DrawInput(std::string inputName, Vector2Int& value)
{
	std::string name1 = "X";
	std::string name2 = "Y";
	std::string inputName1 = "##" + std::to_string(uiId);
	std::string inputName2 = "##" + std::to_string(uiId + 1);
	uiId += 2;

	ImGui::Text(inputName.c_str());
	//float titleWidth = ImGui::GetItemRectSize().x;
	ImGui::SameLine();
	ImGui::SetCursorPosX(150);
	Vector2Int oldValue = Vector2Int(value);
	ImGui::Text(name1.c_str());
	float textWidth = ImGui::GetItemRectSize().x * 2;
	float startAvailSize = ImGui::GetContentRegionAvail().x - 150;
	ImGui::SameLine();
	ImGui::SetNextItemWidth(startAvailSize / 2.0f - textWidth);
	ImGui::InputInt(inputName1.c_str(), &value.x, 0, 0);

	ImGui::SameLine();
	ImGui::Text(name2.c_str());
	ImGui::SameLine();
	ImGui::SetNextItemWidth(startAvailSize / 2.0f - textWidth);
	ImGui::InputInt(inputName2.c_str(), &value.y, 0, 0);

	return value != oldValue;
}

bool EditorUI::DrawInput(std::string inputName, Vector3& value)
{
	std::string name1 = "X";
	std::string name2 = "Y";
	std::string name3 = "Z";
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
	ImGui::InputFloat(inputName1.c_str(), &value.x, 0, 0, "%.4f");

	ImGui::SameLine();
	ImGui::Text(name2.c_str());
	ImGui::SameLine();
	ImGui::SetNextItemWidth(startAvailSize / 3.0f - textWidth);
	ImGui::InputFloat(inputName2.c_str(), &value.y, 0, 0, "%.4f");

	ImGui::SameLine();
	ImGui::Text(name3.c_str());
	ImGui::SameLine();
	ImGui::SetNextItemWidth(startAvailSize / 3.0f - textWidth);
	ImGui::InputFloat(inputName3.c_str(), &value.z, 0, 0, "%.4f");

	return value != oldValue;
}

bool EditorUI::DrawInput(std::string inputName, Vector4& value)
{
	std::string name1 = "X";
	std::string name2 = "Y";
	std::string name3 = "Z";
	std::string name4 = "W";
	std::string inputName1 = "##" + std::to_string(uiId);
	std::string inputName2 = "##" + std::to_string(uiId + 1);
	std::string inputName3 = "##" + std::to_string(uiId + 2);
	std::string inputName4 = "##" + std::to_string(uiId + 3);
	uiId += 4;

	ImGui::Text(inputName.c_str());
	//float titleWidth = ImGui::GetItemRectSize().x;
	ImGui::SameLine();
	ImGui::SetCursorPosX(150);
	Vector4 oldValue = Vector4(value);
	ImGui::Text(name1.c_str());
	float textWidth = ImGui::GetItemRectSize().x * 4;
	float startAvailSize = ImGui::GetContentRegionAvail().x - 150;
	ImGui::SameLine();
	ImGui::SetNextItemWidth(startAvailSize / 4.0f - textWidth);
	ImGui::InputFloat(inputName1.c_str(), &value.x, 0, 0, "%.4f");

	ImGui::SameLine();
	ImGui::Text(name2.c_str());
	ImGui::SameLine();
	ImGui::SetNextItemWidth(startAvailSize / 4.0f - textWidth);
	ImGui::InputFloat(inputName2.c_str(), &value.y, 0, 0, "%.4f");

	ImGui::SameLine();
	ImGui::Text(name3.c_str());
	ImGui::SameLine();
	ImGui::SetNextItemWidth(startAvailSize / 4.0f - textWidth);
	ImGui::InputFloat(inputName3.c_str(), &value.z, 0, 0, "%.4f");

	ImGui::SameLine();
	ImGui::Text(name4.c_str());
	ImGui::SameLine();
	ImGui::SetNextItemWidth(startAvailSize / 4.0f - textWidth);
	ImGui::InputFloat(inputName4.c_str(), &value.w, 0, 0, "%.4f");

	return value != oldValue;
}

void EditorUI::DrawTextCentered(std::string text)
{
	float windowWidth = ImGui::GetWindowSize().x;
	float textWidth = ImGui::CalcTextSize(text.c_str()).x;

	ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
	ImGui::Text(text.c_str());
}

bool EditorUI::DrawInput(std::string inputName, float& value)
{
	std::string inputName1 = "##" + std::to_string(uiId);
	uiId += 1;

	ImGui::Text(inputName.c_str());
	float titleWidth = ImGui::GetItemRectSize().x;
	ImGui::SameLine();
	ImGui::SetCursorPosX(150);
	float oldValue = float(value);
	float startAvailSize = ImGui::GetContentRegionAvail().x;

	ImGui::SetNextItemWidth(startAvailSize);
	ImGui::InputFloat(inputName1.c_str(), &value, 0, 0, "%.4f");

	return value != oldValue;
}

bool EditorUI::DrawInput(std::string inputName, double& value)
{
	std::string inputName1 = "##" + std::to_string(uiId);
	uiId += 1;

	ImGui::Text(inputName.c_str());
	float titleWidth = ImGui::GetItemRectSize().x;
	ImGui::SameLine();
	ImGui::SetCursorPosX(150);
	double oldValue = double(value);
	float startAvailSize = ImGui::GetContentRegionAvail().x;

	ImGui::SetNextItemWidth(startAvailSize);
	ImGui::InputDouble(inputName1.c_str(), &value, 0, 0, "%0.8f");

	return value != oldValue;
}

bool EditorUI::DrawInput(std::string inputName, std::string& value)
{
	std::string inputName1 = "##" + std::to_string(uiId);
	uiId += 1;

	ImGui::Text(inputName.c_str());
	float titleWidth = ImGui::GetItemRectSize().x;
	ImGui::SameLine();
	ImGui::SetCursorPosX(150);
	std::string oldValue = std::string(value);
	float startAvailSize = ImGui::GetContentRegionAvail().x;

	ImGui::SetNextItemWidth(startAvailSize);
	ImGui::InputText(inputName1.c_str(), &value);

	return value != oldValue;
}

bool EditorUI::DrawInput(std::string inputName, int& value)
{
	std::string inputName1 = "##" + std::to_string(uiId);
	uiId += 1;

	ImGui::Text(inputName.c_str());
	float titleWidth = ImGui::GetItemRectSize().x;
	ImGui::SameLine();
	ImGui::SetCursorPosX(150);
	int oldValue = int(value);
	float startAvailSize = ImGui::GetContentRegionAvail().x;

	ImGui::SetNextItemWidth(startAvailSize);
	ImGui::InputInt(inputName1.c_str(), &value);

	return value != oldValue;
}

bool EditorUI::DrawInput(std::string inputName, bool& value)
{
	std::string inputName1 = "##" + std::to_string(uiId);
	uiId += 1;

	ImGui::Text(inputName.c_str());
	float titleWidth = ImGui::GetItemRectSize().x;
	ImGui::SameLine();
	ImGui::SetCursorPosX(150);
	bool oldValue = bool(value);
	float startAvailSize = ImGui::GetContentRegionAvail().x;
	ImGui::SameLine();
	ImGui::SetNextItemWidth(startAvailSize);
	ImGui::Checkbox(inputName1.c_str(), &value);

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