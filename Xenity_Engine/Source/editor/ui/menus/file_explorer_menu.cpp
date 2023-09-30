#include "file_explorer_menu.h"
#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include "../../../xenity.h"
#include "../../editor.h"
#include "../editor_ui.h"
#include "../../../engine/scene_management/scene_manager.h"
#include "../../../engine/asset_management/project_manager.h"
#include "../../../engine/asset_management/code_file.h"
#include "../../../engine/graphics/renderer/renderer.h"

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
	else if (item.directory)
	{
		Engine::SetCurrentProjectDirectory(item.directory);
		Engine::SetSelectedFileReference(nullptr);
	}
}

void FileExplorerMenu::DrawExplorerItem(float iconSize, int& currentCol, int colCount, bool isFile, int offset, FileExplorerItem& item, int itemIndex)
{
	std::string itemName;
	if (isFile)
		itemName = item.file->file->GetFileName();
	else
		itemName = item.directory->GetFolderName();

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

	std::shared_ptr<Texture> tex = EditorUI::icons[Icon_File];
	if (!isFile)
	{
		tex = EditorUI::icons[Icon_Folder];
	}
	else
	{
		int fileType = item.file->fileType;
		if (fileType == File_Texture)
		{
			tex = std::dynamic_pointer_cast<Texture>(item.file);
			if (tex->GetTextureId() == 0)
			{
				tex = EditorUI::icons[Icon_Image];
			}
		}
		else if (fileType == File_Scene)
		{
			tex = EditorUI::icons[Icon_Scene];
		}
		else if (fileType == File_Code)
		{
			if (std::dynamic_pointer_cast<CodeFile>(item.file)->GetIsHeader())
				tex = EditorUI::icons[Icon_Header];
			else
				tex = EditorUI::icons[Icon_Code];
		}
		else if (fileType == File_Mesh)
		{
			tex = EditorUI::icons[Icon_Mesh];
		}
		else if (fileType == File_Audio)
		{
			tex = EditorUI::icons[Icon_Audio];
		}
		else if (fileType == File_Skybox)
		{
			tex = EditorUI::icons[Icon_Sky];
		}
		else if (fileType == File_Font)
		{
			tex = EditorUI::icons[Icon_Font];
		}
		else if (fileType == File_Material)
		{
			tex = EditorUI::icons[Icon_Material];
		}
		else if (fileType == File_Shader)
		{
			tex = EditorUI::icons[Icon_Shader];
		}
	}
	bool doubleClicked = ImGui::IsMouseDoubleClicked(0);
	Engine::renderer->BindTexture(tex);
	ImGui::ImageButton(EditorUI::GenerateItemId().c_str(), (ImTextureID)tex->GetTextureId(), ImVec2(iconSize, iconSize), ImVec2(0.005f, 0.005f), ImVec2(0.995f, 0.995f));
	std::string popupId = "RightClick";
	if (item.file)
	{
		popupId += std::to_string(item.file->fileId);
	}
	else
	{
		popupId += item.directory->GetFolderName();
	}
	CheckOpenRightClickPopupFile(item, true, popupId, itemIndex);
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
	if ((fileToRename == item.file && item.file) || (directoryToRename == item.directory && item.directory))
	{
		if (!focusSet)
		{
			ImGui::SetKeyboardFocusHere();
			focusSet = true;
		}
		ImGui::InputText(EditorUI::GenerateItemId().c_str(), &renamingString, ImGuiInputTextFlags_AutoSelectAll);
		if (ImGui::IsItemClicked())
		{
			ignoreClose = true;
		}
	}
	else
	{
		if (textWidth <= availWidth)
		{
			ImGui::SetCursorPosX(cursorPos + (windowWidth - textWidth) * 0.5f);
			ImGui::Text(itemName.c_str());
		}
		else
		{
			ImGui::TextWrapped(itemName.c_str());
		}
	}

	ImGui::EndGroup();

	if (isFile)
	{
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			std::string payloadName = "Files" + std::to_string(item.file->fileType);
			ImGui::SetDragDropPayload(payloadName.c_str(), item.file.get(), sizeof(FileReference));
			ImGui::ImageButton(EditorUI::GenerateItemId().c_str(), (ImTextureID)tex->GetTextureId(), ImVec2(iconSize, iconSize));
			ImGui::TextWrapped(itemName.c_str());
			ImGui::EndDragDropSource();
		}
	}
	ImGui::PopStyleColor(3);
}

int FileExplorerMenu::CheckOpenRightClickPopupFile(FileExplorerItem& fileExplorerItem, bool itemSelected, const std::string& id, int itemIndex)
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

		if (ImGui::BeginMenu("Create"))
		{
			if (ImGui::MenuItem("Folder"))
			{
				FileSystem::fileSystem->CreateDirectory(fileExplorerItem.directory->path + "\\new Folder");
				ImGui::CloseCurrentPopup();
				ProjectManager::RefreshProjectDirectory();
			}
			if (ImGui::MenuItem("Scene"))
			{
				std::shared_ptr<File> newFile = FileSystem::MakeFile(fileExplorerItem.directory->path + "\\newScene.xen");
				newFile->Open(true);
				newFile->Close();
				ImGui::CloseCurrentPopup();
				ProjectManager::RefreshProjectDirectory();
			}
			if (ImGui::MenuItem("Skybox"))
			{
				std::shared_ptr<File> newFile = FileSystem::MakeFile(fileExplorerItem.directory->path + "\\newSkybox.sky");
				newFile->Open(true);
				newFile->Close();
				ImGui::CloseCurrentPopup();
				ProjectManager::RefreshProjectDirectory();
			}
			ImGui::EndMenu();
		}
		if (itemSelected && ImGui::MenuItem("Rename"))
		{
			fileToRename = fileExplorerItem.file;
			directoryToRename = fileExplorerItem.directory;
			if (fileToRename)
				renamingString = fileToRename->file->GetFileName();
			if (directoryToRename)
				renamingString = directoryToRename->GetFolderName();

			ImGui::CloseCurrentPopup();
		}
		if (itemSelected && ImGui::MenuItem("Open"))
		{
			OpenItem(fileExplorerItem);
			ImGui::CloseCurrentPopup();
		}
		if ((itemSelected && ImGui::MenuItem("Show in Explorer")) || (!itemSelected && ImGui::MenuItem("Open folder in Explorer")))
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
			else if (fileExplorerItem.directory)
				command += fileExplorerItem.directory->path;

			command += "\"";

			system(command.c_str());

			ImGui::CloseCurrentPopup();
		}
		if (ImGui::MenuItem("Refresh"))
		{
			ProjectManager::RefreshProjectDirectory();
			ImGui::CloseCurrentPopup();
		}
		if (itemSelected && ImGui::MenuItem("Delete"))
		{
			if (fileExplorerItem.file)
			{
				FileSystem::fileSystem->DeleteFile(fileExplorerItem.file->file->GetPath());
				FileSystem::fileSystem->DeleteFile(fileExplorerItem.file->file->GetPath() + ".meta");
				if (Engine::GetSelectedFileReference() == fileExplorerItem.file)
				{
					Engine::SetSelectedFileReference(nullptr);
				}
			}
			else if (fileExplorerItem.directory)
			{
				FileSystem::fileSystem->DeleteFile(fileExplorerItem.directory->path);
			}
			ProjectManager::RefreshProjectDirectory();
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

		if (ImGui::BeginTable("filetable", colCount, ImGuiTableFlags_None))
		{
			int currentCol = 0;
			int itemIndex = 0;
			ProjectDirectory* currentDir = Engine::GetCurrentProjectDirectory();
			int folderCount = currentDir->subdirectories.size();
			int fileCount = currentDir->files.size();
			std::vector <std::shared_ptr<FileReference>> filesRefs = currentDir->files;

			for (int i = 0; i < folderCount; i++)
			{
				FileExplorerItem item;
				item.directory = currentDir->subdirectories[i];
				DrawExplorerItem(iconSize, currentCol, colCount, false, offset, item, itemIndex);
				itemIndex++;
			}

			for (int i = 0; i < fileCount; i++)
			{
				FileExplorerItem item;
				item.file = filesRefs[i];
				DrawExplorerItem(iconSize, currentCol, colCount, true, offset, item, itemIndex);
				itemIndex++;
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
			int result = CheckOpenRightClickPopupFile(item, false, "backgroundClick", -1);
			if (result != 0 || (ImGui::IsMouseReleased(0) || ImGui::IsMouseReleased(1)))
			{
				if (ignoreClose)
				{
					ignoreClose = false;
				}
				else
				{
					if (result == 0)
					{
						Rename();
					}
					if (ImGui::IsWindowHovered())
						Engine::SetSelectedFileReference(nullptr);
				}
			}
		}
		if (InputSystem::GetKeyDown(RETURN))
		{
			Rename();
		}
	}

	ImGui::End();
}

void FileExplorerMenu::Rename()
{
	bool needUpdate = false;
	if (!renamingString.empty() && fileToRename)
	{
		needUpdate = true;

		bool success = false;
		std::shared_ptr<File> file = fileToRename->file;
		success = FileSystem::fileSystem->Rename(file->GetPath(), file->GetFolderPath() + renamingString + file->GetFileExtension());
		if (success)
			FileSystem::fileSystem->Rename(file->GetPath() + ".meta", file->GetFolderPath() + renamingString + file->GetFileExtension() + ".meta");
	}
	else if (!renamingString.empty() && directoryToRename)
	{
		needUpdate = true;

		bool success = false;
		std::string parentPath = directoryToRename->path;
		int lastSlash = parentPath.find_last_of('\\', parentPath.size() - 2);
		parentPath = parentPath.substr(0, lastSlash) + "\\";
		success = FileSystem::fileSystem->Rename(directoryToRename->path, parentPath + renamingString);
	}

	fileToRename.reset();
	directoryToRename = nullptr;
	focusSet = false;

	if (needUpdate)
		ProjectManager::RefreshProjectDirectory();
}