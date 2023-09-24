#pragma once

#include "menu.h"

class ProfilerMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;
private:
	float nextFpsUpdate = 0;
	float lastFps = 0;
};

