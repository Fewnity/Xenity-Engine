#pragma once
#include <engine/api.h>

#include <chrono>

/**
* @brief Class to get time informations (Delta time, elapsed time)
*/
class API Time
{
public:

	/**
	* @brief [Internal] Init time system
	*/
	static void Init();

	/**
	* @brief [Internal] Update time values
	*/
	static void UpdateTime();

	/**
	* @brief Get total scaled elapsed time
	*/
	static float GetTime();

	/**
	* @brief Get total unscaled elapsed time
	*/
	static float GetUnscaledTime();

	/**
	* @brief Get scaled delta time
	*/
	static float GetDeltaTime();

	/**
	* @brief Get unscaled delta time
	*/
	static float GetUnscaledDeltaTime();

	/**
	* @brief Get time scale
	*/
	static float GetTimeScale();

	/**
	* @brief Set time scale
	* @param timeScale Time scale (minium 0)
	*/
	static void SetTimeScale(float _timeScale);

private:
	static float timeScale;
	static float time;
	static float deltaTime;
	static float unscaledTime;
	static float unscaledDeltaTime;
};