#pragma once
#include "../engine/platform.h"
#include <string>

enum BuildType {
	EditorHotReloading,
	BuildGame,
	BuildAndRunGame,
};

class Compiler
{
public:
	static void CompileGameThreaded(Platform platform, BuildType buildType, std::string exportPath);
	static void HotReloadGame();
private:
	static void CompileGame(Platform platform, BuildType buildType, std::string exportPath);
	static void CompileInWSL(Platform platform, std::string exportPath);
	static std::string GetStartCompilerCommand();
	static std::string GetAddNextCommand();
	static std::string GetNavToEngineFolderCommand();
	static std::string GetCompileGameLibCommand(BuildType buildType);
	static std::string GetCompileGameExeCommand();
	static void StartGame(Platform platform, std::string exportPath);
};

