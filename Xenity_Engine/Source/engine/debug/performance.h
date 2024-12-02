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
#include <cstdint>
#include <functional>

#include <engine/tools/scope_benchmark.h>
#include <engine/constants.h>

#if defined(USE_PROFILER)
#define SCOPED_PROFILER(name, variableName) static const size_t hash##variableName = Performance::RegisterScopProfiler(name, std::hash<std::string>{}(name)); \
const ScopeBenchmark variableName = ScopeBenchmark(hash##variableName)
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

	static size_t RegisterScopProfiler(const std::string& name, size_t hash);

	static std::unordered_map<std::string, ProfilerCategory*> s_profilerCategories;
	static std::vector<std::unordered_map<uint64_t, std::vector<ScopTimerResult>>> s_scopProfilerList;
	static uint32_t s_currentProfilerFrame;
	static uint32_t s_currentFrame;
	static bool s_isPaused;
	static constexpr uint32_t s_maxProfilerFrameCount = 200;

	static std::unordered_map<uint64_t, std::string> s_scopProfilerNames; // Hash, Name

	static MemoryTracker* s_gameObjectMemoryTracker;
	static MemoryTracker* s_meshDataMemoryTracker;
	static MemoryTracker* s_textureMemoryTracker;
	static uint32_t s_benchmarkScopeLevel;

	static void SaveToBinary(const std::string& path);
	static void LoadFromBinary(const std::string& path);
private:

	/**
	* @brief Reset profiler
	*/
	static void ResetProfiler();

	static int s_drawCallCount;
	static int s_drawTriangleCount;
	static int s_lastDrawCallCount;
	static int s_lastDrawTriangleCount;
	static int s_updatedMaterialCount;

	static int s_tickCount;
	static float s_averageCoolDown;
};