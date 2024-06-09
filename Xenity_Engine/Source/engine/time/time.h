// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

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
	static inline float GetTime()
	{
		return time;
	}

	/**
	* @brief Get total unscaled elapsed time
	*/
	static inline float GetUnscaledTime()
	{
		return unscaledTime;
	}

	/**
	* @brief Get scaled delta time
	*/
	static inline float GetDeltaTime()
	{
		return deltaTime;
	}
	/**
	* @brief Get unscaled delta time
	*/
	static inline float GetUnscaledDeltaTime()
	{
		return unscaledDeltaTime;
	}

	/**
	* @brief Get time scale
	*/
	static inline float GetTimeScale()
	{
		return timeScale;
	}

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