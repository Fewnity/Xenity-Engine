// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <engine/api.h>

/**
* @brief Class to get time informations (Delta time, elapsed time)
*/
class API Time
{
public:

	/**
	* @brief Get total scaled elapsed time
	*/
	[[nodiscard]] static inline float GetTime()
	{
		return s_time;
	}

	/**
	* @brief Get total unscaled elapsed time
	*/
	[[nodiscard]] static inline float GetUnscaledTime()
	{
		return s_unscaledTime;
	}

	/**
	* @brief Get scaled delta time
	*/
	[[nodiscard]] static inline float GetDeltaTime()
	{
		return s_deltaTime;
	}
	/**
	* @brief Get unscaled delta time
	*/
	[[nodiscard]] static inline float GetUnscaledDeltaTime()
	{
		return s_unscaledDeltaTime;
	}

	/**
	* @brief Get time scale
	*/
	[[nodiscard]] static inline float GetTimeScale()
	{
		return s_timeScale;
	}

	/**
	* @brief Set time scale
	* @param timeScale Time scale (minium 0)
	*/
	static void SetTimeScale(float _timeScale);

private:
	friend class Engine;
	friend class GameplayManager;

	/**
	* @brief [Internal] Init time system
	*/
	static void Init();
	static void Reset();

	/**
	* @brief [Internal] Update time values
	*/
	static void UpdateTime();

	static float s_timeScale;
	static float s_time;
	static float s_deltaTime;
	static float s_unscaledTime;
	static float s_unscaledDeltaTime;
};