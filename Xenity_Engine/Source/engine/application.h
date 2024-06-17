// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
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

	static Platform GetPlatform();
	static std::string GetXenityVersion();
	static std::string GetGameName();
	static std::string GetCompanyName();

private:
	static std::string xenityVersion;
};

