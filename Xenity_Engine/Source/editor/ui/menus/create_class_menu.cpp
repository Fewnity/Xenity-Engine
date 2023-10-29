#include "create_class_menu.h"
#include <imgui/imgui.h>
#include "../../../xenity.h"
#include "../../../xenity_editor.h"
#include "../../../engine/tools/string_tag_finder.h"

void CreateClassMenu::Init()
{
	Reset();
}

void CreateClassMenu::Draw()
{
	ImGui::Begin("Create C++ Class", &EditorUI::showCreateClass, ImGuiWindowFlags_NoCollapse);

	if (EditorUI::DrawInput("Class name", className))
	{
		if (!fileNameChanged)
		{
			int classNameSize = className.size();
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
	}

	if (EditorUI::DrawInput("File name", fileName))
	{
		if (!fileNameChanged)
		{
			fileNameChanged = true;
		}
		int fileNameSize = fileName.size();
		for (int i = 0; i < fileNameSize; i++)
		{
			fileName[i] = std::tolower(fileName[i]);
		}
	}

	ImGui::Text("%s.cpp", fileName.c_str());
	ImGui::Text("%s.h", fileName.c_str());

	if (ImGui::Button("Create") && !className.empty() && !fileName.empty())
	{
		std::shared_ptr<File> codeFile = Editor::CreateNewFile(filePath + "\\" + fileName, FileType::File_Code, false);
		std::shared_ptr<File> headerFile = Editor::CreateNewFile(filePath + "\\" + fileName, FileType::File_Header, false);

		std::string codeData = AssetManager::GetDefaultFileData(FileType::File_Code);

		int codeDataSize = codeData.size();
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

		std::string headerData = AssetManager::GetDefaultFileData(FileType::File_Header);
		int headerDataSize = headerData.size();
		for (int i = 0; i < headerDataSize; i++)
		{

			if (StringTagFinder::FindTag(headerData, i, headerDataSize, "{CLASSNAME}", beg, end))
			{
				headerData.replace(beg, end - beg - 1, className);
				headerDataSize = headerData.size();
			}
		}

		if (codeFile->Open(false))
		{
			codeFile->Write(codeData);
			codeFile->Close();
		}
		if (headerFile->Open(false))
		{
			headerFile->Write(headerData);
			headerFile->Close();
		}

		Reset();
		EditorUI::showCreateClass = false;
	}
	ImGui::End();
}

void CreateClassMenu::Reset()
{
	className = "MyClass";
	fileName = "my_class";
}
