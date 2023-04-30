#pragma once

#include <chrono>

class Time
{
public:
	static float GetTime();
	static float GetUnscaledTime();
	static float GetDeltaTime();
	static float GetUnscaledDeltaTime();
	static float GetTimeScale();
	static void SetTimeScale(float timeScale);
	static void UpdateTime();
	static void Init();

private:
	static float timeScale;
	static float time;
	static float deltaTime;
	static float unscaledTime;
	static float unscaledDeltaTime;
};

