// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "profiler_benchmark.h"

#include <engine/debug/performance.h>
#include <engine/engine_settings.h>
#include <engine/assertions/assertions.h>

ProfilerBenchmark::ProfilerBenchmark(const std::string& category, const std::string& name)
{
	XASSERT(!category.empty(), "[ProfilerBenchmark::ProfilerBenchmark] category is empty");
	XASSERT(!name.empty(), "[ProfilerBenchmark::ProfilerBenchmark] name is empty");

#if defined(EDITOR) && defined(DEBUG)
	//If the profiler is new, created a new one
	if (Performance::profilerCategories.count(category) == 0)
	{
		Performance::profilerCategories[category] = new ProfilerCategory();
	}
	if (Performance::profilerCategories[category]->profilerList.count(name) == 0)
	{
		Performance::profilerCategories[category]->profilerList[name] = new ProfilerValue();
	}
	m_profilerValue = Performance::profilerCategories[category]->profilerList[name];
#endif
}

ProfilerBenchmark::~ProfilerBenchmark()
{
	delete m_bench;
}

void ProfilerBenchmark::Start()
{
#if defined(EDITOR)&& defined(DEBUG)
	if (EngineSettings::values.useProfiler)
		m_bench->Start();
#endif
}

void ProfilerBenchmark::Stop()
{
#if defined(EDITOR)&& defined(DEBUG)
	if (EngineSettings::values.useProfiler)
	{
		m_bench->Stop();
		m_profilerValue->AddValue(m_bench->GetMicroSeconds());
	}
#endif
}
