#include "profiler_benchmark.h"

#include "../debug/performance.h"
#include "../engine_settings.h"

ProfilerBenchmark::ProfilerBenchmark(std::string category, std::string name)
{
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
}

ProfilerBenchmark::~ProfilerBenchmark()
{
	delete bench;
}

void ProfilerBenchmark::Start()
{
	if (EngineSettings::useProfiler)
		bench->Start();
}

void ProfilerBenchmark::Stop()
{
	bench->Stop();
	if (EngineSettings::useProfiler)
	{
		profilerValue->AddValue(bench->GetMicroSeconds());
	}
}
