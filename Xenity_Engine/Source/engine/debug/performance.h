#pragma once

/**
 * [Internal]
 */

#include <fstream>
#include <string>
#include <iostream>
#include <unordered_map>

class ProfilerValue
{
public:

	/**
	* Add a time value to the profiler
	*/
	void AddValue(long long v)
	{
		value += v;
		addedValue += v;
	}

	/**
	* Reset the value
	*/
	void ResetValue()
	{
		lastValue = value;
		value = 0;
	}

	/**
	* Get the value
	*/
	long long GetValue()
	{
		return lastValue;
	}

	/**
	* Set the last value
	*/
	void SetLastValue(long long v)
	{
		lastValue = v;
	}

	long long average = 0;
	long long addedValue = 0;

private:
	long long lastValue = 0;
	long long value = 0;
};

struct ProfilerCategory
{
	std::unordered_map<std::string, ProfilerValue*> profilerList;
};

class Performance
{
public:

	/**
	* Init profiler
	*/
	static void Init();

	/**
	* Reset counters and profiler (To call every frame)
	*/
	static void ResetCounters();

	/**
	* Add one to the draw call counter
	*/
	static void AddDrawCall();

	/**
	* Add a number of triangles to the draw triangle counter
	* @param count Number of triangles to add
	*/
	static void AddDrawTriangles(int count);

	/**
	* Add one to the updated material counter
	*/
	static void AddMaterialUpdate();

	/**
	* Get draw call count
	*/
	static int GetDrawCallCount();

	/**
	* Get draw triangle count
	*/
	static int GetDrawTrianglesCount();

	/**
	* Get updated material count
	*/
	static int GetUpdatedMaterialCount();

	/**
	* Update the profiler (To call every frame)
	*/
	static void Update();

	static std::unordered_map<std::string, ProfilerCategory*> profilerCategories;
private:

	/**
	* Reset profiler
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