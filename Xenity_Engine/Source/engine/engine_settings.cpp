#include "engine_settings.h"
#include <engine/file_system/file_system.h>
#include <engine/file_system/file.h>
#include <engine/reflection/reflection_utils.h>

bool EngineSettings::isWireframe = false;
int EngineSettings::maxLightCount = 2;

bool EngineSettings::useProfiler = true;
bool EngineSettings::useDebugger = true;
bool EngineSettings::useOnlineDebugger = false;

bool EngineSettings::compileOnCodeChanged = false;
bool EngineSettings::compileWhenOpeningProject = false;

std::string EngineSettings::engineProjectPath = "C:\\Users\\elect\\Documents\\GitHub\\Xenity-Engine\\Xenity_Engine\\";
//std::string EngineSettings::engineProjectPath = "D:\\Gregory_Machefer\\Xenity - Engine\\Xenity_Engine\\"; // At school
std::string EngineSettings::compilerPath = "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Auxiliary\\Build\\";
//std::string EngineSettings::compilerPath = "C:\\VisualStudio\\VC\\Auxiliary\\Build\\";
std::string EngineSettings::ppssppExePath = "C:\\Program Files\\PPSSPP\\PPSSPPWindows64.exe";

// Texture::AnisotropicLevel EngineSettings::anisotropicLevel = Texture::AnisotropicLevel::X16;

void EngineSettings::SaveEngineSettings()
{
	std::shared_ptr<File> file = FileSystem::MakeFile("engine_settings.json");
	bool result = ReflectionUtils::ReflectiveDataToFile(GetReflectiveData(), file);
}

void EngineSettings::LoadEngineSettings()
{
	std::shared_ptr<File> file = FileSystem::MakeFile("engine_settings.json");
	bool result = ReflectionUtils::FileToReflectiveData(file, GetReflectiveData());
}

ReflectiveData EngineSettings::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, EngineSettings::useProfiler, "useProfiler", true);
	Reflective::AddVariable(reflectedVariables, EngineSettings::useDebugger, "useDebugger", true);
	Reflective::AddVariable(reflectedVariables, EngineSettings::useOnlineDebugger, "useOnlineDebugger", true);
	Reflective::AddVariable(reflectedVariables, EngineSettings::engineProjectPath, "engineProjectPath", true);
	Reflective::AddVariable(reflectedVariables, EngineSettings::compilerPath, "compilerPath", true);
	Reflective::AddVariable(reflectedVariables, EngineSettings::ppssppExePath, "ppssppExePath", true);
	Reflective::AddVariable(reflectedVariables, EngineSettings::compileOnCodeChanged, "compileOnCodeChanged", true);
	Reflective::AddVariable(reflectedVariables, EngineSettings::compileWhenOpeningProject, "compileWhenOpeningProject", true);
	return reflectedVariables;
}