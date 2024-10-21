// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "application.h"

#if defined(_WIN32) || defined(_WIN64)
#include <ShObjIdl.h>
#endif

#include <engine/asset_management/project_manager.h>
#include <engine/constants.h>
#include "engine.h"

void Application::OpenURL(const std::string& url)
{
#if defined(_WIN32) || defined(_WIN64)
	const std::wstring wLink = std::wstring(url.begin(), url.end());
	ShellExecute(0, 0, wLink.c_str(), 0, 0, SW_SHOW);
#endif
}

void Application::Quit()
{
	Engine::Quit();
}

Platform Application::GetPlatform()
{
#if defined(__PSP__)
	return Platform::P_PSP;
#elif defined(__vita__)
	return Platform::P_PsVita;
#elif defined(__PS2__)
	return Platform::P_PS2;
#elif defined(__PS3__)
	return Platform::P_PS3;
#elif defined(__PS4__)
	return Platform::P_PS4;
#elif defined(__LINUX__)
	return Platform::P_Linux;
#elif defined(_WIN32) || defined(_WIN64)
	return Platform::P_Windows;
#endif
}

AssetPlatform Application::GetAssetPlatform()
{
	return PlatformToAssetPlatform(GetPlatform());
}

bool Application::IsInEditor()
{
#if defined(EDITOR)
	return true;
#endif
	return false;
}

AssetPlatform Application::PlatformToAssetPlatform(Platform platform)
{
if (platform == Platform::P_PSP)
	return AssetPlatform::AP_PSP;
else if (platform == Platform::P_PsVita)
	return AssetPlatform::AP_PsVita;
else if (platform == Platform::P_PS2)
	return AssetPlatform::AP_PS2;
else if (platform == Platform::P_PS3)
	return AssetPlatform::AP_PS3;
else if (platform == Platform::P_PS4)
	return AssetPlatform::AP_PS4;
else //if(platform == Platform::P_Windows || platform == Platform::P_Linux)
	return AssetPlatform::AP_Standalone;
}

std::string Application::GetXenityVersion()
{
	return ENGINE_VERSION;
}

std::string Application::GetGameName()
{
	return ProjectManager::projectSettings.gameName;
}

std::string Application::GetCompanyName()
{
	return ProjectManager::projectSettings.companyName;
}
