// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "performance.h"

#include <engine/time/time.h>
#include <engine/debug/debug.h>
#include <engine/engine_settings.h>
#include "memory_tracker.h"

int Performance::drawCallCount = 0;
int Performance::drawTriangleCount = 0;
int Performance::updatedMaterialCount = 0;
std::unordered_map<std::string, ProfilerCategory*> Performance::profilerCategories;
std::unordered_map<std::string, std::vector<ScopTimerResult>> Performance::scopProfilerList;

int Performance::tickCount = 0;
float Performance::averageCoolDown = 0;
int Performance::LastDrawCallCount = 0;
int Performance::LastDrawTriangleCount = 0;

MemoryTracker* Performance::gameObjectMemoryTracker = nullptr;
MemoryTracker* Performance::meshDataMemoryTracker = nullptr;
MemoryTracker* Performance::textureMemoryTracker = nullptr;
uint32_t Performance::benchmarkScopeLevel = 0;

#pragma region Update values

void Performance::Init()
{
	Debug::Print("-------- Profiler initiated --------", true);

#if defined(DEBUG)
	gameObjectMemoryTracker = new MemoryTracker("GameObjects");
	meshDataMemoryTracker = new MemoryTracker("Mesh Data");
	textureMemoryTracker = new MemoryTracker("Textures");
#endif
}

void Performance::ResetCounters()
{
	LastDrawCallCount = drawCallCount;
	drawCallCount = 0;
	LastDrawTriangleCount = drawTriangleCount;
	drawTriangleCount = 0;

	updatedMaterialCount = 0;
	ResetProfiler();
}

void Performance::AddDrawCall()
{
	drawCallCount++;
}

void Performance::AddDrawTriangles(int count)
{
	drawTriangleCount+= count;
}

void Performance::AddMaterialUpdate()
{
	updatedMaterialCount++;
}

#pragma endregion

#pragma region Getters

int Performance::GetDrawCallCount()
{
	return LastDrawCallCount;
}

int Performance::GetDrawTrianglesCount()
{
	return LastDrawTriangleCount;
}

int Performance::GetUpdatedMaterialCount()
{
	return updatedMaterialCount;
}

void Performance::Update()
{
	if (EngineSettings::values.useProfiler)
	{
		tickCount++;

		// Update profiler average values
		averageCoolDown += Time::GetUnscaledDeltaTime();
		if (averageCoolDown >= 1)
		{
			for (const auto& categoryKV : Performance::profilerCategories)
			{
				for (const auto& profilerValueKV : categoryKV.second->profilerList)
				{
					profilerValueKV.second->average = profilerValueKV.second->addedValue / tickCount;
					profilerValueKV.second->addedValue = 0;
				}
			}
			averageCoolDown = 0;
			tickCount = 0;
		}
		ResetCounters();
	}
}

void Performance::ResetProfiler()
{
	scopProfilerList.clear();

	for (const auto& categoryKV : Performance::profilerCategories)
	{
		for (const auto& profilerValueKV : categoryKV.second->profilerList)
		{
			profilerValueKV.second->ResetValue();
		}
	}
}

#pragma endregion
