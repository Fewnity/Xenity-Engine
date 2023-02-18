#pragma once

//#define PROJECT_FOLDER R"(C:\Users\gregory.machefer\Documents\GitHub\Xenity-Engine\)"
//#define PROJECT_FOLDER R"(C:\Users\elect\Documents\GitHub\Xenity-Engine\)" 
//#define PROJECT_FOLDER R"(C:\Users\gregory.machefer\Downloads\OpenGL_Test\)"
//#define PROJECT_FOLDER R"(D:\Projet Visual Studio\OpenGL_Test\)"
//#define PROJECT_FOLDER R"(D:\Projet Visual Studio\OpenGL_Test\)";

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

