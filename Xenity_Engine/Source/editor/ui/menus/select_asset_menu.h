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

template <class T>
class SelectAssetMenu : public Menu
{
public:
	/*void Init() override;
	void Draw() override;*/
	void Init() override
	{
	}

	void SearchFiles(FileType type) 
	{
		std::vector<std::shared_ptr<File>> projectFiles = ProjectManager::projectDirectoryBase->GetAllFiles(true);
		int fileCount = (int)projectFiles.size();
		int allFoundFileCount = 0;
		for (int i = 0; i < fileCount; i++)
		{
			std::shared_ptr<File> file = projectFiles[i];
			FileType fileType = ProjectManager::GetFileType(file->GetFileExtension());

			if (fileType != File_Other && fileType == type)
			{
				allFoundFileCount++;
				std::shared_ptr<FileReference> fileRef = ProjectManager::GetFileReferenceById(file->GetUniqueId());
				fileRef->LoadFileReference();
				Debug::Print(fileRef->file->GetPath());
				foundFiles.push_back(fileRef);
			}
		}

		/*size_t folderCount = currentDir->subdirectories.size();
		size_t fileCount = currentDir->files.size();*/
	}

	void Draw() override
	{
		//if(isActive)
		ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_FirstUseEver);
		bool visible = ImGui::Begin("Select file##Selectfile", &isActive, ImGuiWindowFlags_NoCollapse);
		if (visible)
		{
			OnStartDrawing();
			
			ImGui::Text("List");
			int fileCount = foundFiles.size();
			for (int i = 0; i < fileCount; i++)
			{
				std::string txt = foundFiles[i]->file->GetPath();
				if (ImGui::Button(txt.c_str()))
				{
					valuePtr->get() = std::dynamic_pointer_cast<T>(foundFiles[i]);
				}
			}
			if (ImGui::Button("Clear"))
			{
				valuePtr->get().reset();
			}
			CalculateWindowValues();
		}
		else
		{
			ResetWindowValues();
		}

		ImGui::End();
	}

	std::optional<std::reference_wrapper<std::shared_ptr<T>>> valuePtr;
	std::vector<std::shared_ptr<FileReference>> foundFiles;
private:
};

