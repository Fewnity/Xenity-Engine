#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include <string>
// #include "graphics/texture.h"

class API EngineSettings
{
public:
	static bool isWireframe;
	static int maxLightCount;
	static std::string RootFolder;
	static bool useProfiler;
	static bool useLighting;
	static bool useDebugger;
	static bool useOnlineDebugger;
	static void SaveEngineSettings();
	static void LoadEngineSettings();
	// static Texture::AnisotropicLevel anisotropicLevel;
};
