#pragma once
#include <engine/api.h>

#include "benchmark.h"
#include <string>

class ProfilerValue;

class API ProfilerBenchmark
{
public:
	ProfilerBenchmark(const std::string& category, const std::string& name);
	~ProfilerBenchmark();

	/**
	* @brief Start the benchmark
	*/
	void Start();

	/**
	* @brief Add the elapsed time since the last Start call to the profiler
	*/
	void Stop();

private:
	Benchmark* bench = new Benchmark();

	ProfilerValue* profilerValue = nullptr;
};

