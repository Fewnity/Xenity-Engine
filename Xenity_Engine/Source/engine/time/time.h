#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include <chrono>

class API Time
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

