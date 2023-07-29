#pragma once

#include "menu.h"
#include <string>

class FileReference;
class ProjectDirectory;

class FileExplorerItem 
{
public:
	FileReference* file = nullptr;
	ProjectDirectory* directory = nullptr;
};

class FileExplorerMenu : public Menu
{
public:
	void Init();
	void Draw();
private:
	void DrawExplorerItem(float iconSize, int& currentCol, int colCount, bool isFile, int offset, FileExplorerItem item, std::string itemName);
};

