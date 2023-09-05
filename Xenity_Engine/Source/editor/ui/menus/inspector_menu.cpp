#include "inspector_menu.h"
#include <imgui/imgui.h>
#include "../../../xenity.h"
#include "../editor_ui.h"
#include "../../editor.h"
#include "../../../engine/asset_management/project_manager.h"

void InspectorMenu::Init()
{
}

void InspectorMenu::Draw()
{
	ImGuiIO& io = ImGui::GetIO();

	ImGui::Begin("Inspector", 0, ImGuiWindowFlags_NoCollapse);
	auto selectedGameObject = Engine::selectedGameObject.lock();
	std::shared_ptr<FileReference> selectedFileReference = Engine::GetSelectedFileReference();

	if (selectedFileReference)
	{
		std::string fileNameExt = selectedFileReference->file->GetFileName() + selectedFileReference->file->GetFileExtension();
		ImGui::Text(fileNameExt.c_str());
		ImGui::Separator();

		auto reflection = std::dynamic_pointer_cast<Reflection>(selectedFileReference);

		auto metaReflection = selectedFileReference->GetMetaReflection();
		if (reflection) 
		{
			auto reflectionList = reflection->GetReflection();
			if (reflectionList.size() != 0)
			{
				bool changed = EditorUI::DrawMap(reflectionList);
				if (changed) 
				{
					reflection->OnReflectionUpdated();
				}
			}
		}
		if (metaReflection.size() != 0)
		{
			EditorUI::DrawMap(metaReflection);

			if (ImGui::Button("Apply"))
			{
				ProjectManager::SaveMetaFile(selectedFileReference);
			}
		}
	}
	else if (selectedGameObject)
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

		ImGui::Spacing();
		ImGui::Separator();

		//Local position input
		ImGui::Spacing();
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed))
		{
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			{
				std::string payloadName = "Transform";
				ImGui::SetDragDropPayload(payloadName.c_str(), selectedGameObject->GetTransform().get(), sizeof(Transform));
				ImGui::Text("Transform");
				ImGui::EndDragDropSource();
			}
			Vector3 localPos = selectedGameObject->GetTransform()->GetLocalPosition();
			bool changed = EditorUI::DrawInput("Local Position", localPos);

			if (changed && (InputSystem::GetKeyDown(RETURN) || InputSystem::GetKeyDown(MOUSE_LEFT)))
			{
				selectedGameObject->GetTransform()->SetLocalPosition(localPos);
			}
			//ImGui::Text("World Position: %f %f %f", selectedGameObject->GetTransform()->GetPosition().x, selectedGameObject->GetTransform()->GetPosition().y, selectedGameObject->GetTransform()->GetPosition().z);

			//Local rotation input
			ImGui::Spacing();
			ImGui::Spacing();
			Vector3 localRot = selectedGameObject->GetTransform()->GetLocalRotation();
			changed = EditorUI::DrawInput("Local Rotation", localRot);
			if (changed && (InputSystem::GetKeyDown(RETURN) || InputSystem::GetKeyDown(MOUSE_LEFT)))
			{
				selectedGameObject->GetTransform()->SetLocalRotation(localRot);
			}
			//ImGui::Text("World Rotation: %f %f %f", selectedGameObject->GetTransform()->GetRotation().x, selectedGameObject->GetTransform()->GetRotation().y, selectedGameObject->GetTransform()->GetRotation().z);

			//Local scale input
			ImGui::Spacing();
			ImGui::Spacing();
			Vector3 localScale = selectedGameObject->GetTransform()->GetLocalScale();
			changed = EditorUI::DrawInput("Local Scale", localScale);
			if (changed && (InputSystem::GetKeyDown(RETURN) || InputSystem::GetKeyDown(MOUSE_LEFT)))
			{
				selectedGameObject->GetTransform()->SetLocalScale(localScale);
			}
			//ImGui::Text("World Scale: %f %f %f", selectedGameObject->GetTransform()->GetScale().x, selectedGameObject->GetTransform()->GetScale().y, selectedGameObject->GetTransform()->GetScale().z);
			ImGui::Separator();
		}

		//Component list
		int componentCount = selectedGameObject->GetComponentCount();
		for (int i = 0; i < componentCount; i++)
		{
			auto comp = selectedGameObject->components[i];
			//Draw component title

			float cursorY = ImGui::GetCursorPosY();

			bool isEnable = comp->GetIsEnabled();
			ImGui::SetCursorPosX(35);
			ImGui::Checkbox(EditorUI::GenerateItemId().c_str(), &isEnable);
			if (comp->GetIsEnabled() != isEnable)
			{
				comp->SetIsEnabled(isEnable);
			}
			ImGui::SetCursorPosY(cursorY);

			if (ImGui::CollapsingHeader(EditorUI::GenerateItemId().c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed))
			{
				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
				{
					std::string payloadName = "Component";
					ImGui::SetDragDropPayload(payloadName.c_str(), comp.get(), sizeof(Component));
					ImGui::Text("%s", comp->GetComponentName().c_str());
					ImGui::EndDragDropSource();
				}

				//Draw component variables
				if (EditorUI::DrawReflection(*comp))
				{
					comp->OnReflectionUpdated();
				}

				ImGui::Separator();
			}
			float lastCursorY = ImGui::GetCursorPosY();
			ImGui::SetCursorPosX(65);
			ImGui::SetCursorPosY(cursorY + 4);
			ImGui::Text(comp->GetComponentName().c_str());
			ImGui::SetCursorPosY(lastCursorY);
		}
	}

	if (Engine::GetSelectedFileReference())
	{
		ImVec2 availSize = ImGui::GetContentRegionAvail();
		float sizeY = availSize.x;
		if (availSize.x > availSize.y)
			sizeY = availSize.y;

		int textureId = 0;
		if (loadedPreview != Engine::GetSelectedFileReference())
		{
			loadedPreview = Engine::GetSelectedFileReference();
			previewText = "";
			switch (loadedPreview->fileType)
			{
			case File_Code:
				loadedPreview->file->Open(false);
				previewText = loadedPreview->file->ReadAll();
				loadedPreview->file->Close();
				break;
			}
		}
		if (loadedPreview->fileType == File_Texture) 
		{
			textureId = std::dynamic_pointer_cast<Texture>(loadedPreview)->GetTextureId();
		}

		if (previewText != "") 
		{
			sizeY = ImGui::CalcTextSize(previewText.c_str(), 0, false, availSize.x).y + 10;
		}
		

		ImGui::Text("Preview:");
		ImGui::BeginChild("Preview", ImVec2(0, sizeY), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		if (previewText != "") 
		{
			ImGui::Text(previewText.c_str());
		}
		else if (textureId != 0)
		{
			ImVec2 availArea = ImGui::GetContentRegionAvail();
			ImGui::Image((ImTextureID)textureId, availArea);

			// Print texture resolution
			std::shared_ptr<Texture> texture = std::dynamic_pointer_cast<Texture>(Engine::GetSelectedFileReference());
			std::string text = std::to_string(texture->GetWidth()) + "x" + std::to_string(texture->GetHeight());
			ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
			ImVec2 textPos;
			textPos.x = availArea.x / 2.0f - textSize.x / 2.0f + ImGui::GetCursorPosX();
			textPos.y = availArea.y - textSize.y / 2.0f;
			ImGui::SetCursorPos(textPos);
			ImGui::Text(text.c_str());
		}
		else
			ImGui::Text("No preview available");

		ImGui::EndChild();
	}

	ImGui::End();
}
