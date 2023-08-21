#pragma once

#include "menu.h"
#include <string>

enum CreateProjectError 
{
	NO_ERROR,
	ERROR_UNKNOWN,
	ERROR_PROJECT_ALREADY_EXISTS,
	ERROR_EMPTY_NAME,
	ERROR_EMPTY_FOLDER,
};

class CreateProjectMenu : public Menu
{
public:
	void Init();
	void Draw();
private:
	std::string projectName;
	std::string projectParentDir;
	CreateProjectError createProjectError = NO_ERROR;
};

