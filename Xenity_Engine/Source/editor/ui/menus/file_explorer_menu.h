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
	static std::shared_ptr<Texture> GetItemIcon(FileExplorerItem& fileExplorerItem, bool isFile);

private:
	void DrawExplorerItem(float iconSize, int& currentCol, int colCount, bool isFile, float offset, FileExplorerItem& item, int itemIndex);
	int CheckOpenRightClickPopupFile(FileExplorerItem& fileExplorerItem, bool itemSelected, const std::string& id, int itemIndex);
	void CheckItemDrag(FileExplorerItem& fileExplorerItem, bool isFile, std::shared_ptr<Texture>& iconTexture, float iconSize, const std::string& itemName);


	bool fileHovered = false;
	void OpenItem(FileExplorerItem& item);
	std::string renamingString = "";

	std::shared_ptr<FileReference> fileToRename = nullptr;
	std::shared_ptr<ProjectDirectory> directoryToRename = nullptr;
	bool ignoreClose = false;
	bool focusSet = false;
	bool cancelNextClick = false;
	bool firstClickedInWindow = false;
	void Rename();
};

