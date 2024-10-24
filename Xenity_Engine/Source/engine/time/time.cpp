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

float Time::timeScale = 1;
float Time::time = 0;
float Time::unscaledTime = 0;
float Time::deltaTime = 0;
float Time::unscaledDeltaTime = 0;

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

	Time::timeScale = _timeScale;
}

#pragma endregion

void Time::Init()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);
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
	Debug::Print("-------- Time system initiated --------", true);
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
	const int64_t start = std::chrono::time_point_cast<std::chrono::milliseconds>(start_point).time_since_epoch().count();
	const int64_t end = std::chrono::time_point_cast<std::chrono::milliseconds>(end_point).time_since_epoch().count();
	end_point = start_point;
	start_point = std::chrono::high_resolution_clock::now();
	const float tempDeltaTime = (start - end) / 1000.0f;
#endif
	deltaTime = tempDeltaTime * timeScale;
	unscaledDeltaTime = tempDeltaTime;
	
	if (deltaTime >= 0.3333f) 
	{
		deltaTime = 0.3333f;
	}

	// Should we enable this?
	/*if (unscaledDeltaTime >= 0.3333)
	{
		unscaledDeltaTime = 0.3333;
	}*/

	time += deltaTime;
	unscaledTime += unscaledDeltaTime;
}