#pragma once

#include "menu.h"
#include <memory>

class SkyBox;

class LightingMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;
private:
};

