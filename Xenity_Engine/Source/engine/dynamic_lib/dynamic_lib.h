// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

/**
 * [Internal]
 */

#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)

#include <string>
#include <memory>

class GameInterface;

/**
 * @brief Used to load/unload a game DLL (used only on Windows)
 */
class DynamicLibrary
{
public:

	/**
	* @brief Load a game DLL
	* @param libraryName Name of the library
	*/
	static void LoadGameLibrary(const std::string& libraryName);

	/**
	* @brief Unload the game DLL
	*/
	static void UnloadGameLibrary();

	/**
	* @brief Create a game interface from the DLL
	*/
	static std::unique_ptr<GameInterface> CreateGame();
};
#endif