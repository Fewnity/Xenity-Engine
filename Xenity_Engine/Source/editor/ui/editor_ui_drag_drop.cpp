// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#if defined(EDITOR)

// ImGui
#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include <imgui/imgui_internal.h>

#include <editor/ui/editor_ui.h>
#include <editor/editor.h>

#include <engine/asset_management/project_manager.h>
#include <engine/physics/collider.h>

bool EditorUI::DragDropTarget(const std::string& name, std::shared_ptr <FileReference>& ref, bool getOnMouseRelease)
{
	if (ImGui::BeginDragDropTarget())
	{
		ImGuiDragDropFlags target_flags = 0;
		if(!getOnMouseRelease)
			target_flags |= ImGuiDragDropFlags_AcceptBeforeDelivery;
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

bool EditorUI::DragDropTarget(const std::string& name, std::shared_ptr <ProjectDirectory>& ref, bool getOnMouseRelease)
{
	if (ImGui::BeginDragDropTarget())
	{
		ImGuiDragDropFlags target_flags = 0;
		if (!getOnMouseRelease)
			target_flags |= ImGuiDragDropFlags_AcceptBeforeDelivery;
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(name.c_str(), target_flags))
		{
			ProjectDirectory* movedFolder = (ProjectDirectory*)payload->Data;
			std::shared_ptr<ProjectDirectory> directory = ProjectManager::FindProjectDirectory(ProjectManager::GetProjectDirectory(), movedFolder->path);
			if (directory)
			{
				ref = directory;
				return true;
			}
		}
		ImGui::EndDragDropTarget();
	}
	return false;
}

bool EditorUI::DragDropTarget(const std::string& name, std::shared_ptr<Component>& ref, bool getOnMouseRelease)
{
	if (ImGui::BeginDragDropTarget())
	{
		ImGuiDragDropFlags target_flags = 0;
		if (!getOnMouseRelease)
			target_flags |= ImGuiDragDropFlags_AcceptBeforeDelivery;
		Component* comp = nullptr;
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(name.c_str(), target_flags))
		{
			comp = ((Component*)payload->Data);
		}
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MultiDragData", target_flags))
		{
			const size_t compCount = EditorUI::multiDragData.components.size();

			for (size_t i = 0; i < compCount; i++)
			{
				const uint64_t id = typeid(*EditorUI::multiDragData.components[i]).hash_code();
				if ("Type" + std::to_string(id) == name)
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

bool EditorUI::DragDropTarget(const std::string& name, std::shared_ptr<Collider>& ref, bool getOnMouseRelease)
{
	if (ImGui::BeginDragDropTarget())
	{
		ImGuiDragDropFlags target_flags = 0;
		if (!getOnMouseRelease)
			target_flags |= ImGuiDragDropFlags_AcceptBeforeDelivery;
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

bool EditorUI::DragDropTarget(const std::string& name, std::shared_ptr<GameObject>& ref, bool getOnMouseRelease)
{
	if (ImGui::BeginDragDropTarget())
	{
		ImGuiDragDropFlags target_flags = 0;
		if (!getOnMouseRelease)
			target_flags |= ImGuiDragDropFlags_AcceptBeforeDelivery;
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

bool EditorUI::DragDropTarget(const std::string& name, std::shared_ptr<Transform>& ref, bool getOnMouseRelease)
{
	if (ImGui::BeginDragDropTarget())
	{
		ImGuiDragDropFlags target_flags = 0;
		if (!getOnMouseRelease)
			target_flags |= ImGuiDragDropFlags_AcceptBeforeDelivery;
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