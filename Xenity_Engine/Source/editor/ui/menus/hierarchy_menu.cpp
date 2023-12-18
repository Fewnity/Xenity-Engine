#include "hierarchy_menu.h"
#include <imgui/imgui.h>

#include <editor/ui/editor_ui.h>
#include <engine/game_elements/gameplay_manager.h>

void HierarchyMenu::Init()
{
}

void HierarchyMenu::Draw()
{
	std::string windowName = "Hierarchy###Hierarchy" + std::to_string(id);
	bool isOpen = true;
	bool visible = ImGui::Begin(windowName.c_str(), &isOpen, ImGuiWindowFlags_NoCollapse);
	if (visible)
	{
		OnStartDrawing();

		ImGui::BeginChild("Hierarchy list", ImVec2(0, 0), true);

		bool disableDrag = false;
		//Add in the list only gameobject without parent
		for (int i = 0; i < GameplayManager::gameObjectCount; i++)
		{
			if (GameplayManager::gameObjects[i]->parent.lock() == nullptr)
			{
				int r = EditorUI::DrawTreeItem(GameplayManager::gameObjects[i], rightClickedElement);
				if (r != 0)
				{
					disableDrag = true;
				}
			}
		}

		if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered())
		{
			Editor::SetSelectedGameObject(nullptr);
			Editor::SetSelectedFileReference(nullptr);
		}
		ImGui::EndChild();

		if (!disableDrag)
		{
			std::shared_ptr <GameObject> droppedGameObject = nullptr;
			if (EditorUI::DragDropTarget("GameObject", droppedGameObject))
			{
				droppedGameObject->SetParent(nullptr);
			}
		}
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsItemHovered())
		{
			firstClickedInWindow = true;
		}
		if (ImGui::IsMouseReleased(ImGuiMouseButton_Right) && ImGui::IsItemHovered() && firstClickedInWindow)
		{
			ImGui::OpenPopup("backgroundClick");
			firstClickedInWindow = false;
		}
		if (ImGui::IsMouseReleased(ImGuiMouseButton_Right) && !disableDrag)
		{
			rightClickedElement.reset();
		}
		if (ImGui::BeginPopup("backgroundClick"))
		{
			if (rightClickedElement.lock() != nullptr)
			{
				if (ImGui::MenuItem("Destroy GameObject"))
				{
					Destroy(rightClickedElement);
					rightClickedElement.reset();
					ImGui::CloseCurrentPopup();
				}
			}
			if (ImGui::BeginMenu("GameObject"))
			{
				bool hasSelectedGameObject = Editor::GetSelectedGameObject() != nullptr;
				if (ImGui::MenuItem("Create Empty Parent", nullptr, nullptr, hasSelectedGameObject))
				{
					Editor::CreateEmptyParent();
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::MenuItem("Create Empty Child", nullptr, nullptr, hasSelectedGameObject))
				{
					Editor::CreateEmptyChild();
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::MenuItem("Create Empty"))
				{
					Editor::CreateEmpty();
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndMenu();
			}
			ImGui::EndPopup();
		}

		CalculateWindowValues();
	}
	else
	{
		ResetWindowValues();
	}

	ImGui::End();

	if (!isOpen)
	{
		Editor::RemoveMenu(this);
	}
}
