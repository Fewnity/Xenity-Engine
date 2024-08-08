// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include "menu.h"
#include <engine/platform.h>
#include <editor/compiler.h>
#include <engine/event_system/event_system.h>
#include <editor/platform_settings.h>

class Texture;

class BuildSettingsMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;
	void OnOpen() override;

	static const BuildPlatform& GetBuildPlatform(Platform platform);
private:
	void OnSettingChanged();
	void LoadSettings();
	void SaveSettings();
	void StartBuild(const BuildPlatform& buildPlatform, BuildType buildType);
	static std::vector<BuildPlatform> buildPlatforms;
	int lastSettingError = 0;

	size_t selectedPlatformIndex = 0;
	Event<>* onSettingChangedEvent = nullptr;
};

