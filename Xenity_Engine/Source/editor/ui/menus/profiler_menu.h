// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include "menu.h"
#define FPS_HISTORY_SIZE 400

class TimelineItem 
{
public:
	std::string name;
	uint64_t start;
	uint64_t end;
	uint32_t level;
};

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
	* Draw memory stats
	*/
	void DrawMemoryStats();

	/**
	* Draw files reference count list
	*/
	void DrawFilesList();

	float fpsAVG = 0;
	float nextFpsUpdate = 0;
	float lastFps = 0;
	float fpsHistory[FPS_HISTORY_SIZE] = { 0 };
	bool counterInitialised = false;
	std::vector<TimelineItem> timelineItems;
	bool isPaused = false;
	uint64_t lastStartTime;
	uint64_t lastEndTime;
};

