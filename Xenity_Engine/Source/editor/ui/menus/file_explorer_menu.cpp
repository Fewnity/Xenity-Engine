#include "file_explorer_menu.h"
#include <imgui/imgui.h>
#include "../../../xenity.h"
#include "../../editor.h"
#include "../editor_ui.h"
#include "../../../engine/scene_management/scene_manager.h"
#include "../../../engine/asset_management/project_manager.h"
#include "../../../engine/asset_management/code_file.h"

void FileExplorerMenu::Init()
{
}

void FileExplorerMenu::OpenItem(FileExplorerItem& item)
{
	if (item.file)
	{
		if (item.file->fileType == File_Scene)
		{
			SceneManager::LoadScene(std::dynamic_pointer_cast<Scene>(item.file));
		}
		else if (item.file->fileType == File_Code)
		{
			std::string command = "code \"" + item.file->file->GetPath() + "\"";
			system(command.c_str());
		}
	}
	else if(item.directory)
	{
		Engine::SetCurrentProjectDirectory(item.directory);
		Engine::SetSelectedFileReference(nullptr);
	}
}

void FileExplorerMenu::DrawExplorerItem(float iconSize, int& currentCol, int colCount, bool isFile, int offset, FileExplorerItem item, std::string itemName)
{
	if (currentCol == 0)
		ImGui::TableNextRow();
	ImGui::TableSetColumnIndex(currentCol);

	currentCol++;
	currentCol %= colCount;

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.1f, 0.2f, 0.3f, 0.5f));

	ImGui::BeginGroup();
	int cursorPos = ImGui::GetCursorPosX();
	int availWidth = ImGui::GetContentRegionAvail().x;
	ImGui::SetCursorPosX(cursorPos + (availWidth - iconSize) / 2.0f - offset / 2.0f);

	unsigned int textureId = EditorUI::icons[Icon_File]->GetTextureId();
	if (!isFile)
	{
		textureId = EditorUI::icons[Icon_Folder]->GetTextureId();
	}
	else
	{
		int fileType = item.file->fileType;
		if (fileType == File_Texture)
		{
			std::shared_ptr<Texture> tex = std::dynamic_pointer_cast<Texture>(item.file);
			textureId = tex->GetTextureId();
			if (textureId == 0) 
			{
				textureId = EditorUI::icons[Icon_Image]->GetTextureId();
			}
		}
		else if (fileType == File_Scene)
		{
			textureId = EditorUI::icons[Icon_Scene]->GetTextureId();
		}
		else if (fileType == File_Code)
		{
			if(std::dynamic_pointer_cast<CodeFile>(item.file)->isHeader)
				textureId = EditorUI::icons[Icon_Header]->GetTextureId();
			else
				textureId = EditorUI::icons[Icon_Code]->GetTextureId();
		}
		else if (fileType == File_Mesh)
		{
			textureId = EditorUI::icons[Icon_Mesh]->GetTextureId();
		}
		else if (fileType == File_Audio)
		{
			textureId = EditorUI::icons[Icon_Audio]->GetTextureId();
		}
	}
	bool doubleClicked = ImGui::IsMouseDoubleClicked(0);
	ImGui::ImageButton(EditorUI::GenerateItemId().c_str(), (ImTextureID)textureId, ImVec2(iconSize, iconSize), ImVec2(0.005f, 0.005f), ImVec2(0.995f, 0.995f));
	std::string popupId = "RightClick";
	if (item.file) 
	{
		popupId += std::to_string(item.file->fileId);
	}
	else 
	{
		popupId += item.directory->GetFolderName();
	}
	CheckOpenRightClickPopupFile(item, true, popupId);
	bool hovered = ImGui::IsItemHovered();
	if (hovered)
	{
		fileHovered = true;
	}
	if (hovered && ((ImGui::IsMouseClicked(0) && doubleClicked) || (ImGui::IsMouseReleased(0) || ImGui::IsMouseReleased(1))))
	{
		if (doubleClicked)
		{
			OpenItem(item);
		}
		else
		{
			if (isFile)
			{
				Engine::SetSelectedFileReference(item.file);
			}

			Engine::selectedGameObject.reset();
		}
	}

	float windowWidth = ImGui::GetContentRegionAvail().x;
	float textWidth = ImGui::CalcTextSize(itemName.c_str()).x;
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

	if (isFile)
	{
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			std::string payloadName = "Files" + std::to_string(item.file->fileType);
			ImGui::SetDragDropPayload(payloadName.c_str(), item.file.get(), sizeof(FileReference));
			ImGui::ImageButton(EditorUI::GenerateItemId().c_str(), (ImTextureID)textureId, ImVec2(iconSize, iconSize));
			ImGui::TextWrapped(itemName.c_str());
			ImGui::EndDragDropSource();
		}
	}
	ImGui::PopStyleColor(3);
}

int FileExplorerMenu::CheckOpenRightClickPopupFile(FileExplorerItem& fileExplorerItem, bool itemSelected, std::string id)
{
	int state = 0;
	if (ImGui::IsMouseReleased(ImGuiMouseButton_Right) && ImGui::IsItemHovered())
	{
		ImGui::OpenPopup(id.c_str());
		state = 1;
	}

	if (ImGui::BeginPopup(id.c_str()))
	{
		if (state == 0)
			state = 2;
		fileHovered = true;

		if (ImGui::Button("Create folder"))
		{
			FileSystem::fileSystem->CreateDirectory(fileExplorerItem.directory->path + "\\new Folder");
			ImGui::CloseCurrentPopup();
		}
		if (itemSelected && ImGui::Button("Open"))
		{
			OpenItem(fileExplorerItem);
			ImGui::CloseCurrentPopup();
		}
		if ((itemSelected && ImGui::Button("Show in Explorer")) || (!itemSelected && ImGui::Button("Open folder in Explorer")))
		{
			std::string command = "explorer.exe ";
			if (itemSelected)
			{
				command += "/select, \"";
			}
			else 
			{
				command += "\"";
			}
			if (fileExplorerItem.file)
				command += fileExplorerItem.file->file->GetPath();
			else
				command += fileExplorerItem.directory->path;
			command += "\"";

			system(command.c_str());

			ImGui::CloseCurrentPopup();
		}
		if (ImGui::Button("Refresh"))
		{
			ProjectManager::LoadProject(ProjectManager::GetProjectFolderPath());
			ImGui::CloseCurrentPopup();
		}
		if (itemSelected && ImGui::Button("Delete"))
		{
			if (fileExplorerItem.file) 
			{
				FileSystem::fileSystem->DeleteFile(fileExplorerItem.file->file->GetPath());
				FileSystem::fileSystem->DeleteFile(fileExplorerItem.file->file->GetPath()+".meta");
				ProjectManager::LoadProject(ProjectManager::GetProjectFolderPath());
				if (Engine::GetSelectedFileReference() == fileExplorerItem.file)
				{
					Engine::SetSelectedFileReference(nullptr);
				}
			}
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
	return state;
}

void FileExplorerMenu::Draw()
{
	fileHovered = false;

	float iconSize = 64 * EditorUI::GetUiScale();
	ImGui::Begin("File Explorer", 0, ImGuiWindowFlags_NoCollapse);

	float height = ImGui::GetContentRegionAvail().y;

	int offset = ImGui::GetCursorPosX();
	if (ImGui::BeginTable("explorer_table", 2, ImGuiTableFlags_None | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable))
	{
		ImGui::TableNextRow(0, height);
		ImGui::TableSetColumnIndex(0);
		bool treeItemClicked = EditorUI::DrawTreeItem(ProjectManager::projectDirectory);
		if (treeItemClicked)
		{
			fileHovered = true;
		}

		ImGui::TableSetColumnIndex(1);
		float width = ImGui::GetContentRegionAvail().x;
		int colCount = width / (100 * EditorUI::GetUiScale());
		if (colCount <= 0)
			colCount = 1;

		int currentCol = 0;
		if (ImGui::BeginTable("filetable", colCount, ImGuiTableFlags_None))
		{
			ProjectDirectory* currentDir = Engine::GetCurrentProjectDirectory();
			int folderCount = currentDir->subdirectories.size();
			int fileCount = currentDir->files.size();
			std::vector <std::shared_ptr<FileReference>> filesRefs = currentDir->files;
			for (int i = 0; i < folderCount; i++)
			{
				FileExplorerItem item;
				item.directory = currentDir->subdirectories[i];
				DrawExplorerItem(iconSize, currentCol, colCount, false, offset, item, item.directory->GetFolderName());
			}

			for (int i = 0; i < fileCount; i++)
			{
				FileExplorerItem item;
				item.file = filesRefs[i];
				DrawExplorerItem(iconSize, currentCol, colCount, true, offset, item, item.file->file->GetFileName());
			}
			ImGui::EndTable();
		}
		ImGui::EndTable();

		// Unselect file or open the popup if background is clicked
		if (!fileHovered)
		{
			ProjectDirectory* currentDir = Engine::GetCurrentProjectDirectory();
			FileExplorerItem item;
			item.directory = currentDir;
			int result = CheckOpenRightClickPopupFile(item, false, "backgroundClick");
			if ((result == 0 && (ImGui::IsMouseReleased(0) || ImGui::IsMouseReleased(1))) || result != 0)
			{
				if(ImGui::IsWindowHovered())
					Engine::SetSelectedFileReference(nullptr);
			}
		}
	}

	ImGui::End();
}
