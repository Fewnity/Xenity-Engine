// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <string>

#include <engine/api.h>
#include <engine/platform.h>

class API Application
{
public:
	/**
	* @brief Opens the link in a web browser (Only for Windows)
	*/
	static void OpenURL(const std::string& url);

	/**
	* @brief Quits the application
	*/
	static void Quit();

	/**
	* @brief Returns the current platform the application is running on
	*/
	[[nodiscard]] static Platform GetPlatform();

	/**
	* @brief Returns the current version of Xenity Engine
	*/
	[[nodiscard]] static std::string GetXenityVersion();

	/**
	* @brief Returns the name of the game
	*/
	[[nodiscard]] static std::string GetGameName();

	/**
	* @brief Returns the name of the company
	*/
	[[nodiscard]] static std::string GetCompanyName();

	/**
	* @brief Returns the AssetPlatform of the current platform
	*/
	[[nodiscard]] static AssetPlatform GetAssetPlatform();

	/**
	* @brief Returns if the application is running in the editor
	*/
	[[nodiscard]] static bool IsInEditor();

	/**
	* @brief Return the game folder (folder where you can write data)
	*/
	[[nodiscard]] static std::string GetGameDataFolder();

	/**
	* @brief Returns the game folder (folder where the game is located, may be read-only)
	*/
	[[nodiscard]] static std::string GetGameFolder();

private:
	friend class InspectorMenu;
	friend class BuildSettingsMenu;
	friend class Compiler;
	static AssetPlatform PlatformToAssetPlatform(Platform platform);
};

