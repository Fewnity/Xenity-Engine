#pragma once

#include <xenity.h>
#include <editor/plugin/plugin_manager.h>

class PluginTest : public Plugin
{
public:
	void Startup() override;
	void Shutdown() override;

	PluginInfos CreateInfos() override;
};

#if defined(EXPORT)
	// #define API __declspec(dllexport)
	#define API2
#elif defined(IMPORT)
	#define API2 __declspec(dllexport)
#else
	#define API2
#endif

extern "C"
{
	API2 Plugin* CreatePlugin() { return new PluginTest(); }
}