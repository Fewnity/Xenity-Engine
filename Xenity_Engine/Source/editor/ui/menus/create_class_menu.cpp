// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Grégory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "create_class_menu.h"

#include <imgui/imgui.h>
#include <editor/ui/editor_ui.h>
#include <engine/tools/string_tag_finder.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/file_system/file.h>

void CreateClassMenu::Init()
{
	Reset();
}

void CreateClassMenu::Draw()
{
	ImGui::SetNextWindowSize(ImVec2(370, 0), ImGuiCond_FirstUseEver);
	const bool visible = ImGui::Begin("Create C++ Class", 0, ImGuiWindowFlags_NoCollapse);
	if (visible)
	{
		OnStartDrawing();

		if (EditorUI::DrawInputTemplate("Class name", className) != ValueInputState::NO_CHANGE)
		{
			if (!fileNameChanged)
			{
				SetFileNameFromClassName();
			}
		}

		if (EditorUI::DrawInputTemplate("File name", fileName) != ValueInputState::NO_CHANGE)
		{
			fileNameChanged = true;
			const size_t fileNameSize = fileName.size();
			for (int i = 0; i < fileNameSize; i++)
			{
				fileName[i] = std::tolower(fileName[i]);
			}
		}

		// Draw created files list
		ImGui::Separator();
		ImGui::TextDisabled("Created files: ");
		ImGui::Text("%s.cpp", fileName.c_str());
		ImGui::Text("%s.h", fileName.c_str());
		ImGui::Separator();

		//TODO check if the class and file names are correct
		if (ImGui::Button("Create") && !className.empty() && !fileName.empty())
		{
			CreateFiles();
			Reset();
			isActive = false;
		}

		CalculateWindowValues();
	}
	else
	{
		ResetWindowValues();
	}

	ImGui::End();
}

void CreateClassMenu::Reset()
{
	className = "MyClass";
	fileName = "my_class";
}

void CreateClassMenu::SetFolderPath(const std::string& path)
{
	folderPath = path;
}

void CreateClassMenu::SetFileNameFromClassName()
{
	const size_t classNameSize = className.size();
	fileName.clear();
	for (int i = 0; i < classNameSize; i++)
	{
		if (i > 0 && std::tolower(className[i - 1]) == className[i - 1])
		{
			if (std::tolower(className[i]) != className[i])
			{
				fileName.push_back('_');
			}
		}
		fileName.push_back(std::tolower(className[i]));
	}
}

void CreateClassMenu::CreateFiles()
{
	std::shared_ptr<File> codeFile = Editor::CreateNewFile(folderPath + "\\" + fileName, FileType::File_Code, false);
	std::shared_ptr<File> headerFile = Editor::CreateNewFile(folderPath + "\\" + fileName, FileType::File_Header, false);

	// Get default cpp code text
	std::string codeData = AssetManager::GetDefaultFileData(FileType::File_Code);
	// Get default header text
	std::string headerData = AssetManager::GetDefaultFileData(FileType::File_Header);

	// Replace tag by class name
	size_t codeDataSize = codeData.size();
	size_t headerDataSize = headerData.size();
	int beg;
	int end;
	for (int i = 0; i < codeDataSize; i++)
	{
		if (StringTagFinder::FindTag(codeData, i, codeDataSize, "{CLASSNAME}", beg, end))
		{
			codeData.replace(beg, end - beg - 1, className);
			codeDataSize = codeData.size();
		}
		else if (StringTagFinder::FindTag(codeData, i, codeDataSize, "{FILENAME}", beg, end))
		{
			codeData.replace(beg, end - beg - 1, fileName);
			codeDataSize = codeData.size();
		}
	}
	for (size_t i = 0; i < headerDataSize; i++)
	{
		if (StringTagFinder::FindTag(headerData, i, headerDataSize, "{CLASSNAME}", beg, end))
		{
			headerData.replace(beg, end - beg - 1, className);
			headerDataSize = headerData.size();
		}
	}

	// Write data to files
	if (codeFile->Open(FileMode::WriteOnly))
	{
		codeFile->Write(codeData);
		codeFile->Close();
	}

	if (headerFile->Open(FileMode::WriteOnly))
	{
		headerFile->Write(headerData);
		headerFile->Close();
	}
}
