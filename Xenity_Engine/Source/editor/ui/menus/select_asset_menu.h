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
#include <editor/editor.h>

template <class T>
class SelectAssetMenu : public Menu
{
public:

	void Init() override
	{
	}

	void DrawItem(const std::string& itemName, int& currentCol, int colCount,int offset, std::shared_ptr<Texture> icon, int iconSize, int index)
	{
		if (currentCol == 0)
			ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(currentCol);

		currentCol++;
		currentCol %= colCount;

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.1f, 0.2f, 0.3f, 0.5f));
		ImGui::BeginGroup();
		const int cursorPos = (int)ImGui::GetCursorPosX();
		const int availWidth = (int)ImGui::GetContentRegionAvail().x;
		ImGui::SetCursorPosX(cursorPos + (availWidth - iconSize) / 2.0f - offset / 2.0f);
		ImGui::ImageButton(std::string("SelectAssetMenuItem" + std::to_string(index)).c_str(), (ImTextureID)icon->GetTextureId(), ImVec2(iconSize, iconSize));

		const float windowWidth = ImGui::GetContentRegionAvail().x;
		const float textWidth = ImGui::CalcTextSize(itemName.c_str()).x;
		if (textWidth <= availWidth)
		{
			ImGui::SetCursorPosX(cursorPos + (windowWidth - textWidth) * 0.5f);
			ImGui::Text(itemName.c_str());
		}
		else
		{
			ImGui::TextWrapped(itemName.c_str());
		}
		ImGui::EndGroup();
		ImGui::PopStyleColor(3);
	}

	void SearchFiles(FileType type) 
	{
		const std::vector<std::shared_ptr<File>> projectFiles = ProjectManager::projectDirectoryBase->GetAllFiles(true);
		const int fileCount = (int)projectFiles.size();
		for (int i = 0; i < fileCount; i++)
		{
			const std::shared_ptr<File> file = projectFiles[i];
			FileType fileType = ProjectManager::GetFileType(file->GetFileExtension());

			if (fileType != File_Other && fileType == type)
			{
				std::shared_ptr<FileReference> fileRef = ProjectManager::GetFileReferenceById(file->GetUniqueId());
				fileRef->LoadFileReference();
				foundFiles.push_back(fileRef);
			}
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
			int colCount = width / (100 * 1); // Replace 1 by EditorUI::GetUIScale()
			if (colCount <= 0)
				colCount = 1;
			int currentCol = 0;
			const float offset = ImGui::GetCursorPosX();
			if (ImGui::BeginTable("selectfiletable", colCount, ImGuiTableFlags_None))
			{
				const int fileCount = foundFiles.size();
				for (int i = 0; i < fileCount; i++)
				{
					FileExplorerItem item;
					item.file = foundFiles[i];
					DrawItem(foundFiles[i]->file->GetFileName(), currentCol, colCount, offset, FileExplorerMenu::GetItemIcon(item, true), 64, i);
					if (ImGui::IsItemClicked())
					{
						valuePtr->get() = std::dynamic_pointer_cast<T>(foundFiles[i]);
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
	std::vector<std::shared_ptr<FileReference>> foundFiles;
private:
};

