#pragma once

#include <engine/plugin/plugin_manager.h>

class PluginTest : public Plugin
{
public:
	void Init() override;
	void Shutdown() override;

	PluginInfos CreateInfos() override;
};