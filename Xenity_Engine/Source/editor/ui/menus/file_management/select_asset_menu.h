// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <memory>
#include <vector>
#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>

#include <editor/ui/menus/file_management/file_explorer_menu.h>
#include <editor/ui/menus/basic/inspector_menu.h>
#include <editor/editor.h>
#include <editor/command/command_manager.h>
#include <editor/command/commands/modify.h>
#include <editor/ui/menus/menu.h>

#include <engine/asset_management/project_manager.h>
#include <engine/file_system/file.h>
#include <engine/graphics/texture/texture.h>
#include <engine/event_system/event_system.h>
#include <engine/tools/string_utils.h>

template <class T>
class SelectAssetMenu : public Menu
{
public:

	void Init() override
	{
	}

	void OnOpen() override
	{
		searchBuffer = "";
	}

	void DrawItem(const std::string& itemName, int& currentCol, int colCount, float offset, const Texture& icon, float iconSize, size_t index, bool isSelected)
	{
		if (currentCol == 0)
			ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(currentCol);

		currentCol++;
		currentCol %= colCount;
		if(isSelected)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.2f, 0.3f, 1.0f));
		else
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.1f, 0.2f, 0.3f, 0.5f));
		ImGui::BeginGroup();
		const int cursorPos = (int)ImGui::GetCursorPosX();
		const int availWidth = (int)ImGui::GetContentRegionAvail().x;
		ImGui::SetCursorPosX(cursorPos + (availWidth - iconSize) / 2.0f - offset / 2.0f);
		ImGui::ImageButton(std::string("SelectAssetMenuItem" + std::to_string(index)).c_str(), (ImTextureID)(size_t)EditorUI::GetTextureId(icon), ImVec2(iconSize, iconSize));

		const float windowWidth = ImGui::GetContentRegionAvail().x;
		const float textWidth = ImGui::CalcTextSize(itemName.c_str()).x;
		if (textWidth <= availWidth)
		{
			ImGui::SetCursorPosX(cursorPos + (windowWidth - textWidth) * 0.5f);
			ImGui::Text("%s", itemName.c_str());
		}
		else
		{
			ImGui::TextWrapped("%s", itemName.c_str());
		}
		ImGui::EndGroup();
		ImGui::PopStyleColor(3);
	}

	void SearchFiles(FileType type) 
	{
		foundFiles.clear();
		const std::vector<FileInfo> projectFiles = ProjectManager::GetFilesByType(type);
		const size_t fileCount = projectFiles.size();

		// Load all files and check if they match the search buffer
		for (size_t i = 0; i < fileCount; i++)
		{
			const std::shared_ptr<FileReference> fileRef = ProjectManager::GetFileReferenceById(projectFiles[i].fileAndId.id);
			// Filter files by user search
			if (!searchBuffer.empty())
			{
				std::string lowerFileName = StringUtils::ToLower(fileRef->m_file->GetFileName());
				if (lowerFileName.find(searchBuffer) == std::string::npos)
				{
					continue;
				}
			}
			// Do not show user assets if showEngineAssetOnly is true
			if (showEngineAssetOnly && fileRef->GetFileId() > UniqueId::reservedFileId)
			{
				continue;
			}
			// Do not show hidden files
			if (fileRef->GetFileId() == -1)
			{
				continue;
			}
			FileReference::LoadOptions loadOptions;
			loadOptions.platform = Application::GetPlatform();
			loadOptions.threaded = false;
			fileRef->LoadFileReference(loadOptions);

			foundFiles.push_back(fileRef);
		}

		// Sort files by name
		std::sort(foundFiles.begin(), foundFiles.end(),
			[](const std::shared_ptr<FileReference>& a, const std::shared_ptr<FileReference>& b)
			{
				std::string fileA = a->m_file->GetFileName() + a->m_file->GetFileExtension();
				std::string fileB = b->m_file->GetFileName() + b->m_file->GetFileExtension();

				// Convert both strings to lowercase for case-insensitive comparison
				std::transform(fileA.begin(), fileA.end(), fileA.begin(), [](unsigned char c) { return std::tolower(c); });
				std::transform(fileB.begin(), fileB.end(), fileB.begin(), [](unsigned char c) { return std::tolower(c); });

				return fileA < fileB;
			});

		fileType = type;
	}

	void Draw() override
	{
		ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_FirstUseEver);
		bool isOpen = true;
		const bool visible = ImGui::Begin("Select file##Selectfile", &isOpen, ImGuiWindowFlags_NoCollapse);
		if (visible)
		{
			OnStartDrawing();
			const ImVec2 startCusorPos = ImGui::GetCursorPos();
			ImGui::SetCursorPosY(startCusorPos.y * 2);

			const float width = ImGui::GetContentRegionAvail().x;
			int colCount = (int)(width / (100 * 1)); // Replace 1 by EditorUI::GetUIScale()
			if (colCount <= 0)
				colCount = 1;
			const float offset = ImGui::GetCursorPosX();

			ImGui::BeginChild("SelectAssetContent");
			if (ImGui::BeginTable("selectfiletable", colCount, ImGuiTableFlags_None))
			{
				const size_t fileCount = foundFiles.size();
				int currentCol = 0;
				for (size_t i = 0; i < fileCount; i++)
				{
					FileExplorerItem item;
					item.file = foundFiles[i];
					bool isSelected = valuePtr->get() == std::dynamic_pointer_cast<T>(item.file);
					DrawItem(item.file->m_file->GetFileName(), currentCol, colCount, offset, *FileExplorerMenu::GetItemIcon(item), 64, i, isSelected);

					if (ImGui::IsItemClicked())
					{
						if (hasReflectiveDataToDraw) 
						{
							std::shared_ptr<T> newValue = std::dynamic_pointer_cast<T>(item.file);
							auto command = std::make_shared<ReflectiveChangeValueCommand<std::shared_ptr<T>>>(reflectiveDataToDraw, &valuePtr->get(), valuePtr->get(), newValue);
							CommandManager::AddCommandAndExecute(command);
						}
						else 
						{
							valuePtr->get() = std::dynamic_pointer_cast<T>(item.file);
						}

						if (onValueChangedEvent) 
						{
							onValueChangedEvent->Trigger();
						}
						const std::vector<std::shared_ptr<InspectorMenu>> inspectors = Editor::GetMenus<InspectorMenu>();
						const size_t inspectorsCount = inspectors.size();
						for (size_t menuIndex = 0; menuIndex < inspectorsCount; menuIndex++)
						{
							inspectors[menuIndex]->forceItemUpdate = true;
						}
					}
					if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered())
					{
						isActive = false;
					}
				}
			}
			ImGui::EndTable();
			ImGui::EndChild();

			ImGui::SetCursorPos(startCusorPos);
			ImGui::BeginChild("SelectAssetTopBar", ImVec2(0, 0), ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY);
			// Draw search bar
			ImGui::Text("Search");
			ImGui::SameLine();
			const bool scearchBarChanged = ImGui::InputText("##SearchBar", &searchBuffer);
			if (scearchBarChanged)
			{
				searchBuffer = StringUtils::ToLower(searchBuffer);
				SearchFiles(fileType);
			}
			ImGui::SameLine();
			const bool showEngineAssetOnlyChanged = ImGui::Checkbox("showEngineAssetOnly", &showEngineAssetOnly);
			if (showEngineAssetOnlyChanged)
			{
				SearchFiles(fileType);
			}
			ImGui::EndChild();

			CalculateWindowValues();
		}
		else
		{
			ResetWindowValues();
		}

		ImGui::End();
		if (!isOpen)
		{
			Editor::RemoveMenu(this);
		}
	}

	std::optional<std::reference_wrapper<std::shared_ptr<T>>> valuePtr;
	Event<>* onValueChangedEvent = nullptr;
	ReflectiveDataToDraw reflectiveDataToDraw;
	bool hasReflectiveDataToDraw = false;
	bool showEngineAssetOnly = false;
private:
	std::vector<std::shared_ptr<FileReference>> foundFiles;
	FileType fileType = FileType::File_Other;
	std::string searchBuffer = "";
};

