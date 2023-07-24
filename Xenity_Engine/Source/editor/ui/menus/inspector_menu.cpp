#include "inspector_menu.h"
#include <imgui/imgui.h>
#include "../../../xenity.h"
#include "../editor_ui.h"
#include "../../editor.h"

void InspectorMenu::Init()
{
}

void InspectorMenu::Draw()
{
	ImGuiIO& io = ImGui::GetIO();

	//Create Window
	ImGuiViewport* viewport = ImGui::GetMainViewport();

	float topOffset = 20 * EditorUI::GetUiScale();
	ImGui::SetNextWindowPos(ImVec2(viewport->Size.x, topOffset), 0, ImVec2(1, 0));
	ImGui::SetNextWindowSizeConstraints(ImVec2(50, viewport->Size.y - topOffset), ImVec2(viewport->Size.x / 2.0f, viewport->Size.y - topOffset));
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
		bool changed = EditorUI::DrawInput("Local Position", localPos);

		if (changed && (InputSystem::GetKeyDown(RETURN) || InputSystem::GetKeyDown(MOUSE_LEFT)))
		{
			selectedGameObject->GetTransform()->SetLocalPosition(localPos);
		}
		ImGui::Text("World Position: %f %f %f", selectedGameObject->GetTransform()->GetPosition().x, selectedGameObject->GetTransform()->GetPosition().y, selectedGameObject->GetTransform()->GetPosition().z);

		//Local rotation input
		ImGui::Spacing();
		ImGui::Spacing();
		Vector3 localRot = selectedGameObject->GetTransform()->GetLocalRotation();
		changed = EditorUI::DrawInput("Local Rotation", localRot);
		if (changed && (InputSystem::GetKeyDown(RETURN) || InputSystem::GetKeyDown(MOUSE_LEFT)))
		{
			selectedGameObject->GetTransform()->SetLocalRotation(localRot);
		}
		ImGui::Text("World Rotation: %f %f %f", selectedGameObject->GetTransform()->GetRotation().x, selectedGameObject->GetTransform()->GetRotation().y, selectedGameObject->GetTransform()->GetRotation().z);

		//Local scale input
		ImGui::Spacing();
		ImGui::Spacing();
		Vector3 localScale = selectedGameObject->GetTransform()->GetLocalScale();
		changed = EditorUI::DrawInput("Local Scale", localScale);
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
			EditorUI::DrawReflection(*comp);

			ImGui::Separator();
		}
	}
	ImGui::End();
}
