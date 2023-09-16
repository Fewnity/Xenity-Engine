#include "performance.h"
#include "../time/time.h"
#include "../debug/debug.h"
#include "../engine_settings.h"

int Performance::drawCallCount = 0;
int Performance::updatedMaterialCount = 0;
std::unordered_map<std::string, ProfilerCategory*> Performance::profilerCategories;

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

		// Update profiler average values
		averageCoolDown += Time::GetUnscaledDeltaTime();
		if (averageCoolDown >= 1)
		{
			for (auto& categoryKV : Performance::profilerCategories)
			{
				for (auto& profilerValueKV : categoryKV.second->profilerList)
				{
					profilerValueKV.second->average = profilerValueKV.second->addedValue / tickCount;
					profilerValueKV.second->addedValue = 0;
				}
			}
			averageCoolDown = 0;
			tickCount = 0;
		}
	}
}

void Performance::ResetProfiler()
{
	for (auto& categoryKV : Performance::profilerCategories)
	{
		for (auto& profilerValueKV : categoryKV.second->profilerList)
		{
			profilerValueKV.second->ResetValue();
		}
	}
}

#pragma endregion
