// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Grégory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include "menu.h"
#define FPS_HISTORY_SIZE 400

class ProfilerMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;

private:
	/**
	* Read current fps and store it in the history
	*/
	void UpdateFpsCounter();

	/**
	* Draw all profiler benchmarks
	*/
	void DrawProfilerBenchmarks();

	/**
	* Draw files reference count list
	*/
	void DrawFilesList();

	float fpsAVG = 0;
	float nextFpsUpdate = 0;
	float lastFps = 0;
	float fpsHistory[FPS_HISTORY_SIZE] = { 0 };
	bool counterInitialised = false;
};

