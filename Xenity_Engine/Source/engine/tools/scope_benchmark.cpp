#include "scope_benchmark.h"

#include <chrono>

#include <engine/debug/performance.h>

#if defined(__PSP__)
#include <psptypes.h>
#include <psprtc.h>
#endif

ScopeBenchmark::ScopeBenchmark(const size_t hash) : m_hash(hash), m_scopeLevel(Performance::s_benchmarkScopeLevel)
{
#if defined(__PSP__)
	sceRtcGetCurrentTick(&m_start);
#else
	m_start = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count();
#endif
	Performance::s_benchmarkScopeLevel++;
}

ScopeBenchmark::~ScopeBenchmark()
{
#if defined(__PSP__)
	uint64_t end;
	sceRtcGetCurrentTick(&end);
#else
	const uint64_t end = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count();
#endif
	Performance::s_benchmarkScopeLevel--;
	Performance::s_scopProfilerList[m_hash].push_back({ m_start , end , m_scopeLevel });
}
