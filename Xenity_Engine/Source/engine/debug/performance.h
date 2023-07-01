#pragma once

#include <fstream>
#include <string>
#include <iostream>
#include <unordered_map>

class ProfilerValue
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

class Performance
{
public:
	static void Init();
	static void ResetCounters();
	static void AddDrawCall();
	static void AddMaterialUpdate();
	static int GetDrawCallCount();
	static int GetUpdatedMaterialCount();
	static std::unordered_map<std::string, ProfilerValue *> profilerList;
	static void Update();

private:
	static void ResetProfiler();
	static int drawCallCount;
	static int LastDrawCallCount;
	static int updatedMaterialCount;

	static int tickCount;
	static float averageCoolDown;
};