#pragma once

#include "menu.h"
#include <engine/platform.h>
#include <editor/compiler.h>
#include <engine/reflection/reflection.h>
#include <engine/debug/debug.h>
#include <engine/event_system/event_system.h>

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

private:
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
		Reflective::AddVariable(reflectedVariables, enableOnlineProfiler, "enableOnlineProfiler", true);
		return reflectedVariables;
	}

private:
	bool isDebugMode = false;
	bool enableOnlineProfiler = false;
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
		Reflective::AddVariable(reflectedVariables, enableOnlineProfiler, "enableOnlineProfiler", true);
		return reflectedVariables;
	}

private:
	bool isDebugMode = false;
	bool enableOnlineProfiler = false;
};

class PlatformSettingsWindows : public PlatformSettings
{
public:
	PlatformSettingsWindows() = delete;
	PlatformSettingsWindows(Event<>* onChangeEvent) : PlatformSettings(onChangeEvent) {}

	ReflectiveData GetReflectiveData() override
	{
		ReflectiveData reflectedVariables;
		Reflective::AddVariable(reflectedVariables, isDebugMode, "isDebugMode", true);
		Reflective::AddVariable(reflectedVariables, enableOnlineProfiler, "enableOnlineProfiler", true);
		return reflectedVariables;
	}

private:
	bool isDebugMode = false;
	bool enableOnlineProfiler = false;
};

class BuildSettingsMenu : public Menu
{
public:
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

	void Init() override;
	void Draw() override;
	void OnSettingChanged();

private:
	void LoadSettings();
	void SaveSettings();
	void StartBuild(Platform plaform, BuildType buildType);

	int selectedPlatformIndex = 0;
	std::vector<BuildPlatform> plaforms;
	Event<>* onSettingChangedEvent;
};

