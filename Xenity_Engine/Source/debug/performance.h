#pragma once

#include <fstream>
#include <string>
#include <iostream>

class Performance
{
public:
	static void ResetCounters();
	static void AddDrawCall();
	static void AddMaterialUpdate();
	static int GetDrawCallCount();
	static int GetUpdatedMaterialCount();

private:
	static int drawCallCount;
	static int updatedMaterialCount;
};