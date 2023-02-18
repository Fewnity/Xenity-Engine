#include "performance.h"

int Performance::drawCallCount = 0;
int Performance::updatedMaterialCount = 0;

#pragma region Update values

void Performance::ResetCounters()
{
	drawCallCount = 0;
	updatedMaterialCount = 0;
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
	return drawCallCount;
}

int Performance::GetUpdatedMaterialCount()
{
	return updatedMaterialCount;
}

#pragma endregion


