#include "performance.h"
#include "../time/time.h"
#include "../debug/debug.h"

int Performance::drawCallCount = 0;
int Performance::updatedMaterialCount = 0;
std::unordered_map<std::string, ProfilerValue *> Performance::profilerList;

bool Performance::profilerEnabled = true;
int Performance::tickCount = 0;
float Performance::averageCoolDown = 0;
int Performance::LastDrawCallCount = 0;

#pragma region Update values

/**
 * @brief Init the profilier
 *
 */
void Performance::Init()
{
	Debug::Print("-------- Profiler initiated --------");
}

/**
 * @brief Reset all counters
 *
 */
void Performance::ResetCounters()
{
	LastDrawCallCount = drawCallCount;
	drawCallCount = 0;
	updatedMaterialCount = 0;
	ResetProfiler();
}

/**
 * @brief Add one to the draw call counter
 *
 */
void Performance::AddDrawCall()
{
	drawCallCount++;
}

/**
 * @brief Add one to the updated material counter
 *
 */
void Performance::AddMaterialUpdate()
{
	updatedMaterialCount++;
}

#pragma endregion

#pragma region Getters

/**
 * @brief Get the nomber of drawcall during this frame
 *
 * @return int
 */
int Performance::GetDrawCallCount()
{
	return LastDrawCallCount;
}

/**
 * @brief Get the nomber of updated material during this frame
 *
 * @return int
 */
int Performance::GetUpdatedMaterialCount()
{
	return updatedMaterialCount;
}

/**
 * @brief Update profiler
 *
 */
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

/**
 * @brief Enable or disable the profiler
 *
 * @param enable
 */
void Performance::EnableProfiler(bool enable)
{
	profilerEnabled = enable;
}

/**
 * @brief Get if the profiler is enabled or disabled
 *
 * @return true
 * @return false
 */
bool Performance::IsProfilerEnabled()
{
	return profilerEnabled;
}

/**
 * @brief Reset profiler values
 *
 */
void Performance::ResetProfiler()
{
	for (auto &kv : Performance::profilerList)
	{
		kv.second->ResetValue();
	}
}

#pragma endregion
