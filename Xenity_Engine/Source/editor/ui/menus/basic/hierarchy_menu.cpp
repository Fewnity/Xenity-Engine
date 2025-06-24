// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "hierarchy_menu.h"

#include <imgui/imgui.h>

#include <editor/ui/editor_ui.h>
#include <editor/ui/utils/menu_builder.h>
#include <editor/command/commands/delete.h>

#include <engine/game_elements/gameplay_manager.h>

void HierarchyMenu::Init()
{
}

void HierarchyMenu::Draw()
{
	const std::string windowName = "Hierarchy###Hierarchy" + std::to_string(id);

	const bool visible = ImGui::Begin(windowName.c_str(), &m_isActive, ImGuiWindowFlags_NoCollapse);
	if (visible)
	{
		OnStartDrawing();

		ImGui::BeginChild("Hierarchy list", ImVec2(0, 0), ImGuiChildFlags_Borders);

		ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		bool disableDrag = false;
		//Add in the list only gameobject without parent
		for (int i = 0; i < GameplayManager::gameObjectCount; i++)
		{
			if (GameplayManager::gameObjects[i]->GetParent().lock() == nullptr)
			{
				const int r = EditorUI::DrawTreeItem(GameplayManager::gameObjects[i], m_rightClickedElement);
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
			m_firstClickedInWindow = true;
		}
		if (ImGui::IsMouseReleased(ImGuiMouseButton_Right) && !disableDrag)
		{
			m_rightClickedElement.reset();
		}


		// Start creating right click menu
		RightClickMenu backgroundRightClickMenu = RightClickMenu("HierarchyRightClickMenu");
		RightClickMenuState rightClickState = backgroundRightClickMenu.Check(!m_firstClickedInWindow, false);

		if (rightClickState != RightClickMenuState::Closed)
		{
		std::function<void()> destroyGameObjectFunc = [this]()
			{
				if (m_rightClickedElement.lock())
				{
					auto command = std::make_shared<InspectorDeleteGameObjectCommand>(*m_rightClickedElement.lock());
					CommandManager::AddCommandAndExecute(command);
					m_rightClickedElement.reset();
				}
			};

			const size_t selectedGameObjectCount = Editor::GetSelectedGameObjects().size();
			const bool hasSelectedGameObject = selectedGameObjectCount != 0;
			const bool hasOneSelectedGameObject = selectedGameObjectCount == 1;

			// -
			RightClickMenuItem* destroyGameObjectMenuItem = backgroundRightClickMenu.AddItem("Destroy GameObject", destroyGameObjectFunc);
			RightClickMenuItem* gameObjectMenuItem = backgroundRightClickMenu.AddItem("GameObject");
			destroyGameObjectMenuItem->SetIsVisible(m_rightClickedElement.lock() != nullptr);
			//--
			RightClickMenuItem* createEmptyParentMenuItem = gameObjectMenuItem->AddItem("Create Empty Parent", []() { Editor::CreateEmptyParent(); });
			gameObjectMenuItem->AddItem("Create Empty", [hasSelectedGameObject]() { if(hasSelectedGameObject) Editor::CreateEmptyChild(); else Editor::CreateEmpty(); });
			createEmptyParentMenuItem->SetIsEnabled(hasOneSelectedGameObject);
		}

		backgroundRightClickMenu.Draw();

		if (rightClickState == RightClickMenuState::JustOpened)
			m_firstClickedInWindow = false;

		CalculateWindowValues();
		if (isChildFocused)
			m_isFocused = isChildFocused;
		if (isChildHovered)
			m_isHovered = isChildHovered;
	}
	else
	{
		ResetWindowValues();
	}

	ImGui::End();
}
