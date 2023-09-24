#pragma once

#include "menu.h"

class HierarchyMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;
	bool isFocused = false;
};

