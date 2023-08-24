#include "performance.h"
#include "../time/time.h"
#include "../debug/debug.h"
#include "../engine_settings.h"

int Performance::drawCallCount = 0;
int Performance::updatedMaterialCount = 0;
std::unordered_map<std::string, ProfilerValue *> Performance::profilerList;

int Performance::tickCount = 0;
float Performance::averageCoolDown = 0;
int Performance::LastDrawCallCount = 0;

#pragma region Update values

void Performance::Init()
{
	Debug::Print("-------- Profiler initiated --------");
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
	if (EngineSettings::useProfiler)
	{
		tickCount++;
		averageCoolDown += Time::GetUnscaledDeltaTime();
		if (averageCoolDown >= 1)
		{
			for (auto& kv : Performance::profilerList)
			{
				kv.second->average = kv.second->addedValue / tickCount;
				kv.second->addedValue = 0;
			}
			averageCoolDown = 0;
			tickCount = 0;
		}
	}
}

void Performance::ResetProfiler()
{
	for (auto &kv : Performance::profilerList)
	{
		kv.second->ResetValue();
	}
}

#pragma endregion
