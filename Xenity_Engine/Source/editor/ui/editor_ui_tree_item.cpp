#if defined(EDITOR)

// ImGui
#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include <imgui/imgui_internal.h>

#include <editor/ui/editor_ui.h>
#include <editor/editor.h>

#include <engine/asset_management/project_manager.h>
#include <engine/debug/debug.h>
#include <engine/inputs/input_system.h>
#include <editor/ui/menus/scene_menu.h>

bool EditorUI::DrawTreeItem(std::shared_ptr<ProjectDirectory> projectDir)
{
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

		// TODO : Check if the click was on the arrow to block this condition
		if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(0))
		{
			Editor::SetCurrentProjectDirectory(projectDir);
			objectClicked = true;
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

int EditorUI::DrawTreeItem(const std::shared_ptr<GameObject>& gameObject, std::weak_ptr<GameObject>& rightClickedElement)
{
	int state = 0;

	if (gameObject)
	{
		const int childCount = gameObject->GetChildrenCount();
		int flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;

		if (gameObject->isSelected)
			flags |= ImGuiTreeNodeFlags_Selected;

		if (childCount == 0)
			flags |= ImGuiTreeNodeFlags_Leaf;

		if (gameObject->GetLocalActive())
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1.0f));
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5, 0.5f, 0.5f, 1.0f));
		}

		const std::string nodeName = gameObject->name + "##TIGO" + std::to_string(gameObject->GetUniqueId());
		const bool opened = ImGui::TreeNodeEx(nodeName.c_str(), flags);

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
			ImGui::Text(gameObject->name.c_str());
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
						Editor::SetSelectedGameObject(gameObject);
						Editor::SetSelectedFileReference(nullptr);
						state = 2;
						std::vector<std::shared_ptr<SceneMenu>> sceneMenus = Editor::GetMenus<SceneMenu>();
						for (std::shared_ptr<SceneMenu> sceneMenu : sceneMenus)
						{
							sceneMenu->FocusSelectedObject();
						}
					}
					else if (ImGui::IsMouseReleased(0))
					{
						if(InputSystem::GetKey(KeyCode::LEFT_CONTROL))
							Editor::AddSelectedGameObject(gameObject);
						else
							Editor::SetSelectedGameObject(gameObject);

						Editor::SetSelectedFileReference(nullptr);
						state = 2;
					}
					else if (ImGui::IsMouseReleased(1))
					{
						rightClickedElement = gameObject;
						state = 3;
					}
				}
			}
		}

		if (opened)
		{
			for (int i = 0; i < gameObject->GetChildrenCount(); i++)
			{
				const int clickedTemp = DrawTreeItem(gameObject->children[i].lock(), rightClickedElement);
				if (clickedTemp == 1)
					state = 1;
				else if (clickedTemp == 2)
					state = 2;
			}
			ImGui::TreePop();
		}
	}
	return state;
}

#endif // #if defined(EDITOR)