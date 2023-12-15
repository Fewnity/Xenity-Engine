#pragma once

#include "menu.h"
#define FPS_HISTORY_SIZE 400

class ProfilerMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;

private:
	void UpdateFpsCounter();
	void DrawProfilerBenchmarks();
	void DrawFilesList();

	float fpsAVG = 0;
	float nextFpsUpdate = 0;
	float lastFps = 0;
	float fpsHistory[FPS_HISTORY_SIZE] = { 0 };
	bool counterInitialised = false;
};

