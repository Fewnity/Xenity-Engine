#include "hierarchy_menu.h"
#include <imgui/imgui.h>

#include <editor/ui/editor_ui.h>
#include <engine/game_elements/gameplay_manager.h>

void HierarchyMenu::Init()
{
}

void HierarchyMenu::Draw()
{
	bool visible = ImGui::Begin("Hierarchy", 0, ImGuiWindowFlags_NoCollapse);
	if (visible)
	{
		bool disableDrag = false;

		ImGui::BeginChild("Hierarchy list", ImVec2(0, 0), true);

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
		isFocused = ImGui::IsWindowFocused();
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
		if (ImGui::IsMouseReleased(ImGuiMouseButton_Right) && ImGui::IsItemHovered())
		{
			ImGui::OpenPopup("backgroundClick");
		}
		if (ImGui::IsMouseReleased(ImGuiMouseButton_Right) && !disableDrag)
		{
			rightClickedElement = nullptr;
		}
		if (ImGui::BeginPopup("backgroundClick"))
		{
			if (rightClickedElement != nullptr) 
			{
				if (ImGui::MenuItem("Destroy GameObject"))
				{
					Destroy(rightClickedElement);
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
	}
	ImGui::End();
}
