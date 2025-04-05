// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

/**
 * [Internal]
 */

class CrashHandler
{
public:

	/**
	* @brief Enable crash events
	*/
	static void Init();

	/**
	* @brief Call a function in a try/catch
	* @param function Function to call
	*/
	[[nodiscard]] static bool CallInTry(void (*function)());

private:

	/**
	* @brief Called function on error (Not in Editor mode)
	* @param signum Signal number
	*/
	static void Handler(int signum);
};

