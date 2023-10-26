#if defined(EDITOR)

#include "editor_ui.h"
#include "../editor.h"
#include "../../xenity.h"
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_stdlib.h>
#include "../../engine/tools/shape_spawner.h"
#include <variant>
#include <imgui/imgui_internal.h>
#include "../../engine/asset_management/project_manager.h"
#include <Windows.h>
//#include <Commdlg.h>
#include <ShObjIdl.h>
#include "../../engine/graphics/skybox.h"

bool EditorUI::DrawTreeItem(ProjectDirectory* projectDir)
{
	bool objectClicked = false;
	if (projectDir)
	{
		int childCount = (int)projectDir->subdirectories.size();
		int flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;
		if (Engine::GetCurrentProjectDirectory() == projectDir)
			flags |= ImGuiTreeNodeFlags_Selected;

		if (childCount == 0)
			flags |= ImGuiTreeNodeFlags_Leaf;

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1.0f));

		bool opened = ImGui::TreeNodeEx(projectDir->GetFolderName().c_str(), flags);
		ImGui::PopStyleColor();

		// TODO : Check if the click was on the arrow to block this condition
		if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(0))
		{
			Engine::SetCurrentProjectDirectory(projectDir);
			objectClicked = true;
		}
		if (opened)
		{
			for (int i = 0; i < childCount; i++)
			{
				bool clickedTemp = DrawTreeItem(projectDir->subdirectories[i]);
				if (clickedTemp)
					objectClicked = true;
			}
			ImGui::TreePop();
		}
	}
	return objectClicked;
}

int EditorUI::DrawTreeItem(const std::weak_ptr<GameObject>& child)
{
	int state = 0;
	auto childLock = child.lock();

	if (childLock)
	{
		int childCount = childLock->GetChildrenCount();
		int flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;
		if (Engine::selectedGameObject.lock() == childLock)
			flags |= ImGuiTreeNodeFlags_Selected;

		if (childCount == 0)
			flags |= ImGuiTreeNodeFlags_Leaf;

		if (childLock->GetLocalActive())
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1.0f));
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5, 0.5f, 0.5f, 1.0f));
		}

		bool opened = ImGui::TreeNodeEx(childLock->name.c_str(), flags);

		if (ImGui::BeginDragDropSource())
		{
			EditorUI::multiDragData.gameObjects.clear();
			EditorUI::multiDragData.transforms.clear();
			EditorUI::multiDragData.components.clear();

			EditorUI::multiDragData.gameObjects.push_back(childLock.get());
			EditorUI::multiDragData.transforms.push_back(childLock->GetTransform().get());
			int componentCount = childLock->GetComponentCount();
			for (int i = 0; i < componentCount; i++)
			{
				EditorUI::multiDragData.components.push_back(childLock->components[i].get());
			}
			std::string payloadName = "MultiDragData";
			int emptyInt = 0;
			ImGui::SetDragDropPayload(payloadName.c_str(), &emptyInt, sizeof(int), 0);
			ImGui::Text(childLock->name.c_str());
			ImGui::EndDragDropSource();
		}
		ImGui::PopStyleColor();

		std::shared_ptr <GameObject> droppedGameObject = nullptr;
		if (DragDropTarget("GameObject", droppedGameObject))
		{
			droppedGameObject->SetParent(childLock);
			state = 1;
		}
		else
		{
			if (ImGui::IsItemHovered())
			{
				state = 1;
				if (ImGui::IsMouseReleased(0) && !ImGui::IsDragDropActive())
				{
					Engine::SetSelectedGameObject(child);
					Engine::SetSelectedFileReference(nullptr);
					state = 2;
				}
			}
		}
		if (opened)
		{
			for (int i = 0; i < childLock->GetChildrenCount(); i++)
			{
				int clickedTemp = DrawTreeItem(childLock->children[i]);
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