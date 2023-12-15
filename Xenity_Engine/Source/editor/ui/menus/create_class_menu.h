#pragma once

#include "menu.h"
#include <memory>
#include <string>

class CreateClassMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;
	void Reset();
	void SetFolderPath(const std::string& path);

private:
	std::string className = "";
	std::string fileName = "";
	std::string folderPath = "";
	bool fileNameChanged = false;
	void SetFileNameFromClassName();
	void CreateFiles();
};

