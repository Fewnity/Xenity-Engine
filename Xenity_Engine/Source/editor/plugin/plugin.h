// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <string>

struct PluginInfos
{
	std::string name = "N/A";
	std::string version = "1.0.0";
	std::string description = "No description.";
	std::string author = "Unknown";
};

class Plugin
{
public:
	virtual ~Plugin() = default;

	void Setup();

	virtual void Startup() = 0;
	virtual void Shutdown() = 0;

	[[nodiscard]] virtual PluginInfos CreateInfos() = 0;
	[[nodiscard]] const PluginInfos& GetInfos() const { return infos; }

private:
	PluginInfos infos;
};

