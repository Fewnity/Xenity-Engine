#pragma once

#include "menu.h"
#include <string>
#include <memory>

class FileReference;
class ProjectDirectory;

class FileExplorerItem 
{
public:
	std::shared_ptr<FileReference> file = nullptr;
	ProjectDirectory* directory = nullptr;
};

class FileExplorerMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;
private:
	void DrawExplorerItem(float iconSize, int& currentCol, int colCount, bool isFile, int offset, FileExplorerItem& item, int itemIndex);
	int CheckOpenRightClickPopupFile(FileExplorerItem& fileExplorerItem, bool itemSelected, const std::string& id, int itemIndex);
	bool fileHovered = false;
	void OpenItem(FileExplorerItem& item);
	std::string renamingString = "";

	std::shared_ptr<FileReference> fileToRename = nullptr;
	ProjectDirectory* directoryToRename = nullptr;
	bool ignoreClose = false;
	bool focusSet = false;
	void Rename();
};

