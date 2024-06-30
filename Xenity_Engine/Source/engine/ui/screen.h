// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <engine/api.h>

class API Screen
{
public:

	/**
	* @brief Get the height of the screen in pixels
	*/
	static inline int GetHeight() { return height; }

	/**
	* @brief Get the width of the screen in pixels
	*/
	static inline int GetWidth() { return width; }

	/**
	* @brief Set if the window should be in fullscreen mode
	* @brief Only for Windows, no effect on other platforms
	* @param useFullScreenMode True to enable fullscreen, false to disable
	*/
	static void SetFullScreen(bool useFullScreenMode);

	/**
	* @brief Set if the window should use VSync
	* @param isVSync True to enable VSync, false to disable
	*/
	static void SetVSync(bool isVSync);

	/**
	* @brief Get if VSync is enabled
	* @return True if VSync is enabled, false otherwise
	*/
	static bool IsVSyncEnabled();

private:
	static int height;
	static int width;
	static bool useVSync;
	friend class Window;
};

