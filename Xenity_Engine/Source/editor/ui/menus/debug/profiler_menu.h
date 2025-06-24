// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <editor/ui/menus/menu.h>
#define FPS_HISTORY_SIZE 400
#define USED_MEMORY_HISTORY_SIZE 1000
#define USED_VIDE_MEMORY_HISTORY_SIZE 1000

class ClassicProfilerItem
{
public:
	ClassicProfilerItem(const std::string& _name) : name(_name) {}
	const std::string& name;
	uint32_t totalTime = 0;
	uint32_t callCountInFrame = 0;
};

class TimelineItem 
{
public:
	TimelineItem(const std::string& _name) : name(_name) {}
	const std::string& name;
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

	void UpdateMemoryCounter();

	/**
	* Draw memory stats
	*/
	void DrawMemoryStats();

	/**
	* Draw all profiler benchmarks
	*/
	void DrawProfilerGraph();

	void CreateTimelineItems();

	std::vector<TimelineItem> m_timelineItems;
	std::vector<ClassicProfilerItem> m_classicProfilerItems;
	uint64_t m_lastStartTime;
	uint64_t m_lastEndTime;
	uint32_t m_lastMaxLevel;
	uint32_t m_selectedProfilingRow = 0;
	uint32_t m_lastFrame = 0;
	float m_fpsAVG = 0;
	float m_nextFpsUpdate = 0;
	float m_lastFps = 0;
	float m_fpsHistory[FPS_HISTORY_SIZE] = { 0 };
	float m_usedMemoryHistory[USED_MEMORY_HISTORY_SIZE] = { 0 };
	float m_usedVideoMemoryHistory[USED_VIDE_MEMORY_HISTORY_SIZE] = { 0 };
	bool m_counterInitialised = false;
	bool m_isPaused = false;
};

