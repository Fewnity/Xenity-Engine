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

class API ProfilerBenchmark
{
public:
	ProfilerBenchmark(std::string name);
	~ProfilerBenchmark();
	void Start();
	void Stop();
private:
	Benchmark* bench = new Benchmark();

	class ProfilerValue* profilerValue = nullptr;
};

