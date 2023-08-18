#include "time.h"
#include <chrono>
#include "../debug/debug.h"

float Time::timeScale = 1;
float Time::time = 0;
float Time::unscaledTime = 0;
float Time::deltaTime = 0;
float Time::unscaledDeltaTime = 0;
std::chrono::time_point<std::chrono::high_resolution_clock> start_point, end_point;

using namespace std::chrono;

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

void Time::SetTimeScale(float timeScale)
{
	if (timeScale < 0)
		timeScale = 0;

	Time::timeScale = timeScale;
}

#pragma endregion

void Time::Init()
{
	start_point = high_resolution_clock::now();
	end_point = high_resolution_clock::now();
	Debug::Print("-------- Time system initiated --------");
}

void Time::UpdateTime()
{
	long long start = time_point_cast<milliseconds>(start_point).time_since_epoch().count();
	long long end = time_point_cast<milliseconds>(end_point).time_since_epoch().count();
	end_point = start_point;
	float tempDeltaTime = (start - end) / 1000.0f;

	deltaTime = tempDeltaTime * timeScale;
	unscaledDeltaTime = tempDeltaTime;

	time += deltaTime;
	unscaledTime += unscaledDeltaTime;
	start_point = high_resolution_clock::now();
}