#include "scope_benchmark.h"

#include <chrono>

#include <engine/debug/performance.h>

using namespace std::chrono;

ScopeBenchmark::ScopeBenchmark(const std::string& classAndFunction)
{
	m_name = classAndFunction;
	m_start = time_point_cast<microseconds>(high_resolution_clock::now()).time_since_epoch().count();
	m_scopeLevel = Performance::s_benchmarkScopeLevel;
	Performance::s_benchmarkScopeLevel++;
}

ScopeBenchmark::~ScopeBenchmark()
{
	const uint64_t end = time_point_cast<microseconds>(high_resolution_clock::now()).time_since_epoch().count();

	Performance::s_benchmarkScopeLevel--;
	Performance::s_scopProfilerList[m_name].push_back({ m_start , end , m_scopeLevel });
}
