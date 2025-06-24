// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "create_class_menu.h"

#include <imgui/imgui.h>

#include <editor/ui/editor_ui.h>

#include <engine/tools/string_utils.h>
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

		if (EditorUI::DrawInputTemplate("Class name", m_className) != ValueInputState::NO_CHANGE)
		{
			if (!m_fileNameChanged)
			{
				SetFileNameFromClassName();
			}
		}

		if (EditorUI::DrawInputTemplate("File name", m_fileName) != ValueInputState::NO_CHANGE)
		{
			m_fileNameChanged = true;
			const size_t fileNameSize = m_fileName.size();
			for (int i = 0; i < fileNameSize; i++)
			{
				m_fileName[i] = std::tolower(m_fileName[i]);
			}
		}

		// Draw created files list
		ImGui::Separator();
		ImGui::TextDisabled("Created files: ");
		ImGui::Text("%s.cpp", m_fileName.c_str());
		ImGui::Text("%s.h", m_fileName.c_str());
		ImGui::Separator();

		//TODO check if the class and file names are correct
		if (ImGui::Button("Create") && !m_className.empty() && !m_fileName.empty())
		{
			CreateFiles();
			Reset();
			m_isActive = false;
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
	m_className = "MyClass";
	m_fileName = "my_class";
}

void CreateClassMenu::SetFolderPath(const std::string& path)
{
	m_folderPath = path;
}

void CreateClassMenu::SetFileNameFromClassName()
{
	const size_t classNameSize = m_className.size();
	m_fileName.clear();
	for (int i = 0; i < classNameSize; i++)
	{
		if (i > 0 && std::tolower(m_className[i - 1]) == m_className[i - 1])
		{
			if (std::tolower(m_className[i]) != m_className[i])
			{
				m_fileName.push_back('_');
			}
		}
		m_fileName.push_back(std::tolower(m_className[i]));
	}
}

void CreateClassMenu::CreateFiles()
{
	std::shared_ptr<File> codeFile = Editor::CreateNewFile(m_folderPath + "\\" + m_fileName, FileType::File_Code, false);
	std::shared_ptr<File> headerFile = Editor::CreateNewFile(m_folderPath + "\\" + m_fileName, FileType::File_Header, false);

	// Get default cpp code text
	std::string codeData = AssetManager::GetDefaultFileData(FileType::File_Code);
	// Get default header text
	std::string headerData = AssetManager::GetDefaultFileData(FileType::File_Header);

	// Replace tag by class name
	size_t codeDataSize = codeData.size();
	size_t headerDataSize = headerData.size();
	size_t beg;
	size_t end;
	for (int i = 0; i < codeDataSize; i++)
	{
		if (StringUtils::FindTag(codeData, i, codeDataSize, "{CLASSNAME}", beg, end))
		{
			codeData.replace(beg, end - beg - 1, m_className);
			codeDataSize = codeData.size();
		}
		else if (StringUtils::FindTag(codeData, i, codeDataSize, "{FILENAME}", beg, end))
		{
			codeData.replace(beg, end - beg - 1, m_fileName);
			codeDataSize = codeData.size();
		}
	}
	for (size_t i = 0; i < headerDataSize; i++)
	{
		if (StringUtils::FindTag(headerData, i, headerDataSize, "{CLASSNAME}", beg, end))
		{
			headerData.replace(beg, end - beg - 1, m_className);
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
