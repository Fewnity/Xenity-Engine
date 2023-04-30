#pragma once

#include <string>
#include "graphics/texture.h"

class EngineSettings
{
public:
	static bool isWireframe;
	static int maxLightCount;
	static std::string RootFolder;
	static Texture::AnisotropicLevel anisotropicLevel;
};

