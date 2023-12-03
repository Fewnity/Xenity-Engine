#pragma once

#include <string>
#include <vector>
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
	ERROR_FILE_COPY,
	ERROR_GAME_CODE_COPY,
	ERROR_FINAL_GAME_FILES_COPY,
	ERROR_WSL_COMPILATION,
	ERROR_WSL_ENGINE_CODE_COPY,
	ERROR_WSL_ENGINE_LIBS_INCLUDE_COPY,
	ERROR_WSL_CMAKELISTS_COPY,
	ERROR_COMPILER_AVAILABILITY,
};

enum class CompilerAvailability 
{
	AVAILABLE = 1,
	MISSING_COMPILER_SOFTWARE = 2,
	MISSING_ENGINE_COMPILED_LIB = 4,
	MISSING_PPSSPP = 8,
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
		return libraryName + "_Editor.dll";
	}

	/**
	 * Get runtime dynamic-linked library file name (appending extension)
	 */
	std::string getDynamicLibraryName() const
	{
		return libraryName + ".dll";
	}
};

class CopyEntry 
{
public:
	bool isFolder = false;
	std::string sourcePath = "";
	std::string destPath = "";
};

class Compiler
{
public:
	static std::vector<CopyEntry> copyEntries;

	static CompilerAvailability CheckCompilerAvailability(const CompilerParams& params);

	/**
	 * Compile an engine plugin
	 * @param platform Target compilation platform
	 * @param pluginPath Source code path for the plugin
	 */
	static CompileResult CompilePlugin(
		Platform platform,
		const std::string &pluginPath);

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
	* Execute all created copy entries and clear the list
	*/
	static bool ExecuteCopyEntries();

	/**
	* Create a new copy to be executed later
	*/
	static void AddCopyEntry(bool isFolder, std::string source, std::string dest);

	/**
	 * General function to compile a source code
	 * @param params Compilation parameters
	 */
	static CompileResult Compile(CompilerParams params);

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
