#pragma once

#include "menu.h"
#include <vector>

class ProjectListItem;

class SelectProjectMenu : public Menu
{
public:
	SelectProjectMenu();
	void Init() override;
	void Draw() override;
private:
	void OnLoadButtonClick();
	void DrawProjectsList();
	std::vector<ProjectListItem> projectsList;
};

