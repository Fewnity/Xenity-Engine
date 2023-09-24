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

bool EditorUI::DrawTreeItem(const std::weak_ptr<GameObject>& child)
{
	bool objectClicked = false;
	auto childLock = child.lock();

	if (childLock)
	{
		int childCount = (int)childLock->children.size();
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
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			std::string payloadName = "GameObject";
			ImGui::SetDragDropPayload(payloadName.c_str(), childLock.get(), sizeof(GameObject));
			ImGui::Text(childLock->name.c_str());
			ImGui::EndDragDropSource();
		}
		ImGui::PopStyleColor();

		GameObject* droppedGameObject = nullptr;
		if (DragDropTarget("GameObject", droppedGameObject))
		{
			droppedGameObject->SetParent(childLock);
		}
		else
		{
			if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(0) && !ImGui::IsDragDropActive())
			{
				Engine::SetSelectedGameObject(child);
				Engine::SetSelectedFileReference(nullptr);
				objectClicked = true;
			}
		}
		if (opened)
		{
			for (int i = 0; i < childCount; i++)
			{
				bool clickedTemp = DrawTreeItem(childLock->children[i]);
				if (clickedTemp)
					objectClicked = true;
			}
			ImGui::TreePop();
		}
	}
	return objectClicked;
}

#endif // #if defined(EDITOR)