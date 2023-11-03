#include "time.h"
#include <chrono>
#include "../debug/debug.h"

#if defined(__PSP__)
#include <psptypes.h>
#include <psprtc.h>
#elif defined(__vita__)
#include <psp2/rtc.h> 
#endif

using namespace std::chrono;

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
#else
std::chrono::time_point<std::chrono::high_resolution_clock> start_point, end_point;
#endif

#pragma region Accessors

float Time::GetTime()
{
	return time;
}

float Time::GetUnscaledTime()
{
	return unscaledTime;
}

float Time::GetDeltaTime()
{
	return deltaTime;
}

float Time::GetUnscaledDeltaTime()
{
	return unscaledDeltaTime;
}

float Time::GetTimeScale()
{
	return timeScale;
}

void Time::SetTimeScale(float _timeScale)
{
	if (_timeScale < 0)
		_timeScale = 0;

	Time::timeScale = _timeScale;
}

#pragma endregion

void Time::Init()
{
#if defined(_WIN32) || defined(_WIN64)
	start_point = high_resolution_clock::now();
	end_point = high_resolution_clock::now();
#elif defined(__PSP__)
	sceRtcGetCurrentTick(&currentTick);
	lastTick = currentTick;
#elif defined(__vita__)
	sceRtcGetCurrentTick(&currentTick);
	lastTick = currentTick;
#endif
	Debug::Print("-------- Time system initiated --------");
}

void Time::UpdateTime()
{
#if defined(__PSP__)
	sceRtcGetCurrentTick(&currentTick);
	float tempDeltaTime = (currentTick - lastTick) / 1000000.0f;
	lastTick = currentTick;
#elif defined(__vita__)
	sceRtcGetCurrentTick(&currentTick);
	float tempDeltaTime = (currentTick.tick - lastTick.tick) / 1000000.0f;
	lastTick = currentTick;
#else
	long long start = time_point_cast<milliseconds>(start_point).time_since_epoch().count();
	long long end = time_point_cast<milliseconds>(end_point).time_since_epoch().count();
	end_point = start_point;
	start_point = high_resolution_clock::now();
	float tempDeltaTime = (start - end) / 1000.0f;
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