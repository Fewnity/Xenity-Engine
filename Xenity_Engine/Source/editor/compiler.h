#pragma once

#include <string>

#include <engine/platform.h>

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
	// Plaftorm target
	Platform platform;

	// Build type
	BuildType buildType;

	// Path for temporary files to be created, automatically removed at the end of
	// compilation
	std::string tempPath;

	// Path for source files (.cpp & .h) to be copied
	std::string sourcePath;

	// Path for outputing the compiled files
	std::string exportPath;

	// Library file name (e.g. DLL)
	std::string libraryName;

	/**
	 * Get the editor dynamic-linked library file name (appending extension)
	 */
	std::string getEditorDynamicLibraryName() const
	{
		return libraryName + "_editor.dll";
	}

	/**
	 * Get runtime dynamic-linked library file name (appending extension)
	 */
	std::string getDynamicLibraryName() const
	{
		return libraryName + ".dll";
	}
};

class Compiler
{
public:
	/**
	 * General function to compile a source code
	 * @param params Compilation parameters
	 */
	static CompileResult Compile(CompilerParams params);

	/**
	 * Compile an engine plugin
	 * @param platform Target compilation platform
	 * @param pluginPath Source code path for the plugin
	 */
	static CompileResult CompilePlugin(
		Platform platform,
		const std::string &pluginPath);

	/**
	 * Compile the game code
	 * @param platform Platform target
	 * @param buildType Compile for hot reloading or for a simple build or for build and run
	 * @param exportPath Folder location for the build
	 */
	static CompileResult CompileGame(
		Platform platform,
		BuildType buildType,
		const std::string &exportPath);

	/**
	 * Compile the game code (non blocking code)
	 * @param platform Platform target
	 * @param buildType Compile for hot reloading or for a simple build or for build and run
	 * @param exportPath Folder location for the build
	 */
	static void CompileGameThreaded(Platform platform, BuildType buildType, const std::string &exportPath);

	/**
	 * Start hot reloading
	 */
	static void HotReloadGame();

private:
	/**
	 * Compile code for Windows
	 * @param params Compilation parameters
	 */
	static CompileResult CompileWindows(const CompilerParams &params);

	/**
	 * Compile code in WSL for PSP or PsVita
	 * @param params Compilation parameters
	 */
	static CompileResult CompileWSL(const CompilerParams &params);

	/**
	 * To call when the compile function ends
	 *
	 */
	static void OnCompileEnd(CompileResult result);

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
	static std::string GetNavToEngineFolderCommand(const CompilerParams &params);

	/**
	 * Get the command to compile the game as a dynamic library
	 * @param buildType
	 */
	static std::string GetCompileGameLibCommand(const CompilerParams &params);

	/**
	 * Get the command to compile the game as an executable file
	 */
	static std::string GetCompileExecutableCommand(const CompilerParams &params);

	/**
	 * Start game for build and run (PsVita not supported)
	 * @param buildType
	 * @param location of the game folder
	 */
	static void StartGame(Platform platform, const std::string &exportPath);
};
