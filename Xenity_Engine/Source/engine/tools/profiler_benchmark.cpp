#include "profiler_benchmark.h"

#include <engine/debug/performance.h>
#include <engine/engine_settings.h>
#include <engine/assertions/assertions.h>

ProfilerBenchmark::ProfilerBenchmark(const std::string& category, const std::string& name)
{
	DXASSERT(!category.empty(), "[ProfilerBenchmark::ProfilerBenchmark] category is empty")
	DXASSERT(!name.empty(), "[ProfilerBenchmark::ProfilerBenchmark] name is empty")

#if defined(EDITOR)
	//If the profiler is new, created a new one
	if (Performance::profilerCategories.count(category) == 0)
	{
		Performance::profilerCategories[category] = new ProfilerCategory();
	}
	if (Performance::profilerCategories[category]->profilerList.count(name) == 0)
	{
		Performance::profilerCategories[category]->profilerList[name] = new ProfilerValue();
	}
	profilerValue = Performance::profilerCategories[category]->profilerList[name];
#endif
}

ProfilerBenchmark::~ProfilerBenchmark()
{
	delete bench;
}

void ProfilerBenchmark::Start()
{
#if defined(EDITOR)
	if (EngineSettings::values.useProfiler)
		bench->Start();
#endif
}

void ProfilerBenchmark::Stop()
{
#if defined(EDITOR)
	if (EngineSettings::values.useProfiler)
	{
		bench->Stop();
		profilerValue->AddValue(bench->GetMicroSeconds());
	}
#endif
}
