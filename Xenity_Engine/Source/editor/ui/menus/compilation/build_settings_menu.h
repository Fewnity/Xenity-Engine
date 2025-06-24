// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <editor/ui/menus/menu.h>

#include <editor/compilation/compiler.h>
#include <editor/compilation/platform_settings.h>

#include <engine/platform.h>
#include <engine/event_system/event_system.h>

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

	size_t m_selectedPlatformIndex = 0;
	Event<>* m_onSettingChangedEvent = nullptr;
};

