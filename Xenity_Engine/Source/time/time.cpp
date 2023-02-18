#include "time.h"
#include <SDL2/SDL_timer.h>

float Time::timeScale = 1;
float Time::time = 0;
float Time::unscaledTime = 0;
float Time::deltaTime = 0;
float Time::unscaledDeltaTime = 0;
float Time::lastTick = 0;

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

void Time::UpdateTime() 
{
	float tempTick = (float)SDL_GetTicks64();
	float tempDeltaTime = (tempTick - lastTick) / 1000.0f;
	lastTick = tempTick;

	deltaTime = tempDeltaTime * timeScale;
	unscaledDeltaTime = tempDeltaTime;

	time += deltaTime;
	unscaledTime += unscaledDeltaTime;
}