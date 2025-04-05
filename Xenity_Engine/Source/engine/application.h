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
	static void OpenURL(const std::string& url);
	static void Quit();

	[[nodiscard]] static Platform GetPlatform();
	[[nodiscard]] static std::string GetXenityVersion();
	[[nodiscard]] static std::string GetGameName();
	[[nodiscard]] static std::string GetCompanyName();
	[[nodiscard]] static AssetPlatform GetAssetPlatform();
	[[nodiscard]] static bool IsInEditor();

	/**
	* @brief Return the game folder (folder where you can write data)
	*/
	[[nodiscard]] static std::string GetGameDataFolder();
	[[nodiscard]] static std::string GetGameFolder();
private:
	friend class InspectorMenu;
	friend class BuildSettingsMenu;
	friend class Compiler;
	static AssetPlatform PlatformToAssetPlatform(Platform platform);
};

