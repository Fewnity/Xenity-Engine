#pragma once

#include "menu.h"

class ProfilerMenu : public Menu
{
public:
	void Init();
	void Draw();
private:
	float nextFpsUpdate = 0;
	float lastFps = 0;
};

