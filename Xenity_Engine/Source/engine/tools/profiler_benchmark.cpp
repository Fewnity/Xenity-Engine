#include "profiler_benchmark.h"

#include "../debug/performance.h"
#include "../engine_settings.h"

ProfilerBenchmark::ProfilerBenchmark(std::string name)
{
	if (Performance::profilerList.count(name) == 0) 
	{
		Performance::profilerList[name] = new ProfilerValue();
	}
	profilerValue = Performance::profilerList[name];
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
