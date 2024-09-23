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
#include <engine/debug/stack_debug_object.h>

int Performance::s_drawCallCount = 0;
int Performance::s_drawTriangleCount = 0;
int Performance::s_updatedMaterialCount = 0;
std::unordered_map<std::string, ProfilerCategory*> Performance::s_profilerCategories;
std::unordered_map<size_t, std::vector<ScopTimerResult>> Performance::s_scopProfilerList;
std::unordered_map<size_t, std::string> Performance::s_scopProfilerNames;

int Performance::s_tickCount = 0;
float Performance::s_averageCoolDown = 0;
int Performance::s_lastDrawCallCount = 0;
int Performance::s_lastDrawTriangleCount = 0;

MemoryTracker* Performance::s_gameObjectMemoryTracker = nullptr;
MemoryTracker* Performance::s_meshDataMemoryTracker = nullptr;
MemoryTracker* Performance::s_textureMemoryTracker = nullptr;
uint32_t Performance::s_benchmarkScopeLevel = 0;

#pragma region Update values

void Performance::Init()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);
	Debug::Print("-------- Profiler initiated --------", true);

#if defined(DEBUG)
	s_gameObjectMemoryTracker = new MemoryTracker("GameObjects");
	s_meshDataMemoryTracker = new MemoryTracker("Mesh Data");
	s_textureMemoryTracker = new MemoryTracker("Textures");
#endif
}

void Performance::ResetCounters()
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);
	s_lastDrawCallCount = s_drawCallCount;
	s_drawCallCount = 0;
	s_lastDrawTriangleCount = s_drawTriangleCount;
	s_drawTriangleCount = 0;

	s_updatedMaterialCount = 0;
	ResetProfiler();
}

void Performance::AddDrawCall()
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);
	s_drawCallCount++;
}

void Performance::AddDrawTriangles(int count)
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);
	s_drawTriangleCount+= count;
}

void Performance::AddMaterialUpdate()
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);
	s_updatedMaterialCount++;
}

#pragma endregion

#pragma region Getters

int Performance::GetDrawCallCount()
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);
	return s_lastDrawCallCount;
}

int Performance::GetDrawTrianglesCount()
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);
	return s_lastDrawTriangleCount;
}

int Performance::GetUpdatedMaterialCount()
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);
	return s_updatedMaterialCount;
}

void Performance::Update()
{
	STACK_DEBUG_OBJECT(STACK_LOW_PRIORITY);
	if (EngineSettings::values.useProfiler)
	{
		s_tickCount++;

		// Update profiler average values
		s_averageCoolDown += Time::GetUnscaledDeltaTime();
		if (s_averageCoolDown >= 1)
		{
			for (const auto& categoryKV : Performance::s_profilerCategories)
			{
				for (const auto& profilerValueKV : categoryKV.second->profilerList)
				{
					profilerValueKV.second->average = profilerValueKV.second->addedValue / s_tickCount;
					profilerValueKV.second->addedValue = 0;
				}
			}
			s_averageCoolDown = 0;
			s_tickCount = 0;
		}
		ResetCounters();
	}
}

size_t Performance::RegisterScopProfiler(const std::string& name, size_t hash)
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);
	s_scopProfilerNames[hash] = name;

	return hash;
}

void Performance::ResetProfiler()
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);

	s_scopProfilerList.clear();

	for (const auto& categoryKV : Performance::s_profilerCategories)
	{
		for (const auto& profilerValueKV : categoryKV.second->profilerList)
		{
			profilerValueKV.second->ResetValue();
		}
	}
}

#pragma endregion
