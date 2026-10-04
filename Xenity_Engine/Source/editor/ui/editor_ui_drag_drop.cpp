// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2026 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#if defined(EDITOR)

// ImGui
#include <imgui/imgui.h>

#include <editor/ui/editor_ui.h>

#include <engine/asset_management/project_manager.h>
#include <engine/physics/collider.h>
#include <engine/game_elements/gameobject.h>
#include <engine/game_elements/transform.h>

#include <cstring>

bool EditorUI::DragDropTarget(const std::string& name, std::shared_ptr <FileReference>& ref, bool getOnMouseRelease)
{
	bool returnValue = false;
	if (ImGui::BeginDragDropTarget())
	{
		ImGuiDragDropFlags target_flags = 0;
		if(!getOnMouseRelease)
			target_flags |= ImGuiDragDropFlags_AcceptBeforeDelivery;
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(name.c_str(), target_flags))
		{
			// The payload is the file id
			uint64_t movedFileId = 0;
			memcpy(&movedFileId, payload->Data, sizeof(uint64_t));

			std::shared_ptr<FileReference> file = ProjectManager::GetFileReferenceById(movedFileId);
			if (file)
			{
				FileReference::LoadOptions loadOptions;
				loadOptions.platform = Application::GetPlatform();
				loadOptions.threaded = false;
				file->LoadFileReference(loadOptions);
				ref = file;
				returnValue = true;
			}
		}
		ImGui::EndDragDropTarget();
	}
	return returnValue;
}

bool EditorUI::DragDropTarget(const std::string& name, std::shared_ptr <ProjectDirectory>& ref, bool getOnMouseRelease)
{
	bool returnValue = false;
	if (ImGui::BeginDragDropTarget())
	{
		ImGuiDragDropFlags target_flags = 0;
		if (!getOnMouseRelease)
			target_flags |= ImGuiDragDropFlags_AcceptBeforeDelivery;
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(name.c_str(), target_flags))
		{
			// The payload is the folder path
			const std::string movedFolderPath = std::string(static_cast<const char*>(payload->Data), payload->DataSize > 0 ? payload->DataSize - 1 : 0);
			std::shared_ptr<ProjectDirectory> directory = nullptr;
			if (ProjectManager::GetProjectDirectory())
			{
				directory = ProjectManager::FindProjectDirectory(*ProjectManager::GetProjectDirectory(), movedFolderPath);
			}
			if (directory)
			{
				ref = directory;
				returnValue = true;
			}
		}
		ImGui::EndDragDropTarget();
	}
	return returnValue;
}

bool EditorUI::DragDropTarget(const std::string& name, std::shared_ptr<Component>& ref, bool getOnMouseRelease)
{
	bool returnValue = false;
	if (ImGui::BeginDragDropTarget())
	{
		ImGuiDragDropFlags target_flags = 0;
		if (!getOnMouseRelease)
			target_flags |= ImGuiDragDropFlags_AcceptBeforeDelivery;
		std::shared_ptr<Component> comp = nullptr;
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(name.c_str(), target_flags))
		{
			// The payload is the component id
			uint64_t componentId = 0;
			memcpy(&componentId, payload->Data, sizeof(uint64_t));
			comp = FindComponentById(componentId);
		}
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MultiDragData", target_flags))
		{
			const size_t compCount = EditorUI::multiDragData.components.size();

			for (size_t i = 0; i < compCount; i++)
			{
				const std::shared_ptr<Component> draggedComponent = EditorUI::multiDragData.components[i].lock();
				if (!draggedComponent)
					continue;

				const uint64_t id = typeid(*draggedComponent).hash_code();
				if ("Type" + std::to_string(id) == name)
				{
					comp = draggedComponent;
					break;
				}
			}

		}

		if (comp)
		{
			ref = comp;
			returnValue = true;
		}

		ImGui::EndDragDropTarget();
	}
	return returnValue;
}

bool EditorUI::DragDropTarget(const std::string& name, std::shared_ptr<Collider>& ref, bool getOnMouseRelease)
{
	bool returnValue = false;
	if (ImGui::BeginDragDropTarget())
	{
		ImGuiDragDropFlags target_flags = 0;
		if (!getOnMouseRelease)
			target_flags |= ImGuiDragDropFlags_AcceptBeforeDelivery;
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(name.c_str(), target_flags))
		{
			// The payload is the component id
			uint64_t componentId = 0;
			memcpy(&componentId, payload->Data, sizeof(uint64_t));
			const std::shared_ptr<Collider> collider = std::dynamic_pointer_cast<Collider>(FindComponentById(componentId));
			if (collider)
			{
				ref = collider;
				returnValue = true;
			}
		}
		ImGui::EndDragDropTarget();
	}
	return returnValue;
}

bool EditorUI::DragDropTarget(const std::string& name, std::shared_ptr<GameObject>& ref, bool getOnMouseRelease)
{
	bool returnValue = false;
	if (ImGui::BeginDragDropTarget())
	{
		ImGuiDragDropFlags target_flags = 0;
		if (!getOnMouseRelease)
			target_flags |= ImGuiDragDropFlags_AcceptBeforeDelivery;
		std::shared_ptr<GameObject> gameObject = nullptr;
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(name.c_str(), target_flags))
		{
			// The payload is the GameObject id
			uint64_t gameObjectId = 0;
			memcpy(&gameObjectId, payload->Data, sizeof(uint64_t));
			gameObject = FindGameObjectById(gameObjectId);
		}
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MultiDragData", target_flags))
		{
			if (!EditorUI::multiDragData.gameObjects.empty())
				gameObject = EditorUI::multiDragData.gameObjects[0].lock();
		}

		if (gameObject)
		{
			ref = gameObject;
			returnValue = true;
		}

		ImGui::EndDragDropTarget();
	}
	return returnValue;
}

bool EditorUI::DragDropTarget(const std::string& name, std::shared_ptr<Transform>& ref, bool getOnMouseRelease)
{
	bool returnValue = false;
	if (ImGui::BeginDragDropTarget())
	{
		ImGuiDragDropFlags target_flags = 0;
		if (!getOnMouseRelease)
			target_flags |= ImGuiDragDropFlags_AcceptBeforeDelivery;
		std::shared_ptr<Transform> trans = nullptr;
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(name.c_str(), target_flags))
		{
			// The payload is the id of the GameObject of the transform
			uint64_t gameObjectId = 0;
			memcpy(&gameObjectId, payload->Data, sizeof(uint64_t));
			if (const std::shared_ptr<GameObject> gameObject = FindGameObjectById(gameObjectId))
				trans = gameObject->GetTransform();
		}
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MultiDragData", target_flags))
		{
			if (!EditorUI::multiDragData.transforms.empty())
				trans = EditorUI::multiDragData.transforms[0].lock();
		}

		if (trans)
		{
			ref = trans;
			returnValue = true;
		}

		ImGui::EndDragDropTarget();
	}
	return returnValue;
}

#endif // #if defined(EDITOR)