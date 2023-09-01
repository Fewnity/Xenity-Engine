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
	void Init();
	void Draw();
private:
	void DrawExplorerItem(float iconSize, int& currentCol, int colCount, bool isFile, int offset, FileExplorerItem item, std::string itemName);
	int CheckOpenRightClickPopupFile(FileExplorerItem& fileExplorerItem, bool itemSelected, std::string id);
	bool fileHovered = false;
	void OpenItem(FileExplorerItem& item);
};

