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

	/**
	* [Internal] Init time system
	*/
	static void Init();

	/**
	* [Internal] Update time values
	*/
	static void UpdateTime();

	/**
	* Get total scaled elapsed time
	*/
	static float GetTime();

	/**
	* Get total unscaled elapsed time
	*/
	static float GetUnscaledTime();

	/**
	* Get scaled delta time
	*/
	static float GetDeltaTime();

	/**
	* Get unscaled delta time
	*/
	static float GetUnscaledDeltaTime();

	/**
	* Get time scale
	*/
	static float GetTimeScale();

	/**
	* Set time scale
	* @param timeScale Time scale (minium 0)
	*/
	static void SetTimeScale(float timeScale);

private:
	static float timeScale;
	static float time;
	static float deltaTime;
	static float unscaledTime;
	static float unscaledDeltaTime;
};

