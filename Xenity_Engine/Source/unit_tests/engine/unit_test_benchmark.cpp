// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "../unit_test_manager.h"

#include <engine/debug/debug.h>
#include <engine/tools/benchmark.h>

TestResult BenchmarkTest::Start(std::string& errorOut)
{
	BEGIN_TEST();

	Benchmark benchmark;

	EXPECT_EQUALS(benchmark.GetMicroSeconds(), 0, "Benchmark not initialized correctly (GetMicroSeconds)");
	EXPECT_EQUALS(benchmark.GetMilliseconds(), 0, "Benchmark not initialized correctly (GetMilliseconds)");
	EXPECT_EQUALS(benchmark.GetSeconds(), 0, "Benchmark not initialized correctly (GetSeconds)");

	benchmark.Start();

	// Do some work
	std::string emptyString = "";
	for (size_t i = 0; i < 1000; i++)
	{
		emptyString += " ";
	}
	emptyString.clear();
	errorOut += emptyString; // To avoid optimization

	benchmark.Stop();

	uint64_t microSeconds = benchmark.GetMicroSeconds();

	EXPECT_NOT_EQUALS(microSeconds == 0, true, "Benchmark not correctly recording");

	benchmark.m_time = 56000000;

	microSeconds = benchmark.GetMicroSeconds();
	const uint64_t milliseconds = benchmark.GetMilliseconds();
	const float seconds = benchmark.GetSeconds();

	EXPECT_EQUALS(microSeconds, 56000000, "Benchmark not correctly converting to milliseconds");
	EXPECT_EQUALS(milliseconds, microSeconds / 1000, "Benchmark not correctly converting to milliseconds");
	EXPECT_EQUALS(seconds, microSeconds / 1000000.0f, "Benchmark not correctly converting to seconds");

	benchmark.Reset();

	EXPECT_EQUALS(benchmark.GetMicroSeconds(), 0, "Benchmark hasn't been reset (GetMicroSeconds)");
	EXPECT_EQUALS(benchmark.GetMilliseconds(), 0, "Benchmark hasn't been reset (GetMilliseconds)");
	EXPECT_EQUALS(benchmark.GetSeconds(), 0, "Benchmark hasn't been reset (GetSeconds)");

	END_TEST();
}