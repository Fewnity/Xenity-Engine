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

bool EditorUI::DragDropTarget(const std::string& name, std::shared_ptr <FileReference>& ref)
{
	if (ImGui::BeginDragDropTarget())
	{
		ImGuiDragDropFlags target_flags = 0;
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(name.c_str(), target_flags))
		{
			FileReference* movedFile = (FileReference*)payload->Data;

			std::shared_ptr<FileReference> file = ProjectManager::GetFileReferenceById(movedFile->fileId);
			if (file)
			{
				file->LoadFileReference();
				ref = file;
				return true;
			}
		}
		ImGui::EndDragDropTarget();
	}
	return false;
}

bool EditorUI::DragDropTarget(const std::string& name, Component*& ref)
{
	if (ImGui::BeginDragDropTarget())
	{
		ImGuiDragDropFlags target_flags = 0;
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(name.c_str(), target_flags))
		{
			Component* comp = ((Component*)payload->Data);

			if (comp)
			{
				ref = comp;
				return true;
			}
		}
		ImGui::EndDragDropTarget();
	}
	return false;
}

bool EditorUI::DragDropTarget(const std::string& name, GameObject*& ref)
{
	if (ImGui::BeginDragDropTarget())
	{
		ImGuiDragDropFlags target_flags = 0;
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(name.c_str(), target_flags))
		{
			GameObject* gameObject = ((GameObject*)payload->Data);

			if (gameObject)
			{
				ref = gameObject;
				return true;
			}
		}
		ImGui::EndDragDropTarget();
	}
	return false;
}

bool EditorUI::DragDropTarget(const  std::string& name, Transform*& ref)
{
	if (ImGui::BeginDragDropTarget())
	{
		ImGuiDragDropFlags target_flags = 0;
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(name.c_str(), target_flags))
		{
			Transform* trans = ((Transform*)payload->Data);

			if (trans)
			{
				ref = trans;
				return true;
			}
		}
		ImGui::EndDragDropTarget();
	}
	return false;
}

#endif // #if defined(EDITOR)