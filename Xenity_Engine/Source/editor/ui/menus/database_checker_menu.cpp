// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "database_checker_menu.h"

#include <imgui/imgui.h>

#include <editor/ui/editor_ui.h>

#include <engine/file_system/file.h>
#include <engine/file_system/directory.h>
#include <engine/asset_management/project_manager.h>
#include <engine/file_system/file_system.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/constants.h>

#include <json.hpp>
using json = nlohmann::json;

DataBaseCheckerMenu::DataBaseCheckerMenu()
{
}

void DataBaseCheckerMenu::Init()
{

}

void DataBaseCheckerMenu::Draw()
{
	ImGui::SetNextWindowSize(ImVec2(600, 250), ImGuiCond_FirstUseEver);

	const bool visible = ImGui::Begin("Database Checker", &isActive, ImGuiWindowFlags_NoCollapse);
	if (visible)
	{
		OnStartDrawing();

		if (ImGui::Button("Load"))
		{
			std::string path = EditorUI::OpenFileDialog("Load db.bin", "");
			if (!path.empty())
			{
				wrongDbLoaded = false;
				loaded = false;
				db = std::make_unique<FileDataBase>();
				try
				{
					db->LoadFromFile(path);
					integrityState = db->CheckIntegrity();
					loaded = true;
				}
				catch (const std::exception&)
				{
					wrongDbLoaded = true;
				}
			}
		}
		ImGui::Separator();

		if (wrongDbLoaded)
		{
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "Selected file is not a database");
		}

		if (loaded)
		{
			ImGui::Text("Integrity State:");
			if (integrityState == IntegrityState::Integrity_Ok)
			{
				ImGui::TextColored(ImVec4(0, 1, 0, 1), "Ok");
			}
			else
			{
				if (static_cast<int>(integrityState) & static_cast<int>(IntegrityState::Integrity_Error_Non_Unique_Ids))
				{
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "Non unique ids found");
				}
				if (static_cast<int>(integrityState) & static_cast<int>(IntegrityState::Integrity_Has_Wrong_Type_Files))
				{
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "File with wrong type found");
				}
				if (static_cast<int>(integrityState) & static_cast<int>(IntegrityState::Integrity_Has_Empty_Path))
				{
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "File with an empty path found");
				}
				if (static_cast<int>(integrityState) & static_cast<int>(IntegrityState::Integrity_Wrong_File_Position))
				{
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "File with wrong position found");
				}
				if (static_cast<int>(integrityState) & static_cast<int>(IntegrityState::Integrity_Wrong_File_Size))
				{
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "File with wrong size found");
				}
				if (static_cast<int>(integrityState) & static_cast<int>(IntegrityState::Integrity_Wrong_File_Position))
				{
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "Meta file with wrong size found");
				}
				if (static_cast<int>(integrityState) & static_cast<int>(IntegrityState::Integrity_Wrong_Meta_File_Size))
				{
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "Meta file with wrong position found");
				}
			}

			ImGui::Text("Entry list");
			if (db)
			{
				if (ImGui::BeginTable("meta_file_table", 7, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_BordersOuterV | ImGuiTableFlags_Resizable))
				{
					ImGui::TableSetupColumn("Path", ImGuiTableColumnFlags_WidthStretch);
					ImGui::TableSetupColumn("Id", ImGuiTableColumnFlags_WidthStretch);
					ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthStretch);
					ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthStretch);
					ImGui::TableSetupColumn("Position", ImGuiTableColumnFlags_WidthStretch);
					ImGui::TableSetupColumn("Meta Size", ImGuiTableColumnFlags_WidthStretch);
					ImGui::TableSetupColumn("Meta Position", ImGuiTableColumnFlags_WidthStretch);
					ImGui::TableSetupScrollFreeze(0, 1);
					ImGui::TableHeadersRow();

					std::vector<FileDataBaseEntry*> entries = db->GetFileList();
					for (auto entry : entries)
					{
						ImGui::TableNextRow(0, 0);
						ImGui::TableSetColumnIndex(0);
						ImGui::Text("%s", entry->p.c_str());
						ImGui::TableSetColumnIndex(1);
						const std::string enumName = EnumHelper::EnumAsString(entry->t);
						ImGui::Text("%s", enumName.c_str());
						ImGui::TableSetColumnIndex(2);
						ImGui::Text("%lld", entry->id);
						ImGui::TableSetColumnIndex(3);
						ImGui::Text("%lld", entry->s);
						ImGui::TableSetColumnIndex(4);
						ImGui::Text("%lld", entry->po);
						ImGui::TableSetColumnIndex(5);
						ImGui::Text("%lld", entry->ms);
						ImGui::TableSetColumnIndex(6);
						ImGui::Text("%lld", entry->mpo);
					}

					ImGui::EndTable();
				}
			}
		}

		CalculateWindowValues();
	}
	else
	{
		ResetWindowValues();
	}

	ImGui::End();
}
