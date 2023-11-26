#pragma once

#include "menu.h"
#include <string>

class EngineSettingsMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;
	bool DrawSelectFolderButton(std::string& path);
};

