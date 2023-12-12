#include "file_explorer_menu.h"
#include "create_class_menu.h"

#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>

#include <editor/editor.h>
#include <editor/ui/editor_ui.h>

#include <engine/file_system/file.h>
#include <engine/scene_management/scene_manager.h>
#include <engine/scene_management/scene.h>
#include <engine/asset_management/project_manager.h>
#include <engine/asset_management/code_file.h>
#include <engine/graphics/renderer/renderer.h>
#include <engine/game_elements/gameplay_manager.h>
#include <engine/file_system/file_system.h>
#include <engine/inputs/input_system.h>
#include <engine/ui/window.h>
#include <iostream>
#include <engine/debug/debug.h>

void FileExplorerMenu::Init()
{
}

void FileExplorerMenu::OpenItem(FileExplorerItem& item)
{
	if (item.file) // Do a specific action if the file can be opened
	{
		if (item.file->fileType == File_Scene)
		{
			GameplayManager::SetGameState(Stopped, false);
			SceneManager::LoadScene(std::dynamic_pointer_cast<Scene>(item.file));
		}
		else if (item.file->fileType == File_Code || item.file->fileType == File_Header || item.file->fileType == File_Shader)
		{
			std::string command = "code \"" + item.file->file->GetPath() + "\"";
			system(command.c_str());
		}
	}
	else if (item.directory) // Open the folder
	{
		Editor::SetCurrentProjectDirectory(item.directory);
		Editor::SetSelectedFileReference(nullptr);
	}
}

void FileExplorerMenu::DrawExplorerItem(float iconSize, int& currentCol, int colCount, bool isFile, float offset, FileExplorerItem& item, int itemIndex)
{
	//Get name
	std::string itemName;
	if (isFile)
		itemName = item.file->file->GetFileName();
	else
		itemName = item.directory->GetFolderName();

	// Set item table position
	if (currentCol == 0)
		ImGui::TableNextRow();
	ImGui::TableSetColumnIndex(currentCol);

	currentCol++;
	currentCol %= colCount;

	// Set style
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.1f, 0.2f, 0.3f, 0.5f));

	ImGui::BeginGroup();
	int cursorPos = (int)ImGui::GetCursorPosX();
	int availWidth = (int)ImGui::GetContentRegionAvail().x;
	ImGui::SetCursorPosX(cursorPos + (availWidth - iconSize) / 2.0f - offset / 2.0f);

	std::shared_ptr<Texture> iconTexture = GetItemIcon(item, isFile);

	bool doubleClicked = ImGui::IsMouseDoubleClicked(0);
	Engine::GetRenderer().BindTexture(iconTexture);
	ImGui::ImageButton(EditorUI::GenerateItemId().c_str(), (ImTextureID)iconTexture->GetTextureId(), ImVec2(iconSize, iconSize), ImVec2(0.005f, 0.005f), ImVec2(0.995f, 0.995f));

	// Create an unique popupid
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
	if (hovered && ((ImGui::IsMouseClicked(0) && doubleClicked) || (ImGui::IsMouseReleased(0) || ImGui::IsMouseReleased(1))) && !cancelNextClick)
	{
		if (doubleClicked)
		{
			OpenItem(item);
			cancelNextClick = true;
		}
		else
		{
			if (isFile)
			{
				Editor::SetSelectedFileReference(item.file);
				Editor::SetSelectedGameObject(nullptr);
			}
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
		ImGui::SetNextItemWidth(-1);
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

	// Set a drag drop target for folders
	if (!isFile)
	{
		std::shared_ptr <FileReference> fileRef;
		bool dropFileInFolder = EditorUI::DragDropTarget("Files", fileRef);
		if (dropFileInFolder)
		{
			int copyResult = FileSystem::fileSystem->CopyFile(fileRef->file->GetPath(), item.directory->path + fileRef->file->GetFileName() + fileRef->file->GetFileExtension(), false);
			if (copyResult == 0)
			{
				copyResult = FileSystem::fileSystem->CopyFile(fileRef->file->GetPath() + ".meta", item.directory->path + fileRef->file->GetFileName() + fileRef->file->GetFileExtension() + ".meta", false);

				if (copyResult == 0)
				{
					FileSystem::fileSystem->Delete(fileRef->file->GetPath());
					FileSystem::fileSystem->Delete(fileRef->file->GetPath() + ".meta");
				}
			}

			ProjectManager::RefreshProjectDirectory();
		}
		std::shared_ptr <ProjectDirectory> directoryRef;
		bool dropFolderInFolder = EditorUI::DragDropTarget("Folders", directoryRef);
		if (dropFolderInFolder) 
		{
			std::string destinationPath = item.directory->path + directoryRef->GetFolderName() + "\\";
			FileSystem::fileSystem->CreateDirectory(destinationPath);
			Editor::StartFolderCopy(directoryRef->path, destinationPath);
			FileSystem::fileSystem->Delete(directoryRef->path);
			ProjectManager::RefreshProjectDirectory();
		}
	}
	CheckItemDrag(item, isFile, iconTexture, iconSize, itemName);

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
				ProjectManager::RefreshProjectDirectory();
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Scene"))
			{
				Editor::CreateNewFile(fileExplorerItem.directory->path + "\\newScene", FileType::File_Scene, true);
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Skybox"))
			{
				Editor::CreateNewFile(fileExplorerItem.directory->path + "\\newSkybox", FileType::File_Skybox, true);
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Shader"))
			{
				Editor::CreateNewFile(fileExplorerItem.directory->path + "\\newShader", FileType::File_Shader, true);
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Material"))
			{
				Editor::CreateNewFile(fileExplorerItem.directory->path + "\\newMaterial", FileType::File_Material, true);
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("C++ Class"))
			{
				EditorUI::showCreateClass = true;
				Editor::createClassMenu->Reset();
				Editor::createClassMenu->filePath = fileExplorerItem.directory->path;
				ImGui::CloseCurrentPopup();
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
			if (fileExplorerItem.file)
				Editor::OpenExplorerWindow(fileExplorerItem.file->file->GetPath(), itemSelected);
			else if (fileExplorerItem.directory)
				Editor::OpenExplorerWindow(fileExplorerItem.directory->path, itemSelected);

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
				FileSystem::fileSystem->Delete(fileExplorerItem.file->file->GetPath());
				FileSystem::fileSystem->Delete(fileExplorerItem.file->file->GetPath() + ".meta");
				if (Editor::GetSelectedFileReference() == fileExplorerItem.file)
				{
					Editor::SetSelectedFileReference(nullptr);
				}
			}
			else if (fileExplorerItem.directory)
			{
				FileSystem::fileSystem->Delete(fileExplorerItem.directory->path);
			}
			ProjectManager::RefreshProjectDirectory();
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
	return state;
}

void FileExplorerMenu::CheckItemDrag(FileExplorerItem& fileExplorerItem, bool isFile, std::shared_ptr<Texture>& iconTexture, float iconSize, const std::string& itemName)
{
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
	{
		std::string payloadName;
		if (isFile)
		{
			payloadName = "Files" + std::to_string(fileExplorerItem.file->fileType);
			if (isHovered)
				payloadName = "Files";
			ImGui::SetDragDropPayload(payloadName.c_str(), fileExplorerItem.file.get(), sizeof(FileReference));
		}
		else
		{
			payloadName = "Folders";
			ImGui::SetDragDropPayload(payloadName.c_str(), fileExplorerItem.directory.get(), sizeof(ProjectDirectory));
		}

		ImGui::ImageButton(EditorUI::GenerateItemId().c_str(), (ImTextureID)iconTexture->GetTextureId(), ImVec2(iconSize, iconSize));
		ImGui::TextWrapped(itemName.c_str());
		ImGui::EndDragDropSource();
	}
}

std::shared_ptr<Texture> FileExplorerMenu::GetItemIcon(FileExplorerItem& fileExplorerItem, bool isFile)
{
	// Get item icon
	std::shared_ptr<Texture> tex = EditorUI::icons[Icon_File];
	if (!isFile)
	{
		tex = EditorUI::icons[Icon_Folder];
	}
	else
	{
		int fileType = fileExplorerItem.file->fileType;
		if (fileType == File_Texture)
		{
			tex = std::dynamic_pointer_cast<Texture>(fileExplorerItem.file);
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
			tex = EditorUI::icons[Icon_Code];
		}
		else if (fileType == File_Header)
		{
			tex = EditorUI::icons[Icon_Header];
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
	return tex;
}

void FileExplorerMenu::Draw()
{
	fileHovered = false;

	float iconSize = 64 * EditorUI::GetUiScale();
	bool visible = ImGui::Begin("File Explorer", 0, ImGuiWindowFlags_NoCollapse);
	if (visible)
	{
		float height = ImGui::GetContentRegionAvail().y;

		float offset = ImGui::GetCursorPosX();
		if (ImGui::BeginTable("explorer_table", 2, ImGuiTableFlags_None | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable))
		{
			ImGui::TableNextRow(0, height);
			ImGui::TableSetColumnIndex(0);
			bool treeItemClicked = EditorUI::DrawTreeItem(ProjectManager::GetProjectDirectory());
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
				std::shared_ptr <ProjectDirectory> currentDir = Editor::GetCurrentProjectDirectory();
				size_t folderCount = currentDir->subdirectories.size();
				size_t fileCount = currentDir->files.size();
				std::vector <std::shared_ptr<FileReference>> filesRefs = currentDir->files;

				for (size_t i = 0; i < folderCount; i++)
				{
					FileExplorerItem item;
					item.directory = currentDir->subdirectories[i];
					DrawExplorerItem(iconSize, currentCol, colCount, false, offset, item, itemIndex);
					itemIndex++;
				}

				for (size_t i = 0; i < fileCount; i++)
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
				std::shared_ptr <ProjectDirectory> currentDir = Editor::GetCurrentProjectDirectory();
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
							Editor::SetSelectedFileReference(nullptr);
					}
				}
			}
			if (InputSystem::GetKeyDown(RETURN))
			{
				Rename();
			}
		}
		isHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
	}
	else 
	{
		isHovered = true;
	}
	if (ImGui::IsMouseReleased(0))
	{
		cancelNextClick = false;
	}
	ImGui::End();
}

void FileExplorerMenu::Rename()
{
	bool needTitleRefresh = false;
	bool needUpdate = false;
	if (!renamingString.empty() && fileToRename)
	{
		needUpdate = true;

		bool success = false;
		std::shared_ptr<File> file = fileToRename->file;
		success = FileSystem::fileSystem->Rename(file->GetPath(), file->GetFolderPath() + renamingString + file->GetFileExtension());
		if (success)
		{
			FileSystem::fileSystem->Rename(file->GetPath() + ".meta", file->GetFolderPath() + renamingString + file->GetFileExtension() + ".meta");
			if (SceneManager::GetOpenedScene() == fileToRename)
			{
				needTitleRefresh = true;
			}
		}
		else
		{
			EditorUI::OpenDialog("Error", "There is already a file with the same name in this location.", Dialog_Type_OK);
		}
	}
	else if (!renamingString.empty() && directoryToRename)
	{
		needUpdate = true;

		bool success = false;
		std::string parentPath = directoryToRename->path;
		size_t lastSlash = parentPath.find_last_of('\\', parentPath.size() - 2);
		parentPath = parentPath.substr(0, lastSlash) + "\\";
		success = FileSystem::fileSystem->Rename(directoryToRename->path, parentPath + renamingString);
	}

	fileToRename.reset();
	directoryToRename = nullptr;
	focusSet = false;

	if (needUpdate)
		ProjectManager::RefreshProjectDirectory();

	if (needTitleRefresh)
	{
		Window::UpdateWindowTitle(); // If it's a scene, update the window title
	}
}