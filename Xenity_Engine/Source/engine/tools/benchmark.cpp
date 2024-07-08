// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "benchmark.h"

#if defined(__PSP__)
#include <psptypes.h>
#include <psprtc.h>
#endif

using namespace std::chrono;

/// <summary>
/// Start the benchmark timer
/// </summary>
void Benchmark::Start()
{
#if defined(_WIN32) || defined(_WIN64) || defined(__vita__) || defined(__LINUX__)
	start_point = high_resolution_clock::now();
#elif defined(__PSP__)
	sceRtcGetCurrentTick(&startTick);
#endif
}

/// <summary>
/// Stop the benchmark timer
/// </summary>
void Benchmark::Stop()
{
#if defined(_WIN32) || defined(_WIN64) || defined(__vita__) || defined(__LINUX__)
	end_point = high_resolution_clock::now();

	const long long start = time_point_cast<microseconds>(start_point).time_since_epoch().count();
	const long long end = time_point_cast<microseconds>(end_point).time_since_epoch().count();

	time = end - start;
#elif defined(__PSP__)
	sceRtcGetCurrentTick(&endTick);

	time = endTick - startTick;
#endif
}