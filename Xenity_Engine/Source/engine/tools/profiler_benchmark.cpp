#include "profiler_benchmark.h"

#include "../debug/performance.h"
#include "../engine_settings.h"

ProfilerBenchmark::ProfilerBenchmark(const std::string& category, const std::string& name)
{
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
	if (EngineSettings::useProfiler)
		bench->Start();
#endif
}

void ProfilerBenchmark::Stop()
{
#if defined(EDITOR)
	bench->Stop();
	if (EngineSettings::useProfiler)
	{
		profilerValue->AddValue(bench->GetMicroSeconds());
	}
#endif
}
