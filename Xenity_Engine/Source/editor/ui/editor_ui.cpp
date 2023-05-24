#include "editor_ui.h"
#include "../../xenity.h"
Engine* EditorUI::engine = nullptr;
int EditorUI::uiId = 0;
float EditorUI::nextFpsUpdate = 0;
float EditorUI::lastFps = 0;

void EditorUI::Init()
{
	//ImGui::GetStyle().WindowRounding = 10;

	Debug::Print("---- Editor UI initiated ----");
}

void EditorUI::NewFrame()
{
	/*ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();*/
	EditorUI::uiId = 0;
}

void EditorUI::DrawInspector()
{
	/*ImGuiIO& io = ImGui::GetIO();

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
			Component* comp = selectedGameObject->components[i];
			std::string componentName = "- " + comp->componentName;
			//std::string componentName = typeid(comp).name();

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
	ImGui::End();*/
}

void EditorUI::DrawTreeItem(GameObject* child)
{
	/*int childCount = child->children.size();
	int flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;
	if (Engine::selectedGameObject == child)
		flags |= ImGuiTreeNodeFlags_Selected;

	if (childCount == 0)
		flags |= ImGuiTreeNodeFlags_Leaf;

	if (child->GetLocalActive())
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1.0f));
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5, 0.5f, 0.5f, 1.0f));
	}

	bool opened = ImGui::TreeNodeEx(child->name.c_str(), flags);
	ImGui::PopStyleColor();
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
	}*/
}

void EditorUI::DrawHierarchy()
{
	/*ImGui::Begin("Hierarchy");
	//ImGui::SetWindowFontScale(2);
	if (!ImGui::IsWindowCollapsed())
	{
		ImGui::BeginChild("Hierarchy list", ImVec2(0, 0), true);

		for (int i = 0; i < engine->gameObjectCount; i++)
		{
			if (engine->GetGameObjects()[i]->parent == nullptr)
			{
				DrawTreeItem(engine->GetGameObjects()[i]);
			}
		}
		ImGui::EndChild();
	}


	ImGui::End();*/
}

void EditorUI::DrawTextCentered(std::string text)
{
	/*float windowWidth = ImGui::GetWindowSize().x;
	float textWidth = ImGui::CalcTextSize(text.c_str()).x;

	ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
	ImGui::Text(text.c_str());*/
}

bool EditorUI::DrawFloatInput(std::string inputName, float& value)
{
	/*std::string inputName1 = "##" + std::to_string(uiId);
	uiId += 1;

	ImGui::Text(inputName.c_str());
	float titleWidth = ImGui::GetItemRectSize().x;
	ImGui::SameLine();
	float oldValue = float(value);
	float startAvailSize = ImGui::GetContentRegionAvail().x;
	ImGui::SameLine();
	ImGui::SetNextItemWidth(startAvailSize);
	ImGui::InputFloat(inputName1.c_str(), &value, 0, 0, "%f");

	return value != oldValue;*/
	return false;
}

bool EditorUI::DrawIntInput(std::string inputName, int& value)
{
	/*std::string inputName1 = "##" + std::to_string(uiId);
	uiId += 1;

	ImGui::Text(inputName.c_str());
	float titleWidth = ImGui::GetItemRectSize().x;
	ImGui::SameLine();
	int oldValue = int(value);
	float startAvailSize = ImGui::GetContentRegionAvail().x;
	ImGui::SameLine();
	ImGui::SetNextItemWidth(startAvailSize);
	ImGui::InputInt(inputName1.c_str(), &value);

	return value != oldValue;*/
	return false;
}

bool EditorUI::DrawVector3Input(std::string inputName, std::string name1, std::string name2, std::string name3, Vector3& value)
{
	/*std::string inputName1 = "##" + std::to_string(uiId);
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

	return value != oldValue;*/
	return false;
}

void EditorUI::DrawProfiler()
{
	/*ImGuiIO& io = ImGui::GetIO();

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

	ImGui::End();*/
}