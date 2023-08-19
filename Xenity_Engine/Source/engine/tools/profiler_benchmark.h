#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include "benchmark.h"
#include <string>

class ProfilerValue;

class API ProfilerBenchmark
{
public:
	ProfilerBenchmark(std::string name);
	~ProfilerBenchmark();

	/**
	* Start the benchmark
	*/
	void Start();

	/**
	* Add the elapsed time since the last Start call to the profiler
	*/
	void Stop();

private:
	Benchmark* bench = new Benchmark();

	ProfilerValue* profilerValue = nullptr;
};

