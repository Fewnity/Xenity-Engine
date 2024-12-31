// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "time.h"

#include <chrono>
#if defined(_EE)
#include <timer.h>
#elif defined(__PSP__)
#include <psptypes.h>
#include <psprtc.h>
#elif defined(__vita__)
#include <psp2/rtc.h> 
#endif

#include <engine/tools/scope_benchmark.h>
#include <engine/debug/performance.h>
#include <engine/debug/debug.h>
#include <engine/debug/stack_debug_object.h>

float Time::s_timeScale = 1;
float Time::s_time = 0;
float Time::s_unscaledTime = 0;
float Time::s_deltaTime = 0;
float Time::s_unscaledDeltaTime = 0;

#if defined(__PSP__)
uint64_t lastTick;
uint64_t currentTick;
#elif defined(__vita__)
SceRtcTick lastTick;
SceRtcTick currentTick;
#elif defined(_EE)
uint64_t lastTick;
uint64_t currentTick;
#else
std::chrono::time_point<std::chrono::high_resolution_clock> start_point, end_point;
#endif

#pragma region Accessors

void Time::SetTimeScale(float _timeScale)
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);
	if (_timeScale < 0)
		_timeScale = 0;

	Time::s_timeScale = _timeScale;
}

#pragma endregion

void Time::Init()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);
	Reset();
	Debug::Print("-------- Time system initiated --------", true);
}

void Time::Reset()
{
#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
	start_point = std::chrono::high_resolution_clock::now();
	end_point = start_point;
#elif defined(__PSP__)
	sceRtcGetCurrentTick(&currentTick);
	lastTick = currentTick;
#elif defined(__vita__)
	sceRtcGetCurrentTick(&currentTick);
	lastTick = currentTick;
#endif
	s_time = 0;
	s_unscaledTime = 0;
}

void Time::UpdateTime()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	SCOPED_PROFILER("Time::UpdateTime", scopeBenchmark);
#if defined(__PSP__)
	sceRtcGetCurrentTick(&currentTick);
	const float tempDeltaTime = (currentTick - lastTick) / 1000000.0f;
	lastTick = currentTick;
#elif defined(__vita__)
	sceRtcGetCurrentTick(&currentTick);
	const float tempDeltaTime = (currentTick.tick - lastTick.tick) / 1000000.0f;
	lastTick = currentTick;
#elif defined(_EE)
	currentTick = GetTimerSystemTime();
	const float tempDeltaTime = (currentTick - lastTick) / (float)kBUSCLK;
	lastTick = currentTick;
#else
	start_point = std::chrono::high_resolution_clock::now();
	const float tempDeltaTime = std::chrono::duration<float>(start_point - end_point).count();
	end_point = start_point;
#endif
	s_deltaTime = tempDeltaTime * s_timeScale;
	s_unscaledDeltaTime = tempDeltaTime;
	
	if (s_deltaTime >= 0.2f) 
	{
		s_deltaTime = 0.2f;
	}

	if (s_unscaledDeltaTime >= 0.2f)
	{
		s_unscaledDeltaTime = 0.2f;
	}

	s_time += s_deltaTime;
	s_unscaledTime += s_unscaledDeltaTime;
}