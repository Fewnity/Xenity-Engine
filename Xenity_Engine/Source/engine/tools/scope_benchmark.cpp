#include "scope_benchmark.h"

#include <chrono>

#include <engine/debug/performance.h>

ScopeBenchmark::ScopeBenchmark(const size_t hash) : m_hash(hash), m_scopeLevel(Performance::s_benchmarkScopeLevel)
{
	m_start = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count();
	Performance::s_benchmarkScopeLevel++;
}

ScopeBenchmark::~ScopeBenchmark()
{
	const uint64_t end = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count();

	Performance::s_benchmarkScopeLevel--;
	Performance::s_scopProfilerList[m_hash].push_back({ m_start , end , m_scopeLevel });
}
