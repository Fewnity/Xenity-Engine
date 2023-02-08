#include "engine_settings.h"

bool EngineSettings::isWireframe = false;
float EngineSettings::deltaTime = 0;
int EngineSettings::maxLightCount = 2;
std::string EngineSettings::RootFolder = "";
Texture::AnisotropicLevel EngineSettings::anisotropicLevel = Texture::AnisotropicLevel::X16;
