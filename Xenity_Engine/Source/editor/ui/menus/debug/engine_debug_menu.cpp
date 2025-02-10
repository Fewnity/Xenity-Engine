// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "engine_debug_menu.h"

#include <imgui/imgui.h>

#include <editor/ui/editor_ui.h>

#include <engine/asset_management/asset_manager.h>
#include <engine/file_system/file.h>
#include <engine/file_system/file_reference.h>

void EngineDebugMenu::Init()
{
}

void EngineDebugMenu::Draw()
{
	ImGui::SetNextWindowSize(ImVec2(400, 0), ImGuiCond_FirstUseEver);
	const bool visible = ImGui::Begin("Engine Debug", &isActive, ImGuiWindowFlags_NoCollapse);
	if (visible)
	{
		OnStartDrawing();

		DrawFilesList();

		CalculateWindowValues();
	}
	else
	{
		ResetWindowValues();
	}

	ImGui::End();
}

void EngineDebugMenu::DrawFilesList()
{
	if (ImGui::CollapsingHeader("Files", ImGuiTreeNodeFlags_Framed))
	{
		for (int i = 0; i < AssetManager::GetFileReferenceCount(); i++)
		{
			const std::shared_ptr<FileReference>& fileRef = AssetManager::GetFileReference(i);
			if (fileRef && fileRef->GetFileStatus() != FileStatus::FileStatus_Loaded)
			{
				ImGui::SetCursorPosX(20);
				if (fileRef->m_file)
				{
					ImGui::Text("File%lld isLoaded:%d useCount:%ld :", fileRef->m_fileId, fileRef->GetFileStatus() == FileStatus::FileStatus_Loaded, fileRef.use_count());
					ImGui::SameLine();
					ImGui::Text("%s%s", fileRef->m_file->GetFileName().c_str(), fileRef->m_file->GetFileExtension().c_str());
				}
				else
				{
					ImGui::Text("Missing file isLoaded:%d type:%d useCount:%ld", fileRef->GetFileStatus() == FileStatus::FileStatus_Loaded, static_cast<int>(fileRef->GetFileType()), fileRef.use_count());
				}
			}
		}
		ImGui::Text("-----------------------------------------------------");
		for (int i = 0; i < AssetManager::GetFileReferenceCount(); i++)
		{
			const std::shared_ptr<FileReference>& fileRef = AssetManager::GetFileReference(i);
			if (fileRef && fileRef->GetFileStatus() == FileStatus::FileStatus_Loaded)
			{
				ImGui::SetCursorPosX(20);
				if (fileRef->m_file)
				{
					ImGui::Text("File%lld isLoaded:%d useCount:%ld :", fileRef->m_fileId, fileRef->GetFileStatus() == FileStatus::FileStatus_Loaded, fileRef.use_count());
					ImGui::SameLine();
					ImGui::Text("%s%s", fileRef->m_file->GetFileName().c_str(), fileRef->m_file->GetFileExtension().c_str());
				}
				else
				{
					ImGui::Text("missing file isLoaded:%d useCount:%ld", fileRef->GetFileStatus() == FileStatus::FileStatus_Loaded, fileRef.use_count());
				}
			}
		}
	}
}
