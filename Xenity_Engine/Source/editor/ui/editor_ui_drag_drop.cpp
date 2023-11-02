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

bool EditorUI::DragDropTarget(const std::string& name, std::shared_ptr<Component>& ref, uint64_t typeId)
{
	if (ImGui::BeginDragDropTarget())
	{
		ImGuiDragDropFlags target_flags = 0;
		Component* comp = nullptr;
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(name.c_str(), target_flags))
		{
			comp = ((Component*)payload->Data);
		}
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MultiDragData", target_flags))
		{
			size_t compCount = EditorUI::multiDragData.components.size();

			for (size_t i = 0; i < compCount; i++)
			{
				if (typeid(*EditorUI::multiDragData.components[i]).hash_code() == typeId)
				{
					comp = EditorUI::multiDragData.components[i];
					break;
				}
			}

		}

		if (comp)
		{
			ref = comp->shared_from_this();
			return true;
		}

		ImGui::EndDragDropTarget();
	}
	return false;
}

bool EditorUI::DragDropTarget(const std::string& name, std::shared_ptr<Collider>& ref)
{
	if (ImGui::BeginDragDropTarget())
	{
		ImGuiDragDropFlags target_flags = 0;
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(name.c_str(), target_flags))
		{
			Collider* obj = ((Collider*)payload->Data);

			if (obj)
			{
				ref = std::dynamic_pointer_cast<Collider>(((Component*)obj)->shared_from_this());
				return true;
			}
		}
		ImGui::EndDragDropTarget();
	}
	return false;
}

bool EditorUI::DragDropTarget(const std::string& name, std::shared_ptr<GameObject>& ref)
{
	if (ImGui::BeginDragDropTarget())
	{
		ImGuiDragDropFlags target_flags = 0;
		GameObject* gameObject = nullptr;
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(name.c_str(), target_flags))
		{
			gameObject = ((GameObject*)payload->Data);
		}
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MultiDragData", target_flags))
		{
			gameObject = EditorUI::multiDragData.gameObjects[0];
		}

		if (gameObject)
		{
			ref = gameObject->shared_from_this();
			return true;
		}

		ImGui::EndDragDropTarget();
	}
	return false;
}

bool EditorUI::DragDropTarget(const  std::string& name, std::shared_ptr<Transform>& ref)
{
	if (ImGui::BeginDragDropTarget())
	{
		ImGuiDragDropFlags target_flags = 0;
		Transform* trans = nullptr;
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(name.c_str(), target_flags))
		{
			trans = ((Transform*)payload->Data);
		}
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MultiDragData", target_flags))
		{
			trans = EditorUI::multiDragData.transforms[0];
		}

		if (trans)
		{
			ref = trans->shared_from_this();
			return true;
		}

		ImGui::EndDragDropTarget();
	}
	return false;
}

#endif // #if defined(EDITOR)