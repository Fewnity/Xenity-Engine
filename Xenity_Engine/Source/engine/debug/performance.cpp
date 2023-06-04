#include "performance.h"
#include "../time/time.h"

int Performance::drawCallCount = 0;
int Performance::updatedMaterialCount = 0;
std::unordered_map<std::string, ProfilerValue *> Performance::profilerList;

bool Performance::profilerEnabled = true;
int Performance::tickCount = 0;
float Performance::averageCoolDown = 0;
int Performance::LastDrawCallCount = 0;

#pragma region Update values

void Performance::Init()
{
	profilerList = std::unordered_map<std::string, ProfilerValue *>();
}

void Performance::ResetCounters()
{
	LastDrawCallCount = drawCallCount;
	drawCallCount = 0;
	updatedMaterialCount = 0;
	ResetProfiler();
}

void Performance::AddDrawCall()
{
	drawCallCount++;
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

int Performance::GetUpdatedMaterialCount()
{
	return updatedMaterialCount;
}

void Performance::Update()
{
	tickCount++;
	averageCoolDown += Time::GetUnscaledDeltaTime();
	if (averageCoolDown >= 1)
	{
		for (auto &kv : Performance::profilerList)
		{
			kv.second->average = kv.second->addedValue / tickCount;
			kv.second->addedValue = 0;
		}
		averageCoolDown = 0;
		tickCount = 0;
	}
}

void Performance::EnableProfiler(bool enable)
{
	profilerEnabled = enable;
}

bool Performance::IsProfilerEnabled()
{
	return profilerEnabled;
}

void Performance::ResetProfiler()
{
	for (auto &kv : Performance::profilerList)
	{
		kv.second->ResetValue();
	}
}

#pragma endregion
