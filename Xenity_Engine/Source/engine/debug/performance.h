// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

/**
 * [Internal]
 */

#include <string>
#include <unordered_map>
#include <vector>

#include <engine/tools/scope_benchmark.h>

#if defined(EDITOR)
#define USE_PROFILER
#endif

#if defined(USE_PROFILER)
#define SCOPED_PROFILER(name, variableName) ScopeBenchmark variableName = ScopeBenchmark(name)
#else
#define SCOPED_PROFILER(name, variableName)
#endif

class MemoryTracker;

class ProfilerValue
{
public:

	/**
	* @brief Add a time value to the profiler
	*/
	inline void AddValue(uint64_t v)
	{
		value += v;
		addedValue += v;
	}

	/**
	* @brief Reset the value
	*/
	inline void ResetValue()
	{
		lastValue = value;
		value = 0;
	}

	/**
	* @brief Get the value
	*/
	inline long long GetValue()
	{
		return lastValue;
	}

	/**
	* @brief Set the last value
	*/
	inline void SetLastValue(uint64_t v)
	{
		lastValue = v;
	}

	uint64_t average = 0;
	uint64_t addedValue = 0;

private:
	uint64_t lastValue = 0;
	uint64_t value = 0;
};

struct ProfilerCategory
{
	std::unordered_map<std::string, ProfilerValue*> profilerList;
};

struct ScopTimerResult
{
	uint64_t start;
	uint64_t end;
	uint32_t level;
};

class Performance
{
public:

	/**
	* @brief Init profiler
	*/
	static void Init();

	/**
	* @brief Reset counters and profiler (To call every frame)
	*/
	static void ResetCounters();

	/**
	* @brief Add one to the draw call counter
	*/
	static void AddDrawCall();

	/**
	* @brief Add a number of triangles to the draw triangle counter
	* @param count Number of triangles to add
	*/
	static void AddDrawTriangles(int count);

	/**
	* @brief Add one to the updated material counter
	*/
	static void AddMaterialUpdate();

	/**
	* @brief Get draw call count
	*/
	static int GetDrawCallCount();

	/**
	* @brief Get draw triangle count
	*/
	static int GetDrawTrianglesCount();

	/**
	* @brief Get updated material count
	*/
	static int GetUpdatedMaterialCount();

	/**
	* @brief Update the profiler (To call every frame)
	*/
	static void Update();

	static std::unordered_map<std::string, ProfilerCategory*> profilerCategories;
	static std::unordered_map<std::string, std::vector<ScopTimerResult>> scopProfilerList;

	static MemoryTracker* gameObjectMemoryTracker;
	static MemoryTracker* meshDataMemoryTracker;
	static MemoryTracker* textureMemoryTracker;
	static uint32_t benchmarkScopeLevel;
private:

	/**
	* @brief Reset profiler
	*/
	static void ResetProfiler();

	static int drawCallCount;
	static int drawTriangleCount;
	static int LastDrawCallCount;
	static int LastDrawTriangleCount;
	static int updatedMaterialCount;

	static int tickCount;
	static float averageCoolDown;
};