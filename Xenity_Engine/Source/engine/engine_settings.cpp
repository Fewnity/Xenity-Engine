#include "engine_settings.h"

bool EngineSettings::isWireframe = false;
int EngineSettings::maxLightCount = 2;
std::string EngineSettings::RootFolder = "";

bool EngineSettings::useProfiler = true;
bool EngineSettings::useLighting = true;
bool EngineSettings::useDebugger = true;
bool EngineSettings::useOnlineDebugger = true;

// Texture::AnisotropicLevel EngineSettings::anisotropicLevel = Texture::AnisotropicLevel::X16;
