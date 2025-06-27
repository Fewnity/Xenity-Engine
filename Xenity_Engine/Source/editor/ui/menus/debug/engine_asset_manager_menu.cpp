// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "engine_asset_manager_menu.h"

#include <imgui/imgui.h>

#include <editor/ui/editor_ui.h>

#include <engine/file_system/file.h>
#include <engine/file_system/directory.h>
#include <engine/asset_management/project_manager.h>
#include <engine/file_system/file_system.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/constants.h>

#include <json.hpp>
using ordered_json = nlohmann::ordered_json;

void EngineAssetManagerMenu::Init()
{
	//engineAssetsFiles = ProjectManager::publicEngineAssetsDirectoryBase->GetAllFiles(true);
	/*const int engineAssetsFileCount = (int)engineAssetsFiles.size();
	for (int i = 0; i < engineAssetsFileCount; i++)
	{
		projectFiles.push_back(engineAssetsFiles[i]);
	}
	engineAssetsFiles.clear();*/
}

void EngineAssetManagerMenu::Draw()
{
	ImGui::SetNextWindowSize(ImVec2(600, 250), ImGuiCond_FirstUseEver);

	const bool visible = ImGui::Begin("Engine Assets Manager", &m_isActive, ImGuiWindowFlags_NoCollapse);
	if (visible)
	{
		OnStartDrawing();
		if (ImGui::Button("Refresh list")) 
		{
			ProjectManager::RefreshProjectDirectory();
			m_engineAssetsFiles = ProjectManager::s_publicEngineAssetsDirectoryBase->GetAllFiles(true);

			m_ids.clear();
			size_t fileCount = m_engineAssetsFiles.size();
			for (size_t i = 0; i < fileCount; i++)
			{
				std::shared_ptr<File> file = m_engineAssetsFiles[i];
				if (file->GetPath().substr(file->GetPath().size() - 5) == ".meta") 
				{
					m_engineAssetsFiles.erase(m_engineAssetsFiles.begin() + i);
					i--;
					fileCount--;
					continue;
				}

				const std::string metaFilePath = file->GetPath() + META_EXTENSION;
				std::shared_ptr<File> metaFile = FileSystem::MakeFile(metaFilePath);

				if (metaFile->Open(FileMode::ReadOnly))
				{
					const std::string jsonString = metaFile->ReadAll();
					metaFile->Close();
					if (!jsonString.empty())
					{
						ordered_json data;
						try
						{
							data = ordered_json::parse(jsonString);
						}
						catch (const std::exception&)
						{
							m_ids.push_back(0);
						}
						m_ids.push_back(data["id"]);
					}
				}
			}
			m_oldIds = m_ids;
		}
		const size_t idCount = m_ids.size();
		ImGui::SameLine();
		if (ImGui::Button("Save"))
		{
			for (size_t i = 0; i < idCount; i++)
			{
				std::shared_ptr<FileReference> fileRef = ProjectManager::GetFileReferenceById(m_oldIds[i]);
				fileRef->m_fileId = m_ids[i];
				fileRef->m_isMetaDirty = true;
				ProjectManager::SaveMetaFile(*fileRef);
			}
			m_oldIds = m_ids;
		}
		/*if (ImGui::Button("Fix ids"))
		{
			int fileRefCount = AssetManager::GetFileReferenceCount();
			for (int i = 0; i < fileRefCount; i++)
			{
				AssetManager::GetFileReference(i)->fileId += UniqueId::reservedFileId;
				AssetManager::GetFileReference(i)->isMetaDirty = true;
			}

			for (int i = 0; i < fileRefCount; i++)
			{
				ProjectManager::SaveMetaFile(AssetManager::GetFileReference(i));
			}
		}*/
		ImGui::Separator();

		ImGui::Text("Engine Assets list");
		ImGui::Text("Only change asset ids when id is >= %lld", UniqueId::reservedFileId);
		if (ImGui::BeginTable("meta_file_table", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_BordersOuterV | ImGuiTableFlags_Resizable))
		{
			for (int i = 0; i < idCount; i++)
			{
				ImGui::TableNextRow(0, 0);
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%s", m_engineAssetsFiles[i]->GetFileName().c_str());
				ImGui::TableSetColumnIndex(1);
				EditorUI::DrawInputTemplate("Id", m_ids[i]);
			}
			ImGui::EndTable();
		}

		CalculateWindowValues();
	}
	else
	{
		ResetWindowValues();
	}

	ImGui::End();
}
