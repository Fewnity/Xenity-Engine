// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <engine/api.h>

#include <chrono>

/**
* @brief Class used to test the performances of a piece of code
*/
class API Benchmark
{
public:

	/**
	* @brief Start the benchmark
	*/
	void Start();

	/**
	* @brief Calculate the elapsed time since the last Start call
	*/
	void Stop();

	/**
	* @brief Get elapsed microseconds between Start and Stop calls
	*/
	long long GetMicroSeconds() const;

	/**
	* @brief Get elapsed milliseconds between Start and Stop calls
	*/
	long long GetMilliseconds() const;

	/**
	* @brief Get elapsed seconds between Start and Stop calls
	*/
	float GetSeconds() const;

	/**
	* @brief Set times values to 0
	*/
	void Reset();

private:
	std::chrono::time_point< std::chrono::high_resolution_clock> start_point, end_point;
	long long time = 0;

#if defined(__PSP__)
	uint64_t endTick;
	uint64_t startTick;
#endif
};

