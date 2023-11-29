#pragma once
#include "../engine/platform.h"
#include <string>

enum class BuildType
{
	EditorHotReloading,
	BuildGame,
	BuildAndRunGame,
};

enum class CompileResult
{
	SUCCESS,
	ERROR_UNKNOWN,
	ERROR_ENGINE_GAME_LIB_MISSING,
	ERROR_ENGINE_EDITOR_LIB_MISSING,
	ERROR_LIB_DLLS_MISSING,
	ERROR_ENGINE_HEADERS_COPY,
	ERROR_GAME_CODE_COPY,
	ERROR_FINAL_GAME_FILES_COPY,
	ERROR_WSL_COMPILATION,
	ERROR_WSL_ENGINE_CODE_COPY,
	ERROR_WSL_ENGINE_LIBS_INCLUDE_COPY,
	ERROR_WSL_CMAKELISTS_COPY,
};

struct CompilerParams
{
	Platform platform;
	BuildType buildType;
};

class Compiler
{
public:
	static void Compile(const CompilerParams& params);

	/**
	* Compile an engine plugin
	*/
	static void CompilePlugin(Platform platform, const std::string& pluginPath);

	/**
	* Compile the game code
	* @param platform Platform target
	* @param buildType Compile for hot reloading or for a simple build or for build and run
	* @param exportPath Folder location for the build
	*/
	static void CompileGame(Platform platform, BuildType buildType, const std::string& exportPath);


	/**
	* Compile the game code (non blocking code)
	* @param platform Platform target
	* @param buildType Compile for hot reloading or for a simple build or for build and run
	* @param exportPath Folder location for the build
	*/
	static void CompileGameThreaded(Platform platform, BuildType buildType, const std::string& exportPath);

	/**
	* Start hot reloading
	*/
	static void HotReloadGame();

private:
	/**
	* To call when the compile function ends
	* 
	*/
	static void OnCompileEnd(CompileResult result);

	/**
	* Compile the game code in WSL for PSP or PsVita
	* @param platform Platform target
	* @param exportPath Folder location for the build
	*/
	static CompileResult CompileInWSL(Platform platform, const std::string& exportPath);

	/**
	* Get the command to start the compiler
	*/
	static std::string GetStartCompilerCommand();

	/**
	* Get the command to add another command
	*/
	static std::string GetAddNextCommand();

	/**
	* Get the command to navigate to the engine folder
	*/
	static std::string GetNavToEngineFolderCommand();

	/**
	* Get the command to compile the game as a dynamic library
	* @param buildType
	*/
	static std::string GetCompileGameLibCommand(BuildType buildType);

	/**
	* Get the command to compile the game as an executable file
	*/
	static std::string GetCompileGameExeCommand();

	/**
	* Start game for build and run (PsVita not supported)
	* @param buildType
	* @param location of the game folder
	*/
	static void StartGame(Platform platform, const std::string& exportPath);
};

