// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "hierarchy_menu.h"
#include <imgui/imgui.h>

#include <editor/ui/editor_ui.h>
#include <editor/ui/utils/menu_builder.h>
#include <engine/game_elements/gameplay_manager.h>

void HierarchyMenu::Init()
{
}

void HierarchyMenu::Draw()
{
	const std::string windowName = "Hierarchy###Hierarchy" + std::to_string(id);
	bool isOpen = true;
	const bool visible = ImGui::Begin(windowName.c_str(), &isOpen, ImGuiWindowFlags_NoCollapse);
	if (visible)
	{
		OnStartDrawing();

		ImGui::BeginChild("Hierarchy list", ImVec2(0, 0), ImGuiChildFlags_Border);

		ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		bool disableDrag = false;
		//Add in the list only gameobject without parent
		for (int i = 0; i < GameplayManager::gameObjectCount; i++)
		{
			if (GameplayManager::gameObjects[i]->GetParent().lock() == nullptr)
			{
				const int r = EditorUI::DrawTreeItem(GameplayManager::gameObjects[i], rightClickedElement);
				if (r != 0)
				{
					disableDrag = true;
				}
			}
		}
		ImGui::PopStyleColor();

		if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered() && !disableDrag)
		{
			Editor::SetSelectedGameObject(nullptr);
			Editor::SetSelectedFileReference(nullptr);
		}
		const bool isChildFocused = ImGui::IsWindowFocused();
		const bool isChildHovered = ImGui::IsWindowHovered();
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
		if (ImGui::IsMouseReleased(ImGuiMouseButton_Right) && !disableDrag)
		{
			rightClickedElement.reset();
		}


		// Start creating right click menu
		RightClickMenu backgroundRightClickMenu = RightClickMenu("HierarchyRightClickMenu");
		RightClickMenuState rightClickState = backgroundRightClickMenu.Check(!firstClickedInWindow);

		if (rightClickState != RightClickMenuState::Closed)
		{
		std::function<void()> destroyGameObjectFunc = [this]()
			{
				Destroy(rightClickedElement);
				rightClickedElement.reset();
			};

			const size_t selectedGameObjectCount = Editor::GetSelectedGameObjects().size();
			const bool hasSelectedGameObject = selectedGameObjectCount != 0;
			const bool hasOneSelectedGameObject = selectedGameObjectCount == 1;

			// -
			RightClickMenuItem* destroyGameObjectMenuItem = backgroundRightClickMenu.AddItem("Destroy GameObject", destroyGameObjectFunc);
			RightClickMenuItem* gameObjectMenuItem = backgroundRightClickMenu.AddItem("GameObject");
			destroyGameObjectMenuItem->SetIsVisible(rightClickedElement.lock() != nullptr);
			//--
			RightClickMenuItem* createEmptyParentMenuItem = gameObjectMenuItem->AddItem("Create Empty Parent", []() { Editor::CreateEmptyParent(); });
			RightClickMenuItem* createEmptyChildMenuItem = gameObjectMenuItem->AddItem("Create Empty Child", []() { Editor::CreateEmptyChild(); });
			gameObjectMenuItem->AddItem("Create Empty", []() { Editor::CreateEmpty(); });
			createEmptyParentMenuItem->SetIsEnabled(hasOneSelectedGameObject);
			createEmptyChildMenuItem->SetIsEnabled(hasSelectedGameObject);
		}

		backgroundRightClickMenu.Draw();

		if (rightClickState == RightClickMenuState::JustOpened)
			firstClickedInWindow = false;

		CalculateWindowValues();
		if (isChildFocused)
			isFocused = isChildFocused;
		if (isChildHovered)
			isHovered = isChildHovered;
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
