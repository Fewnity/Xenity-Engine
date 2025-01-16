// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

/**
 * [Internal]
 */

#include <string>

#include <engine/reflection/reflection.h>
#include <engine/graphics/color/color.h>

/**
* @brief Contains all engine settings
*/
class EngineSettingsValues : public Reflective
{
public:
	ReflectiveData GetReflectiveData() override;
	
	bool isWireframe = false;
	int maxLightCount = 2;
	bool useProfiler = true;
	bool useDebugger = true;
	bool useOnlineDebugger = false;
	std::string compilerPath = "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Auxiliary\\Build\\";
	std::string ppssppExePath = "C:\\Program Files\\PPSSPP\\PPSSPPWindows64.exe";
	std::string dockerExePath = "C:\\Program Files\\Docker\\Docker\\Docker Desktop.exe";

	bool compileOnCodeChanged = false;
	bool compileWhenOpeningProject = false;

	Color backbgroundColor = Color::CreateFromRGBAFloat(0.059f, 0.059f, 0.059f, 1);
	Color secondaryColor = Color::CreateFromRGBAFloat(0.22f, 0.48f, 0.796f, 1);
	Color playTintColor = Color::CreateFromRGBAFloat(0.2f, 0.0f, 0.0f, 1);
	bool isPlayTintAdditive = true;
};

/**
* @brief Class used to save and load engine settings
*/
class EngineSettings
{
public:
	static EngineSettingsValues values;

	/**
	* @brief Save engine settings
	*/
	static void SaveEngineSettings();

	/**
	* @brief Load engine settings
	*/
	static void LoadEngineSettings();
};
