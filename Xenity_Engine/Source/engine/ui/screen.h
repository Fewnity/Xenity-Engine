// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <engine/api.h>
#include <string>

class API Screen
{
public:

	/**
	* @brief Get the width of the screen in pixels
	*/
	[[nodiscard]] static int GetWidth();

	/**
	* @brief Get the height of the screen in pixels
	*/
	[[nodiscard]] static int GetHeight();

	/**
	* @brief Set if the window should be in fullscreen mode
	* @brief Only for Windows and Linux, no effect on other platforms
	* @brief Does not affect the editor window
	* @param useFullScreenMode True to enable fullscreen, false to disable
	*/
	static void SetFullScreen(bool useFullScreenMode);

	/**
	* @brief Set if the window should use VSync (May cause graphical glitch on PSP)
	* @param useVSync True to enable VSync, false to disable
	*/
	static void SetVSync(bool useVSync);

	/**
	* @brief Get if VSync is enabled
	* @return True if VSync is enabled, false otherwise
	*/
	[[nodiscard]] static bool IsVSyncEnabled();

	/**
	* @brief Make a screenshot of the game (.png)
	* @brief (Note: on PSP/PsVita, plugins overlays are also captured)
	*
	* @param fileName The name of the file to save the screenshot (without the extension)
	*/
	static bool MakeScreenshot(const std::string& fileName);

private:

	friend class Window;

	static int s_height;
	static int s_width;
	static bool s_useVSync;
};

