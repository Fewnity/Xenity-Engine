// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <string>

#include <engine/api.h>
#include "benchmark.h"

class ProfilerValue;

/**
* @brief Class to use to benchmark the engine and send the result to the profiler
*/
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
	Benchmark* m_bench = new Benchmark();

	ProfilerValue* m_profilerValue = nullptr;
};

