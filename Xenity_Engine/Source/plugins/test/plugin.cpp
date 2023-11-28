#include "plugin.h"

#include <xenity.h>

void PluginTest::Init()
{
	Debug::Print( "PluginTest: Initialize!" );
}

void PluginTest::Shutdown()
{
	Debug::Print( "PluginTest: Shutdown!" );
}

PluginInfos PluginTest::CreateInfos()
{
	Debug::Print( "PluginTest: CreateInfos!" );

	PluginInfos infos {};
	infos.name = "Test";
	infos.version = "0.0.1";
	infos.description = 
		"A plugin without any concrete utility except testing the system.";
	infos.author = "Triforce Entertainment";
	return infos;
}