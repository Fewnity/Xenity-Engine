// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <engine/reflection/reflection.h>
#include <engine/event_system/event_system.h>
#include <engine/platform.h>
#include <engine/graphics/icon.h>
#include <engine/graphics/texture.h>

class Texture;

class PlatformSettings : public Reflective
{
public:
	PlatformSettings() = delete;
	PlatformSettings(Event<>* onChangeEvent)
	{
		this->onChangeEvent = onChangeEvent;
	}

	ReflectiveData GetReflectiveData() override
	{
		ReflectiveData reflectedVariables;
		return reflectedVariables;
	}

	void OnReflectionUpdated() override
	{
		if (onChangeEvent)
			onChangeEvent->Trigger();
	}

	virtual int IsValid() = 0;

	bool isDebugMode = false;
	bool enableOnlineProfiler = false;
	bool enableProfiler = false;
	bool useCompilationCache = false; // Should be removed when the engine will be compiled as a static library
protected:
	Event<>* onChangeEvent = nullptr;
};

class PlatformSettingsPSP : public PlatformSettings
{
public:
	PlatformSettingsPSP() = delete;
	PlatformSettingsPSP(Event<>* onChangeEvent) : PlatformSettings(onChangeEvent) {}

	ReflectiveData GetReflectiveData() override
	{
		ReflectiveData reflectedVariables;
		Reflective::AddVariable(reflectedVariables, isDebugMode, "isDebugMode", true);
		Reflective::AddVariable(reflectedVariables, enableProfiler, "enableProfiler", true);
		//Reflective::AddVariable(reflectedVariables, enableOnlineProfiler, "enableOnlineProfiler", true);
		Reflective::AddVariable(reflectedVariables, iconImage, "iconImage", true);
		Reflective::AddVariable(reflectedVariables, backgroundImage, "backgroundImage", true);
		Reflective::AddVariable(reflectedVariables, previewImage, "previewImage", true);
		Reflective::AddVariable(reflectedVariables, useCompilationCache, "useCompilationCache", true);
		return reflectedVariables;
	}

	int IsValid() override;

	std::shared_ptr<Texture> backgroundImage;
	std::shared_ptr<Texture> iconImage;
	std::shared_ptr<Texture> previewImage;
private:
};

class PlatformSettingsPsVita : public PlatformSettings
{
public:
	PlatformSettingsPsVita() = delete;
	PlatformSettingsPsVita(Event<>* onChangeEvent) : PlatformSettings(onChangeEvent) {}

	ReflectiveData GetReflectiveData() override
	{
		ReflectiveData reflectedVariables;
		Reflective::AddVariable(reflectedVariables, isDebugMode, "isDebugMode", true);
		Reflective::AddVariable(reflectedVariables, enableProfiler, "enableProfiler", true);
		//Reflective::AddVariable(reflectedVariables, enableOnlineProfiler, "enableOnlineProfiler", true);
		Reflective::AddVariable(reflectedVariables, iconImage, "iconImage", true);
		Reflective::AddVariable(reflectedVariables, backgroundImage, "backgroundImage", true);
		Reflective::AddVariable(reflectedVariables, startupImage, "startupImage", true);
		Reflective::AddVariable(reflectedVariables, gameId, "gameId", true);
		Reflective::AddVariable(reflectedVariables, useCompilationCache, "useCompilationCache", true);
		return reflectedVariables;
	}

	int IsValid() override;

	std::shared_ptr<Texture> backgroundImage;
	std::shared_ptr<Texture> iconImage;
	std::shared_ptr<Texture> startupImage;
	std::string gameId = "";
private:
};

class PlatformSettingsPS3 : public PlatformSettings
{
public:
	PlatformSettingsPS3() = delete;
	PlatformSettingsPS3(Event<>* onChangeEvent) : PlatformSettings(onChangeEvent) {}

	ReflectiveData GetReflectiveData() override
	{
		ReflectiveData reflectedVariables;
		Reflective::AddVariable(reflectedVariables, isDebugMode, "isDebugMode", true);
		//Reflective::AddVariable(reflectedVariables, enableProfiler, "enableProfiler", true);
		//Reflective::AddVariable(reflectedVariables, enableOnlineProfiler, "enableOnlineProfiler", true);
		Reflective::AddVariable(reflectedVariables, useCompilationCache, "useCompilationCache", true);
		return reflectedVariables;
	}

	int IsValid() override;
private:
};

class PlatformSettingsWindows : public PlatformSettings
{
public:
	PlatformSettingsWindows() = delete;
	PlatformSettingsWindows(Event<>* onChangeEvent) : PlatformSettings(onChangeEvent) {}

	ReflectiveData GetReflectiveData() override
	{
		ReflectiveData reflectedVariables;
		//Reflective::AddVariable(reflectedVariables, isDebugMode, "isDebugMode", true);
		//Reflective::AddVariable(reflectedVariables, enableProfiler, "enableProfiler", true);
		//Reflective::AddVariable(reflectedVariables, enableOnlineProfiler, "enableOnlineProfiler", true);
		Reflective::AddVariable(reflectedVariables, icon, "icon", true);
		return reflectedVariables;
	}

	int IsValid() override;

	std::shared_ptr<Icon> icon;
private:
};

class BuildPlatform
{
public:
	Platform platform = Platform::P_Windows;
	std::shared_ptr<Texture> icon;
	std::string name;
	bool isSupported = false;
	bool supportBuildAndRun = false;
	bool supportBuildAndRunOnHardware = false;
	std::shared_ptr<PlatformSettings> settings = nullptr;
};