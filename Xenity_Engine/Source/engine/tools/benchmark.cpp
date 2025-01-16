// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "benchmark.h"

#if defined(__PSP__)
#include <psptypes.h>
#include <psprtc.h>
#endif

/// <summary>
/// Start the benchmark timer
/// </summary>
void Benchmark::Start()
{
#if defined(_WIN32) || defined(_WIN64) || defined(__vita__) || defined(__LINUX__)
	m_start_point = std::chrono::high_resolution_clock::now();
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
	m_end_point = std::chrono::high_resolution_clock::now();

	const int64_t start = std::chrono::time_point_cast<std::chrono::microseconds>(m_start_point).time_since_epoch().count();
	const int64_t end = std::chrono::time_point_cast<std::chrono::microseconds>(m_end_point).time_since_epoch().count();

	m_time = end - start;
#elif defined(__PSP__)
	sceRtcGetCurrentTick(&m_endTick);

	m_time = m_endTick - m_startTick;
#endif
}