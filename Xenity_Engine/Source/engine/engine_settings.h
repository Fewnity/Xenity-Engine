#pragma once

#include <string>
// #include "graphics/texture.h"

class EngineSettings
{
public:
	static bool isWireframe;
	static int maxLightCount;
	static std::string RootFolder;
	static bool useProfiler;
	static bool useLighting;
	static bool useDebugger;
	static bool useOnlineDebugger;
	// static Texture::AnisotropicLevel anisotropicLevel;
};
