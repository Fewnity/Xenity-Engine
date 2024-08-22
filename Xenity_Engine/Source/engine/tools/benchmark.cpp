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
	m_start_point = high_resolution_clock::now();
#elif defined(__PSP__)
	sceRtcGetCurrentTick(&m_startTick);
#endif
}

/// <summary>
/// Stop the benchmark timer
/// </summary>
void Benchmark::Stop()
{
#if defined(_WIN32) || defined(_WIN64) || defined(__vita__) || defined(__LINUX__)
	m_end_point = high_resolution_clock::now();

	const long long start = time_point_cast<microseconds>(m_start_point).time_since_epoch().count();
	const long long end = time_point_cast<microseconds>(m_end_point).time_since_epoch().count();

	m_time = end - start;
#elif defined(__PSP__)
	sceRtcGetCurrentTick(&m_endTick);

	m_time = m_endTick - m_startTick;
#endif
}