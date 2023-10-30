#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include <string>

class API EngineSettings
{
public:

	/**
	* Save engine settings
	*/
	static void SaveEngineSettings();

	/**
	* Load engine settings
	*/
	static void LoadEngineSettings();

	static bool isWireframe;
	static int maxLightCount;
	static std::string RootFolder;
	static bool useProfiler;
	static bool useLighting;
	static bool useDebugger;
	static bool useOnlineDebugger;
};
