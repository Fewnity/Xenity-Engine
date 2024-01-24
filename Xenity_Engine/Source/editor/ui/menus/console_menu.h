#pragma once

#include <memory>
#include "menu.h"

class ConsoleMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;

private:
	bool consoleMode = true;
};

