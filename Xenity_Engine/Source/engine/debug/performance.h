#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include <fstream>
#include <string>
#include <iostream>
#include <unordered_map>

class API ProfilerValue
{
public:
	void AddValue(long long v)
	{
		value += v;
		addedValue += v;
	}
	void ResetValue()
	{
		lastValue = value;
		value = 0;
	}
	long long GetValue()
	{
		return lastValue;
	}
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

class API ProfilerCategory 
{
public:
	std::unordered_map<std::string, ProfilerValue*> profilerList;
};

class API Performance
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
	* Add one to the updated material counter
	*/
	static void AddMaterialUpdate();
	
	/**
	* Get draw call count
	*/
	static int GetDrawCallCount();
	
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
	static int LastDrawCallCount;
	static int updatedMaterialCount;

	static int tickCount;
	static float averageCoolDown;
};