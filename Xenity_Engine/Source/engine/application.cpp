// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "application.h"
#include "engine.h"
#include <engine/asset_management/project_manager.h>

#if defined(_WIN32) || defined(_WIN64)
#include <ShObjIdl.h>
#endif

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
#elif defined(_WIN32) || defined(_WIN64)
	return Platform::P_Windows;
#endif
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
