#pragma once

#include <memory>
#include "menu.h"

class SkyBox;

class LightingMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;

private:
};

