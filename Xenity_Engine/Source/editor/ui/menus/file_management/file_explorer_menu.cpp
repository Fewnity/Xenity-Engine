// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "file_explorer_menu.h"

#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>

#include <editor/editor.h>
#include <editor/ui/editor_ui.h>

#include <engine/file_system/file.h>
#include <engine/scene_management/scene_manager.h>
#include <engine/scene_management/scene.h>
#include <engine/asset_management/project_manager.h>
#include <engine/graphics/renderer/renderer.h>
#include <engine/game_elements/gameplay_manager.h>
#include <engine/file_system/file_system.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/inputs/input_system.h>
#include <engine/ui/window.h>
#include <editor/ui/utils/menu_builder.h>
#include <editor/file_handler/file_handler.h>
#include <engine/engine.h>

#include "create_class_menu.h"
#include <engine/graphics/texture/texture_default.h>
#include <engine/game_elements/prefab.h>
#include <editor/ui/editor_icons.h>

void FileExplorerMenu::Init()
{
}

void FileExplorerMenu::OpenItem(const FileExplorerItem& item)
{
	if (item.file) // Do a specific action if the file can be opened
	{
		if (item.file->GetFileType() == FileType::File_Scene) // If the file is a scene, load the scene
		{
			GameplayManager::SetGameState(GameState::Stopped, false);
			SceneManager::LoadSceneInternal(std::dynamic_pointer_cast<Scene>(item.file));
		}
		else if (item.file->GetFileType() == FileType::File_Code || item.file->GetFileType() == FileType::File_Header || item.file->GetFileType() == FileType::File_Shader) // If the file is something like code, open Visual Studio Code
		{
			// Open the folder to allow vs code c++ settings
			std::string command = "code \"" + ProjectManager::GetAssetFolderPath() + "\"";
			system(command.c_str());

			// Open the file
			command = "code \"" + item.file->m_file->GetPath() + "\"";
			system(command.c_str());
		}
	}
	else if (item.directory) // Open the folder
	{
		Editor::SetCurrentProjectDirectory(item.directory);
	}
}

void FileExplorerMenu::SetFileToRename(const std::shared_ptr<FileReference>& file, const std::shared_ptr<ProjectDirectory>& directory)
{
	m_fileToRename = file;
	m_directoryToRename = directory;
	if (m_fileToRename)
		m_renamingString = m_fileToRename->m_file->GetFileName();
	else if (m_directoryToRename)
		m_renamingString = m_directoryToRename->GetFolderName();
}

void FileExplorerMenu::DrawExplorerItem(const float iconSize, int& currentCol, const int colCount, const float offset, const FileExplorerItem& item)
{
	//Get name
	std::string itemName;
	if (item.file)
		itemName = item.file->m_file->GetFileName();
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

	int childFlags = ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysUseWindowPadding;
	bool isSelected = Editor::GetSelectedFileReference() == item.file && item.file;
	// Set child background color
	if (isSelected)
	{
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
		childFlags |= ImGuiChildFlags_Borders;
	}
	ImVec2 oldPadding = ImGui::GetStyle().WindowPadding;
	ImGui::GetStyle().WindowPadding = ImVec2(0, 14);
	ImGui::BeginChild(EditorUI::GenerateItemId().c_str(), ImVec2(0,0), childFlags);
	ImGui::BeginGroup();
	ImGui::GetStyle().WindowPadding = oldPadding;
	const int cursorPos = (int)ImGui::GetCursorPosX();
	const int availWidth = (int)ImGui::GetContentRegionAvail().x;
	ImGui::SetCursorPosX(cursorPos + (availWidth - iconSize) / 2.0f - offset / 2.0f);
	ImVec2 imageCursorPos = ImGui::GetCursorPos();
	std::shared_ptr<Texture> iconTexture = GetItemIcon(item);

	const bool doubleClicked = ImGui::IsMouseDoubleClicked(0);
	iconTexture->Bind();

	ImVec4 color = ImVec4(1, 1, 1, 1);
	if (item.file && item.file->GetFileType() == FileType::File_Material)
	{
		const RGBA rgba = std::dynamic_pointer_cast<Material>(item.file)->GetColor().GetRGBA();
		color = ImVec4(rgba.r, rgba.g, rgba.b, rgba.a);
	}

	ImGui::ImageButton(EditorUI::GenerateItemId().c_str(), (ImTextureID)(size_t)EditorUI::GetTextureId(*iconTexture), ImVec2(iconSize, iconSize), ImVec2(0.005f, 0.005f), ImVec2(0.995f, 0.995f), ImVec4(0, 0, 0, 0), color);
	const bool hovered = ImGui::IsItemHovered();

	// Create an unique popupid
	std::string popupId = "RightClick";
	if (item.file)
	{
		popupId += std::to_string(item.file->m_fileId);
	}
	else
	{
		popupId += item.directory->GetFolderName();
	}
	CheckOpenRightClickPopupFile(item, true, popupId);

	const ImVec2 finalImageCursorPos = ImGui::GetCursorPos();

	if (item.file && item.file->GetFileType() == FileType::File_Material)
	{
		const std::shared_ptr<Texture>& matTexture = EditorIcons::GetIcons()[(int)IconName::Icon_Material];
		matTexture->Bind();
		imageCursorPos.x -= iconSize / 3 / 2;
		imageCursorPos.y -= iconSize / 3 / 2;

		ImGui::SetCursorPos(imageCursorPos);
		ImGui::Image((ImTextureID)(size_t)EditorUI::GetTextureId(*matTexture), ImVec2(iconSize / 3, iconSize / 3), ImVec2(0.005f, 0.005f), ImVec2(0.995f, 0.995f));
	}

	ImGui::SetCursorPos(finalImageCursorPos);

	if (hovered)
	{
		m_fileHovered = true;
	}
	if (hovered && ((ImGui::IsMouseClicked(0) && doubleClicked) || (ImGui::IsMouseReleased(0) || ImGui::IsMouseReleased(1))) && !m_cancelNextClick)
	{
		if (doubleClicked)
		{
			OpenItem(item);
			m_cancelNextClick = true;
		}
		else
		{
			if (item.file)
			{
				Editor::SetSelectedFileReference(item.file);
			}
		}
	}

	const float windowWidth = ImGui::GetContentRegionAvail().x;
	const float textWidth = ImGui::CalcTextSize(itemName.c_str()).x;
	if ((m_fileToRename == item.file && item.file) || (m_directoryToRename == item.directory && item.directory && !item.file))
	{
		if (!m_focusSet)
		{
			ImGui::SetKeyboardFocusHere();
			m_focusSet = true;
		}
		ImGui::SetNextItemWidth(-1);
		ImGui::InputText(EditorUI::GenerateItemId().c_str(), &m_renamingString, ImGuiInputTextFlags_AutoSelectAll);
		if (ImGui::IsItemClicked())
		{
			m_ignoreClose = true;
		}
	}
	else
	{
		if (textWidth <= availWidth)
		{
			ImGui::SetCursorPosX(cursorPos + (windowWidth - textWidth) * 0.5f);
			ImGui::Text("%s", itemName.c_str());
		}
		else
		{
			ImGui::TextWrapped("%s", itemName.c_str());
		}
	}

	ImGui::EndGroup();
	// Set a drag drop target for folders
	if (!item.file)
	{
		std::shared_ptr <FileReference> fileRef = nullptr;
		const bool dropFileInFolder = EditorUI::DragDropTarget("Files", fileRef);
		if (dropFileInFolder)
		{
			const std::shared_ptr<File>& file = fileRef->m_file;
			CopyFileResult copyResult = FileSystem::CopyFile(file->GetPath(), item.directory->path + file->GetFileName() + file->GetFileExtension(), false);
			if (copyResult == CopyFileResult::Success)
			{
				copyResult = FileSystem::CopyFile(file->GetPath() + ".meta", item.directory->path + file->GetFileName() + file->GetFileExtension() + ".meta", false);

				if (copyResult == CopyFileResult::Success)
				{
					FileSystem::Delete(file->GetPath());
					FileSystem::Delete(file->GetPath() + ".meta");
				}
			}

			ProjectManager::RefreshProjectDirectory();
		}
		std::shared_ptr <ProjectDirectory> directoryRef = nullptr;
		const bool dropFolderInFolder = EditorUI::DragDropTarget("Folders", directoryRef);
		if (dropFolderInFolder)
		{
			const std::string destinationPath = item.directory->path + directoryRef->GetFolderName() + "\\";
			FileSystem::CreateFolder(destinationPath);
			Editor::StartFolderCopy(directoryRef->path, destinationPath);
			FileSystem::Delete(directoryRef->path);
			ProjectManager::RefreshProjectDirectory();
		}
	}
	CheckItemDrag(item, *iconTexture, iconSize, itemName);
	ImGui::EndChild();
	if (isSelected)
	{
		ImGui::PopStyleColor(1);
	}

	ImGui::PopStyleColor(3);
}

int FileExplorerMenu::CheckOpenRightClickPopupFile(const FileExplorerItem& fileExplorerItem, const bool itemSelected, const std::string& id)
{
	RightClickMenu fileExplorerRightClickMenu = RightClickMenu(id);
	RightClickMenuState rightClickState = fileExplorerRightClickMenu.Check(false, !itemSelected);
	if (rightClickState != RightClickMenuState::Closed)
	{
		//-
		RightClickMenuItem* createItem = fileExplorerRightClickMenu.AddItem("Create");
		//--
		createItem->AddItem("Folder", [&fileExplorerItem]()
			{
				FileSystem::CreateFolder(fileExplorerItem.directory->path + "\\new Folder");
				ProjectManager::RefreshProjectDirectory();
			});
		createItem->AddItem("Scene", [this, &fileExplorerItem]()
			{
				std::shared_ptr<File> newFile = Editor::CreateNewFile(fileExplorerItem.directory->path + "\\newScene", FileType::File_Scene, true);
				std::shared_ptr<FileReference> newFileRef = ProjectManager::GetFileReferenceByFile(*newFile);
				SetFileToRename(newFileRef, nullptr);
			});
		createItem->AddItem("Skybox", [this, &fileExplorerItem]()
			{
				std::shared_ptr<File> newFile = Editor::CreateNewFile(fileExplorerItem.directory->path + "\\newSkybox", FileType::File_Skybox, true);
				std::shared_ptr<FileReference> newFileRef = ProjectManager::GetFileReferenceByFile(*newFile);
				SetFileToRename(newFileRef, nullptr);
			});
		createItem->AddItem("Shader", [this, &fileExplorerItem]()
			{
				std::shared_ptr<File> newFile = Editor::CreateNewFile(fileExplorerItem.directory->path + "\\newShader", FileType::File_Shader, true);
				std::shared_ptr<FileReference> newFileRef = ProjectManager::GetFileReferenceByFile(*newFile);
				SetFileToRename(newFileRef, nullptr);
			});
		createItem->AddItem("Material", [this, &fileExplorerItem]()
			{
				std::shared_ptr<File> newFile = Editor::CreateNewFile(fileExplorerItem.directory->path + "\\newMaterial", FileType::File_Material, true);
				std::shared_ptr<FileReference> newFileRef = ProjectManager::GetFileReferenceByFile(*newFile);
				SetFileToRename(newFileRef, nullptr);
			});
		createItem->AddItem("C++ Class", [&fileExplorerItem]()
			{
				auto createClassMenu = Editor::GetMenu<CreateClassMenu>();
				createClassMenu->SetActive(true);
				createClassMenu->Reset();
				createClassMenu->SetFolderPath(fileExplorerItem.directory->path);
			});
		//-
		RightClickMenuItem* RenameItem = fileExplorerRightClickMenu.AddItem("Rename", [this, &fileExplorerItem]()
			{
				m_fileToRename = fileExplorerItem.file;
				m_directoryToRename = fileExplorerItem.directory;
				if (m_fileToRename)
					m_renamingString = m_fileToRename->m_file->GetFileName();
				else if (m_directoryToRename)
					m_renamingString = m_directoryToRename->GetFolderName();
			});
		RenameItem->SetIsVisible(itemSelected);
		RightClickMenuItem* openMenuItem = fileExplorerRightClickMenu.AddItem("Open", [this, &fileExplorerItem]()
			{
				OpenItem(fileExplorerItem);
			});
		openMenuItem->SetIsVisible(itemSelected);
		std::string explorerTitle = "Show in Explorer";
		if (!itemSelected)
			explorerTitle = "Open folder in Explorer";
		fileExplorerRightClickMenu.AddItem(explorerTitle, [&fileExplorerItem, &itemSelected]()
			{
				if (fileExplorerItem.file)
					Editor::OpenExplorerWindow(fileExplorerItem.file->m_file->GetPath(), itemSelected);
				else if (fileExplorerItem.directory)
					Editor::OpenExplorerWindow(fileExplorerItem.directory->path, itemSelected);

			});
		fileExplorerRightClickMenu.AddItem("Refresh", []()
			{
				ProjectManager::RefreshProjectDirectory();
			});
		if (fileExplorerItem.file && fileExplorerItem.file->GetFileType() == FileType::File_Texture)
		{
			fileExplorerRightClickMenu.AddItem("Create material for this", [&fileExplorerItem]()
				{
					const std::string path = fileExplorerItem.directory->path + "\\" + fileExplorerItem.file->m_file->GetFileName();
					std::shared_ptr<File> file = Editor::CreateNewFile(path, FileType::File_Material, true);
					if (file) 
					{
						std::shared_ptr<FileReference> newMaterialFileRef = ProjectManager::GetFileReferenceByFile(*file);
						std::shared_ptr<Material> newMaterial = std::dynamic_pointer_cast<Material>(newMaterialFileRef);
						newMaterial->SetTexture(std::dynamic_pointer_cast<Texture>(fileExplorerItem.file));
						newMaterial->SetShader(AssetManager::standardShader);
						newMaterial->SetUseLighting(true);
						newMaterialFileRef->OnReflectionUpdated();
					}
				});
		}
		RightClickMenuItem* deleteMenuItem = fileExplorerRightClickMenu.AddItem("Delete", [&fileExplorerItem]()
			{
				if (fileExplorerItem.file)
				{
					FileSystem::Delete(fileExplorerItem.file->m_file->GetPath());
					FileSystem::Delete(fileExplorerItem.file->m_file->GetPath() + ".meta");
					FileHandler::RemoveOneFile();
					if (Editor::GetSelectedFileReference() == fileExplorerItem.file)
					{
						Editor::SetSelectedFileReference(nullptr);
					}
				}
				else if (fileExplorerItem.directory)
				{
					FileSystem::Delete(fileExplorerItem.directory->path);
				}
				ProjectManager::RefreshProjectDirectory();
			});
		deleteMenuItem->SetIsVisible(itemSelected);
	}

	const bool rightClickDrawn = fileExplorerRightClickMenu.Draw();

	int state = 0;
	if (rightClickDrawn)
	{
		m_fileHovered = true;
		if (rightClickState == RightClickMenuState::JustOpened)
		{
			state = 1;
		}
		else
		{
			state = 2;
		}
	}
	return state;
}

void FileExplorerMenu::CheckItemDrag(const FileExplorerItem& fileExplorerItem, const Texture& iconTexture, const float iconSize, const std::string& itemName)
{
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
	{
		std::string payloadName;
		if (fileExplorerItem.file)
		{
			if (m_isHovered)
				payloadName = "Files";
			else
				payloadName = "Files" + std::to_string((int)fileExplorerItem.file->GetFileType());

			ImGui::SetDragDropPayload(payloadName.c_str(), fileExplorerItem.file.get(), sizeof(FileReference));
		}
		else
		{
			payloadName = "Folders";
			ImGui::SetDragDropPayload(payloadName.c_str(), fileExplorerItem.directory.get(), sizeof(ProjectDirectory));
		}

		const TextureDefault& openglTexture = dynamic_cast<const TextureDefault&>(iconTexture);
		ImGui::Image((ImTextureID)(size_t)openglTexture.GetTextureId(), ImVec2(iconSize, iconSize));
		ImGui::TextWrapped("%s", itemName.c_str());
		ImGui::EndDragDropSource();
	}
}

std::shared_ptr<Texture> FileExplorerMenu::GetItemIcon(const FileExplorerItem& fileExplorerItem)
{
	// Get item icon
	std::shared_ptr<Texture> tex = EditorIcons::GetIcons()[(int)IconName::Icon_File];
	if (!fileExplorerItem.file)
	{
		tex = EditorIcons::GetIcons()[(int)IconName::Icon_Folder];
	}
	else
	{
		const FileType fileType = fileExplorerItem.file->GetFileType();

		switch (fileType)
		{
		case FileType::File_Texture:
		{
			tex = std::dynamic_pointer_cast<Texture>(fileExplorerItem.file);
			if (EditorUI::GetTextureId(*tex) == 0)
			{
				tex = EditorIcons::GetIcons()[(int)IconName::Icon_Image];
			}
			break;
		}
		case FileType::File_Scene:
			tex = EditorIcons::GetIcons()[(int)IconName::Icon_Scene];
			break;
		case FileType::File_Code:
			tex = EditorIcons::GetIcons()[(int)IconName::Icon_Code];
			break;
		case FileType::File_Header:
			tex = EditorIcons::GetIcons()[(int)IconName::Icon_Header];
			break;
		case FileType::File_Mesh:
			tex = EditorIcons::GetIcons()[(int)IconName::Icon_Mesh];
			break;
		case FileType::File_Audio:
			tex = EditorIcons::GetIcons()[(int)IconName::Icon_Audio];
			break;
		case FileType::File_Skybox:
			tex = EditorIcons::GetIcons()[(int)IconName::Icon_Sky];
			break;
		case FileType::File_Font:
			tex = EditorIcons::GetIcons()[(int)IconName::Icon_Font];
			break;
		case FileType::File_Material:
		{
			std::shared_ptr<Material> mat = std::dynamic_pointer_cast<Material>(fileExplorerItem.file);
			if (!mat->GetTexture() || EditorUI::GetTextureId(*mat->GetTexture()) == 0)
			{
				tex = AssetManager::defaultTexture;
			}
			else
			{
				tex = mat->GetTexture();
			}
			break;
		}
		case FileType::File_Shader:
			tex = EditorIcons::GetIcons()[(int)IconName::Icon_Shader];
			break;

		case FileType::File_Other:
			break;
		}
	}
	return tex;
}

void FileExplorerMenu::Draw()
{
	m_fileHovered = false;

	const float iconSize = 64 * EditorUI::GetUiScale();
	std::string windowName = "File Explorer###File_Explorer" + std::to_string(id);

	const bool visible = ImGui::Begin(windowName.c_str(), &m_isActive, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);

	if (visible)
	{
		OnStartDrawing();

		const float offset = ImGui::GetCursorPosX();
		if (ImGui::BeginTable("explorer_table", 2, ImGuiTableFlags_None | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable))
		{
			ImGui::TableNextRow(0, m_startAvailableSize.y);
			ImGui::TableSetColumnIndex(0);
			ImGui::BeginChild("explorer_table_folder_tree_child");
			const bool treeItemClicked = EditorUI::DrawTreeItem(ProjectManager::GetProjectDirectory());
			if (treeItemClicked)
			{
				m_fileHovered = true;
			}
			ImGui::EndChild();
			ImGui::TableSetColumnIndex(1);
			const float width = ImGui::GetContentRegionAvail().x;
			int colCount = static_cast<int>(width / (100 * EditorUI::GetUiScale()));
			if (colCount <= 0)
				colCount = 1;

			std::shared_ptr <ProjectDirectory> currentDir = Editor::GetCurrentProjectDirectory();
			if (ImGui::BeginTable("filetable", colCount, ImGuiTableFlags_None | ImGuiTableFlags_ScrollY))
			{
				int currentCol = 0;
				if (currentDir)
				{
					size_t folderCount = currentDir->subdirectories.size();
					size_t fileCount = currentDir->files.size();
					std::vector <std::shared_ptr<FileReference>> filesRefs = currentDir->files;

					for (size_t i = 0; i < folderCount; i++)
					{
						FileExplorerItem item;
						item.directory = currentDir->subdirectories[i];
						DrawExplorerItem(iconSize, currentCol, colCount, offset, item);
					}

					for (size_t i = 0; i < fileCount; i++)
					{
						FileExplorerItem item;
						item.file = filesRefs[i];
						item.directory = currentDir;
						DrawExplorerItem(iconSize, currentCol, colCount, offset, item);
					}
				}
				ImGui::EndTable();
				// Unselect file or open the popup if background is clicked
				if (!m_fileHovered && ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem | ImGuiHoveredFlags_ChildWindows | ImGuiHoveredFlags_AllowWhenBlockedByPopup))
				{
					std::shared_ptr <ProjectDirectory> currentDir = Editor::GetCurrentProjectDirectory();
					FileExplorerItem item;
					item.directory = currentDir;
					const int result = CheckOpenRightClickPopupFile(item, false, "backgroundClick");
					if (result != 0 || (ImGui::IsMouseReleased(0) || ImGui::IsMouseReleased(1)))
					{
						if (m_ignoreClose)
						{
							m_ignoreClose = false;
						}
						else
						{
							if (result == 0)
							{
								Rename();
							}
							if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows))
							{
								Editor::SetSelectedFileReference(nullptr);
							}
						}
					}
				}
			}
			ImGui::EndTable();

			// Detect drag and drop for prefabs
			// std::shared_ptr<GameObject> is unused because we only want to detect the drop 
			std::shared_ptr<GameObject> unused = nullptr;
			const bool droppedGameObject = EditorUI::DragDropTarget("MultiDragData", unused);
			if (droppedGameObject)
			{
				if (EditorUI::multiDragData.gameObjects.size() == 1)
				{
					Debug::Print("Create prefab of: " + EditorUI::multiDragData.gameObjects[0]->GetName());
					std::shared_ptr<File> newFile = Editor::CreateNewFile(currentDir->path + "\\" + EditorUI::multiDragData.gameObjects[0]->GetName(), FileType::File_Prefab, true);
					std::shared_ptr<FileReference> newFileRef = ProjectManager::GetFileReferenceByFile(*newFile);
					if (newFileRef)
					{
						std::shared_ptr<Prefab> prefab = std::dynamic_pointer_cast<Prefab>(newFileRef);
						prefab->SetData(*EditorUI::multiDragData.gameObjects[0]);
					}
					//SetFileToRename(newFileRef, nullptr);
				}
			}
			if (InputSystem::GetKeyDown(KeyCode::RETURN))
			{
				Rename();
			}
		}

		CalculateWindowValues();
		// Recheck for isHovered because of child windows and dragging item UI
		m_isHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem | ImGuiHoveredFlags_ChildWindows); // Do this after CalculateWindowValues()
	}
	else
	{
		ResetWindowValues();
	}

	if (ImGui::IsMouseReleased(0))
	{
		m_cancelNextClick = false;
	}
	ImGui::End();
}

void FileExplorerMenu::Rename()
{
	bool needTitleRefresh = false;
	bool needUpdate = false;
	if (!m_renamingString.empty() && m_fileToRename)
	{
		needUpdate = true;

		std::shared_ptr<File> file = m_fileToRename->m_file;
		const bool success = FileSystem::Rename(file->GetPath(), file->GetFolderPath() + m_renamingString + file->GetFileExtension());
		if (success)
		{
			FileSystem::Rename(file->GetPath() + ".meta", file->GetFolderPath() + m_renamingString + file->GetFileExtension() + ".meta");
			if (SceneManager::GetOpenedScene() == m_fileToRename)
			{
				needTitleRefresh = true;
			}
		}
		else if(m_renamingString + file->GetFileExtension() != file->GetFileName() + file->GetFileExtension())
		{
			EditorUI::OpenDialog("Error", "There is already a file with the same name in this location.", DialogType::Dialog_Type_OK);
		}
	}
	else if (!m_renamingString.empty() && m_directoryToRename)
	{
		needUpdate = true;

		std::string parentPath = m_directoryToRename->path;
		// Remove the old folder name from the path
		const size_t lastSlash = parentPath.find_last_of('/', parentPath.size() - 2);
		parentPath = parentPath.substr(0, lastSlash) + "/";

		const bool success = FileSystem::Rename(m_directoryToRename->path, parentPath + m_renamingString + "/");
	}

	m_fileToRename.reset();
	m_directoryToRename = nullptr;
	m_focusSet = false;

	if (needUpdate)
		ProjectManager::RefreshProjectDirectory();

	if (needTitleRefresh)
	{
		Window::UpdateWindowTitle(); // If it's a scene, update the window title
	}
}