// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "profiler_benchmark.h"

#include <engine/debug/performance.h>
#include <engine/engine_settings.h>
#include <engine/assertions/assertions.h>
#include <engine/constants.h>

ProfilerBenchmark::ProfilerBenchmark(const std::string& category, const std::string& name)
{
	XASSERT(!category.empty(), "[ProfilerBenchmark::ProfilerBenchmark] category is empty");
	XASSERT(!name.empty(), "[ProfilerBenchmark::ProfilerBenchmark] name is empty");

#if defined(USE_PROFILER)
	//If the profiler is new, created a new one
	if (Performance::s_profilerCategories.count(category) == 0)
	{
		Performance::s_profilerCategories[category] = new ProfilerCategory();
	}
	if (Performance::s_profilerCategories[category]->profilerList.count(name) == 0)
	{
		Performance::s_profilerCategories[category]->profilerList[name] = new ProfilerValue();
	}
	m_profilerValue = Performance::s_profilerCategories[category]->profilerList[name];
#endif
}

ProfilerBenchmark::~ProfilerBenchmark()
{
	delete m_bench;
}

void ProfilerBenchmark::Start()
{
#if defined(USE_PROFILER)
	if (EngineSettings::values.useProfiler)
		m_bench->Start();
#endif
}

void ProfilerBenchmark::Stop()
{
#if defined(USE_PROFILER)
	if (EngineSettings::values.useProfiler)
	{
		m_bench->Stop();
		m_profilerValue->AddValue(m_bench->GetMicroSeconds());
	}
#endif
}
