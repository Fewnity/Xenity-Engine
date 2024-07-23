#include "scope_benchmark.h"

#include <engine/debug/performance.h>
#include <chrono>

using namespace std::chrono;

ScopeBenchmark::ScopeBenchmark(const std::string& classAndFunction)
{
	name = classAndFunction;
	start = time_point_cast<microseconds>(high_resolution_clock::now()).time_since_epoch().count();
	scopeLevel = Performance::benchmarkScopeLevel;
	Performance::benchmarkScopeLevel++;
}

ScopeBenchmark::~ScopeBenchmark()
{
	const uint64_t end = time_point_cast<microseconds>(high_resolution_clock::now()).time_since_epoch().count();

	Performance::benchmarkScopeLevel--;
	Performance::scopProfilerList[name].push_back({ start , end , scopeLevel });
}
