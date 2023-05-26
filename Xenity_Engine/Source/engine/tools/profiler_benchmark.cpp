#include "profiler_benchmark.h"

#include "../debug/performance.h"

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
	if (Performance::IsProfilerEnabled())
		bench->Start();
}

void ProfilerBenchmark::Stop()
{
	bench->Stop();
	if (Performance::IsProfilerEnabled())
	{
		profilerValue->AddValue(bench->GetMicroSeconds());
	}
}
