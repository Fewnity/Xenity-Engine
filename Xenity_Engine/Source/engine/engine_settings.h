#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include <string>
#include <engine/reflection/reflection.h>

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
	static bool useProfiler;
	static bool useDebugger;
	static bool useOnlineDebugger;
	static std::string engineProjectPath;
	static std::string compilerPath;
	static std::string ppssppExePath;

	static ReflectiveData GetReflectiveData();
};
