// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include "menu.h"
#include <string>
#include <memory>

class FileReference;
class ProjectDirectory;
class Texture;

struct FileExplorerItem
{
	std::shared_ptr<FileReference> file = nullptr;
	std::shared_ptr <ProjectDirectory> directory = nullptr;
};

class FileExplorerMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;

	/**
	* Get icon texture from a FileExplorerItem
	*/
	static std::shared_ptr<Texture> GetItemIcon(const FileExplorerItem& fileExplorerItem);

private:

	/**
	* Draw a file/folder icon and check for clicks/drag
	*/
	void DrawExplorerItem(const float iconSize, int& currentCol, const int colCount, const float offset, const FileExplorerItem& item, const int itemIndex);
	
	/**
	* Check for right click on item and draw the right click menu
	*/
	int CheckOpenRightClickPopupFile(const FileExplorerItem& fileExplorerItem, const bool itemSelected, const std::string& id, const int itemIndex);

	/**
	* Check if an item is being dragged, and apply drag behavior if needed
	*/
	void CheckItemDrag(const FileExplorerItem& fileExplorerItem, std::shared_ptr<Texture>& iconTexture, const float iconSize, const std::string& itemName);

	/**
	* Rename the selected file
	*/
	void Rename();

	/**
	* Open an Item if the item supports this
	*/
	void OpenItem(const FileExplorerItem& item);

	bool fileHovered = false;
	std::string renamingString = "";

	std::shared_ptr<FileReference> fileToRename = nullptr;
	std::shared_ptr<ProjectDirectory> directoryToRename = nullptr;
	bool ignoreClose = false;
	bool focusSet = false;
	bool cancelNextClick = false;
	bool firstClickedInWindow = false;
};

