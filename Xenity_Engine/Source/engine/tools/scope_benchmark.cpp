#include "scope_benchmark.h"

#include <engine/debug/performance.h>
#include <chrono>

using namespace std::chrono;

ScopeBenchmark::ScopeBenchmark(const std::string& classAndFunction)
{
	name = classAndFunction;
	std::chrono::time_point<std::chrono::high_resolution_clock> start_point = high_resolution_clock::now();
	//start = high_resolution_clock::now();
	start = time_point_cast<microseconds>(start_point).time_since_epoch().count();
	scopeLevel = Performance::benchmarkScopeLevel;
	Performance::benchmarkScopeLevel++;
}

ScopeBenchmark::~ScopeBenchmark()
{
	time_point<high_resolution_clock> end_point = high_resolution_clock::now();
	//time_point_cast<microseconds>(start).time_since_epoch().count();
	uint64_t end = time_point_cast<microseconds>(end_point).time_since_epoch().count();

	ScopTimerResult result;
	result.start = start;
	result.end = end;
	result.level = scopeLevel;
	Performance::benchmarkScopeLevel--;
	Performance::scopProfilerList[name].push_back(result);
}
