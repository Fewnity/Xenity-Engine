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
