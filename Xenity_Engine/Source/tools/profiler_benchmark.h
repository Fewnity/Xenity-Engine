#pragma once

#include "benchmark.h"
#include <string>

class ProfilerBenchmark
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

