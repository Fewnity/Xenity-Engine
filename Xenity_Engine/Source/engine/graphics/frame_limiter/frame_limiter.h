// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <cstdint>

class FrameLimiter
{
public:
	/**
	* @brief Enable or disable the frame limiter
	*/
	static void SetIsEnabled(bool isEnabled) 
	{
		m_isEnabled = isEnabled;
	}

	/**
	* @brief Set the frame limiter timing
	* @param waitTiming Frame limiter timing in milliseconds
	*/
	static void SetWaitTiming(uint32_t waitTiming)
	{
		m_waitTiming = waitTiming;
	}

	/**
	* @brief Wait for the frame limiter if enabled
	*/
	static void Wait();

private:
	static bool m_isEnabled;
	static uint32_t m_waitTiming;
};