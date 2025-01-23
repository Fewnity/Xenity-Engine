// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "frame_limiter.h"

#include <chrono>
#include <thread>

bool FrameLimiter::m_isEnabled = false;
uint32_t FrameLimiter::m_waitTiming = 0;

void FrameLimiter::Wait()
{
	if (!m_isEnabled)
	{
		return;
	}

#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
	std::this_thread::sleep_for(std::chrono::milliseconds(m_waitTiming));
#endif
}
