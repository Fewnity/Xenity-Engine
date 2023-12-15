#pragma once

#include "menu.h"
#include <string>

class EngineSettingsMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;
private:
	bool DrawSelectFolderButton(std::string& path);
	bool DrawCompilerOptions();
};

