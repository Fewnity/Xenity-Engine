#pragma once

#include "menu.h"
#include <engine/platform.h>
#include <editor/compiler.h>
#include <engine/reflection/reflection.h>

class Texture;

class PlatformSettings : public Reflective
{
public:

};

class PlatformSettingsPSP : public PlatformSettings
{
public:
	ReflectiveData GetReflectiveData()
	{
		ReflectiveData reflectedVariables;
		Reflective::AddVariable(reflectedVariables, isDebugMode, "isDebugMode", true);
		return reflectedVariables;
	}
private:
	bool isDebugMode = false;
};

class PlatformSettingsPsVita : public PlatformSettings
{
public:
	ReflectiveData GetReflectiveData()
	{
		ReflectiveData reflectedVariables;
		Reflective::AddVariable(reflectedVariables, isDebugMode, "isDebugMode", true);
		return reflectedVariables;
	}
private:
	bool isDebugMode = false;
};

class PlatformSettingsWindows : public PlatformSettings
{
public:
	ReflectiveData GetReflectiveData()
	{
		ReflectiveData reflectedVariables;
		Reflective::AddVariable(reflectedVariables, isDebugMode, "isDebugMode", true);
		return reflectedVariables;
	}
private:
	bool isDebugMode = false;
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
private:
	void LoadSettings();
	void SaveSettings();
	void StartBuild(Platform plaform, BuildType buildType);
	int selectedPlatformIndex = 0;
	std::vector<BuildPlatform> plaforms;
};

