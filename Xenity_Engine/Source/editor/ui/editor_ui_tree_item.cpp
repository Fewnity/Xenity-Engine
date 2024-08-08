// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

// ImGui
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <editor/ui/editor_ui.h>
#include <editor/editor.h>

#include <engine/asset_management/project_manager.h>
#include <engine/inputs/input_system.h>
#include <editor/ui/menus/scene_menu.h>
#include <engine/game_elements/gameplay_manager.h>

bool EditorUI::DrawTreeItem(const std::shared_ptr<ProjectDirectory>& projectDir)
{
	static bool cancelClick = false;
	bool objectClicked = false;
	if (projectDir)
	{
		const int childCount = (int)projectDir->subdirectories.size();
		int flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;
		if (Editor::GetCurrentProjectDirectory() == projectDir)
			flags |= ImGuiTreeNodeFlags_Selected;

		if (childCount == 0)
			flags |= ImGuiTreeNodeFlags_Leaf;

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1.0f));

		const std::string nodeName = projectDir->GetFolderName() + "##TIPD" + std::to_string(projectDir->uniqueId);
		const bool opened = ImGui::TreeNodeEx(nodeName.c_str(), flags);
		ImGui::PopStyleColor();
		const bool justOpened = ImGui::IsItemToggledOpen();
		if (justOpened)
		{
			cancelClick = true;
		}

		// TODO : Check if the click was on the arrow to block this condition
		if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(0))
		{
			if (cancelClick)
			{
				cancelClick = false;
			}
			else
			{
				Editor::SetCurrentProjectDirectory(projectDir);
				objectClicked = true;
			}
		}
		if (opened)
		{
			for (int i = 0; i < childCount; i++)
			{
				const bool clickedTemp = DrawTreeItem(projectDir->subdirectories[i]);
				if (clickedTemp)
					objectClicked = true;
			}
			ImGui::TreePop();
		}
	}
	return objectClicked;
}

bool EditorUI::DragDropOrderGameObject(std::shared_ptr <GameObject>& droppedGameObject, const std::shared_ptr <GameObject>& dropAreaOwner, bool isParent, bool isParentOpened)
{
	if (DragDropTarget("GameObject", droppedGameObject))
	{
		std::shared_ptr<GameObject> newParent = dropAreaOwner->GetParent().lock();
		if (isParentOpened && dropAreaOwner->GetChildrenCount() != 0 && !isParent)
		{
			newParent = dropAreaOwner;
		}
		droppedGameObject->SetParent(newParent);
		if (!newParent)
		{
			const int gameObjectCount = GameplayManager::gameObjectCount;
			int gameObjectIndex = -1;
			int gameObjectToMoveIndex = -1;
			for (int i = 0; i < gameObjectCount; i++)
			{
				if (GameplayManager::gameObjects[i] == dropAreaOwner)
				{
					gameObjectIndex = i;
					break;
				}
			}
			for (int i = 0; i < gameObjectCount; i++)
			{
				if (GameplayManager::gameObjects[i] == droppedGameObject)
				{
					gameObjectToMoveIndex = i;
					break;
				}
			}
			int offset = 0;
			if (gameObjectToMoveIndex > gameObjectIndex)
			{
				offset = 1;
			}
			GameplayManager::gameObjects.erase(GameplayManager::gameObjects.begin() + gameObjectToMoveIndex);
			GameplayManager::gameObjects.insert(GameplayManager::gameObjects.begin() + gameObjectIndex + offset, droppedGameObject);
		}
		else
		{
			const int gameObjectCount = newParent->GetChildrenCount();
			int gameObjectIndex = -1;
			int gameObjectToMoveIndex = -1;

			for (int i = 0; i < gameObjectCount; i++)
			{
				if (newParent->GetChildren()[i].lock() == dropAreaOwner)
				{
					gameObjectIndex = i;
					break;
				}
			}
			for (int i = 0; i < gameObjectCount; i++)
			{
				if (newParent->GetChildren()[i].lock() == droppedGameObject)
				{
					gameObjectToMoveIndex = i;
					break;
				}
			}

			if (newParent == dropAreaOwner)
			{
				gameObjectIndex = -1;
			}

			if (gameObjectIndex != gameObjectToMoveIndex && (gameObjectIndex != -1 || newParent == dropAreaOwner) && gameObjectToMoveIndex != -1)
			{
				std::vector<std::weak_ptr<GameObject>>& parentChildren = newParent->GetChildren();
				parentChildren.erase(parentChildren.begin() + gameObjectToMoveIndex);
				parentChildren.insert(parentChildren.begin() + (gameObjectIndex + 1), droppedGameObject);
			}
		}
		return true;
	}
	return false;
}

int EditorUI::DrawTreeItem(const std::shared_ptr<GameObject>& gameObject, std::weak_ptr<GameObject>& rightClickedElement)
{
	static bool cancelClick = false;
	int state = 0;

	if (gameObject)
	{
		const int childCount = gameObject->GetChildrenCount();
		int flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;

		if (gameObject->isSelected)
			flags |= ImGuiTreeNodeFlags_Selected;

		if (childCount == 0)
			flags |= ImGuiTreeNodeFlags_Leaf;

		if (gameObject->IsLocalActive())
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1.0f));
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5, 0.5f, 0.5f, 1.0f));
		}

		const std::string nodeName = gameObject->GetName() + "##TIGO" + std::to_string(gameObject->GetUniqueId());
		const bool opened = ImGui::TreeNodeEx(nodeName.c_str(), flags);
		const bool justOpened = ImGui::IsItemToggledOpen();
		if (justOpened)
		{
			cancelClick = true;
		}
		if (ImGui::BeginDragDropSource())
		{
			EditorUI::multiDragData.gameObjects.clear();
			EditorUI::multiDragData.transforms.clear();
			EditorUI::multiDragData.components.clear();

			EditorUI::multiDragData.gameObjects.push_back(gameObject.get());
			EditorUI::multiDragData.transforms.push_back(gameObject->GetTransform().get());
			const int componentCount = gameObject->GetComponentCount();
			for (int i = 0; i < componentCount; i++)
			{
				EditorUI::multiDragData.components.push_back(gameObject->components[i].get());
			}
			const std::string payloadName = "MultiDragData";
			int emptyInt = 0;
			ImGui::SetDragDropPayload(payloadName.c_str(), &emptyInt, sizeof(int), 0);
			ImGui::Text("%s", gameObject->GetName().c_str());
			ImGui::EndDragDropSource();
		}
		ImGui::PopStyleColor();

		std::shared_ptr <GameObject> droppedGameObject = nullptr;
		if (DragDropTarget("GameObject", droppedGameObject))
		{
			droppedGameObject->SetParent(gameObject);
			state = 1;
		}
		else
		{
			if (ImGui::IsItemHovered())
			{
				state = 1;
				if (!ImGui::IsDragDropActive())
				{
					if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					{
						if (cancelClick)
						{
							cancelClick = false;
						}
						else
						{
							Editor::SetSelectedGameObject(gameObject);
							state = 2;
							std::vector<std::shared_ptr<SceneMenu>> sceneMenus = Editor::GetMenus<SceneMenu>();
							for (std::shared_ptr<SceneMenu> sceneMenu : sceneMenus)
							{
								sceneMenu->FocusSelectedObject();
							}
						}
					}
					else if (ImGui::IsMouseReleased(0))
					{
						if (cancelClick)
						{
							cancelClick = false;
						}
						else
						{
							if (InputSystem::GetKey(KeyCode::LEFT_CONTROL))
							{
								Editor::AddSelectedGameObject(gameObject);
								Editor::SetSelectedFileReference(nullptr);
							}
							else
								Editor::SetSelectedGameObject(gameObject);

							state = 2;
						}
					}
					else if (ImGui::IsMouseReleased(1))
					{
						rightClickedElement = gameObject;
						state = 3;
					}
				}
			}
		}

		if ((!opened && gameObject->GetChildrenCount() == 0) || (opened && gameObject->GetChildrenCount() != 0))
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 4);
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3);
			if (DragDropOrderGameObject(droppedGameObject, gameObject, false, opened))
			{
				state = 1;
			}
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 4);
		}

		if (opened)
		{
			for (int i = 0; i < gameObject->GetChildrenCount(); i++)
			{
				const int clickedTemp = DrawTreeItem(gameObject->GetChildren()[i].lock(), rightClickedElement);
				if (clickedTemp == 1)
					state = 1;
				else if (clickedTemp == 2)
					state = 2;
			}
			ImGui::TreePop();
		}
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 4);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3);
		if (DragDropOrderGameObject(droppedGameObject, gameObject, true, opened))
		{
			state = 1;
		}
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 4);
	}

	return state;
}