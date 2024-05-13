#pragma once
#include <engine/reflection/reflection.h>
#include <engine/event_system/event_system.h>
#include <engine/platform.h>
#include <engine/graphics/icon.h>

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
		Reflective::AddVariable(reflectedVariables, iconImage, "iconImage", true);
		Reflective::AddVariable(reflectedVariables, backgroundImage, "backgroundImage", true);
		Reflective::AddVariable(reflectedVariables, previewImage, "previewImage", true);
		return reflectedVariables;
	}

	int IsValid() override;

	std::shared_ptr<Texture> backgroundImage;
	std::shared_ptr<Texture> iconImage;
	std::shared_ptr<Texture> previewImage;
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
		Reflective::AddVariable(reflectedVariables, iconImage, "iconImage", true);
		Reflective::AddVariable(reflectedVariables, backgroundImage, "backgroundImage", true);
		Reflective::AddVariable(reflectedVariables, startupImage, "startupImage", true);
		Reflective::AddVariable(reflectedVariables, gameId, "gameId", true);
		return reflectedVariables;
	}

	int IsValid() override;

	std::shared_ptr<Texture> backgroundImage;
	std::shared_ptr<Texture> iconImage;
	std::shared_ptr<Texture> startupImage;
	std::string gameId = "";
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
		Reflective::AddVariable(reflectedVariables, icon, "icon", true);
		return reflectedVariables;
	}

	int IsValid() override;

	std::shared_ptr<Icon> icon;
private:
	bool isDebugMode = false;
	bool enableOnlineProfiler = false;
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