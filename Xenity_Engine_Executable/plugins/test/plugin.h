#pragma once

#include <xenity.h>
#include <editor/plugin/api.h>

class PluginTest : public Plugin
{
public:
	void Startup() override;
	void Shutdown() override;

	PluginInfos CreateInfos() override;
};

REGISTER_PLUGIN(PluginTest)