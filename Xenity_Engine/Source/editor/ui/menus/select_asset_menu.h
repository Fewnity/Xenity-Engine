// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <memory>
#include <vector>
#include <functional>
#include "menu.h"
#include <imgui/imgui.h>
#include <engine/asset_management/project_manager.h>
#include <engine/debug/debug.h>
#include <engine/file_system/file.h>
#include <engine/file_system/directory.h>
#include <engine/graphics/texture.h>
#include <editor/ui/menus/file_explorer_menu.h>
#include <editor/ui/menus/inspector_menu.h>
#include <editor/editor.h>
#include <engine/event_system/event_system.h>
#include <editor/command/command_manager.h>
#include <editor/command/commands/modify.h>

template <class T>
class SelectAssetMenu : public Menu
{
public:

	void Init() override
	{
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
		ImGui::ImageButton(std::string("SelectAssetMenuItem" + std::to_string(index)).c_str(), (ImTextureID)(size_t)icon.GetTextureId(), ImVec2(iconSize, iconSize));

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
		const std::vector<FileInfo> projectFiles = ProjectManager::GetFilesByType(type);
		const int fileCount = (int)projectFiles.size();
		for (int i = 0; i < fileCount; i++)
		{
			std::shared_ptr<FileReference> fileRef = ProjectManager::GetFileReferenceById(projectFiles[i].file->GetUniqueId());
			fileRef->LoadFileReference();
			foundFiles.push_back(fileRef);
		}
	}

	void Draw() override
	{
		ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_FirstUseEver);
		bool isOpen = true;
		const bool visible = ImGui::Begin("Select file##Selectfile", &isOpen, ImGuiWindowFlags_NoCollapse);
		if (visible)
		{
			OnStartDrawing();

			const float width = ImGui::GetContentRegionAvail().x;
			int colCount = (int)(width / (100 * 1)); // Replace 1 by EditorUI::GetUIScale()
			if (colCount <= 0)
				colCount = 1;
			const float offset = ImGui::GetCursorPosX();
			if (ImGui::BeginTable("selectfiletable", colCount, ImGuiTableFlags_None))
			{
				const size_t fileCount = foundFiles.size();
				int currentCol = 0;
				for (size_t i = 0; i < fileCount; i++)
				{
					FileExplorerItem item;
					item.file = foundFiles[i];
					bool isSelected = valuePtr->get() == std::dynamic_pointer_cast<T>(foundFiles[i]);
					DrawItem(foundFiles[i]->file->GetFileName(), currentCol, colCount, offset, *FileExplorerMenu::GetItemIcon(item), 64, i, isSelected);

					if (ImGui::IsItemClicked())
					{
						if (hasReflectiveDataToDraw) 
						{
							std::shared_ptr<T> newValue = std::dynamic_pointer_cast<T>(foundFiles[i]);
							auto command = std::make_shared<ReflectiveChangeValueCommand<std::shared_ptr<T>>>(reflectiveDataToDraw, &valuePtr->get(), valuePtr->get(), newValue);
							CommandManager::AddCommandAndExecute(command);
						}
						else 
						{
							valuePtr->get() = std::dynamic_pointer_cast<T>(foundFiles[i]);
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
private:
	std::vector<std::shared_ptr<FileReference>> foundFiles;
};

