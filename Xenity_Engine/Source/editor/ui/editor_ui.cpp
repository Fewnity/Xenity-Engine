#include "editor_ui.h"
#include "../../xenity.h"
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>

Engine* EditorUI::engine = nullptr;
int EditorUI::uiId = 0;
float EditorUI::nextFpsUpdate = 0;
float EditorUI::lastFps = 0;

void EditorUI::Init()
{
	ImGui::GetStyle().WindowRounding = 10;
}

void EditorUI::DrawInspector()
{
	ImGuiIO& io = ImGui::GetIO();

	ImGui::Begin("Inspector");
	GameObject* selectedGameObject = engine->selectedGameObject;
	if (selectedGameObject != nullptr)
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
		Vector3 localPos = selectedGameObject->transform.GetLocalPosition();
		bool changed = DrawVector3Input("Local Position", "X", "Y", "Z", localPos);

		if (changed && (InputSystem::GetKeyDown(RETURN) || InputSystem::GetKeyDown(MOUSE_LEFT))) {
			selectedGameObject->transform.SetLocalPosition(localPos);
		}
		//ImGui::Text("World Position: %f %f %f", selectedGameObject->transform.GetPosition().x, selectedGameObject->transform.GetPosition().y, selectedGameObject->transform.GetPosition().z);

		ImGui::Spacing();
		ImGui::Spacing();
		Vector3 localRot = selectedGameObject->transform.GetLocalRotation();
		changed = DrawVector3Input("Local Rotation", "X", "Y", "Z", localRot);
		if (changed && (InputSystem::GetKeyDown(RETURN) || InputSystem::GetKeyDown(MOUSE_LEFT))) {
			selectedGameObject->transform.SetLocalRotation(localRot);
		}
		//ImGui::Text("World Rotation: %f %f %f", selectedGameObject->transform.GetRotation().x, selectedGameObject->transform.GetRotation().y, selectedGameObject->transform.GetRotation().z);
		ImGui::Spacing();
		ImGui::Spacing();

		//ImGui::Text("Local Scale:");
		Vector3 localScale = selectedGameObject->transform.GetLocalScale();
		changed = DrawVector3Input("Local Scale", "X", "Y", "Z", localScale);
		if (changed && (InputSystem::GetKeyDown(RETURN) || InputSystem::GetKeyDown(MOUSE_LEFT))) {
			selectedGameObject->transform.SetLocalScale(localScale);
		}
		//ImGui::Text("World Scale: %f %f %f", selectedGameObject->transform.GetScale().x, selectedGameObject->transform.GetScale().y, selectedGameObject->transform.GetScale().z);

		ImGui::Spacing();
		ImGui::Spacing();
		int componentCount = selectedGameObject->GetComponentCount();
		ImGui::Text("Component count: %d", componentCount);
		for (int i = 0; i < componentCount; i++)
		{
			std::string componentName = typeid(selectedGameObject->components[i]).name();
			int nameLenght = componentName.size();
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
			}
			ImGui::Text("Component: %s", componentName.c_str());
		}
	}
	ImGui::End();
}

void EditorUI::DrawTreeItem(GameObject* child)
{
	int childCount = child->children.size();
	int flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;
	if (Engine::selectedGameObject == child)
		flags |= ImGuiTreeNodeFlags_Selected;

	if (childCount == 0)
		flags |= ImGuiTreeNodeFlags_Leaf;

	bool opened = ImGui::TreeNodeEx(child->name.c_str(), flags);
	if (ImGui::IsItemActivated() && ImGui::IsItemClicked())
	{
		engine->SetSelectedGameObject(child);
	}
	if (opened)
	{
		for (int i = 0; i < childCount; i++)
		{
			DrawTreeItem(child->children[i]);
		}
		ImGui::TreePop();
	}
}

void EditorUI::DrawHierarchy()
{
	ImGuiIO& io = ImGui::GetIO();

	ImGui::Begin("Hierarchy");
	//ImGui::SetWindowFontScale(2);

	ImGui::BeginChild("Hierarchy list", ImVec2(0, 0), true);

	for (int i = 0; i < engine->gameObjectCount; i++)
	{
		if (engine->GetGameObjects()[i]->parent == nullptr)
		{
			DrawTreeItem(engine->GetGameObjects()[i]);
		}
	}
	ImGui::EndChild();


	ImGui::End();
}

void EditorUI::DrawTextCentered(std::string text)
{
	float windowWidth = ImGui::GetWindowSize().x;
	float textWidth = ImGui::CalcTextSize(text.c_str()).x;

	ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
	ImGui::Text(text.c_str());
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
	ImGui::InputFloat(inputName1.c_str(), &value.x);
	ImGui::SameLine();
	ImGui::Text(name2.c_str());
	ImGui::SameLine();
	ImGui::SetNextItemWidth(startAvailSize / 3.0f - textWidth);
	ImGui::InputFloat(inputName2.c_str(), &value.y);
	ImGui::SameLine();
	ImGui::Text(name3.c_str());
	ImGui::SameLine();
	ImGui::SetNextItemWidth(startAvailSize / 3.0f - textWidth);
	ImGui::InputFloat(inputName3.c_str(), &value.z);

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

	if (Performance::IsProfilerEnabled())
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