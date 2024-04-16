#pragma once

/**
 * [Internal]
 */

#include <string>
#include <vector>
#include <engine/platform.h>
#include <engine/reflection/reflection.h>
#include <engine/event_system/event_system.h>

ENUM(BuildType, 
	EditorHotReloading,
	BuildGame,
	BuildAndRunGame,
	BuildAndRunOnHardwareGame);

ENUM(CompileResult, SUCCESS,
	ERROR_UNKNOWN,
	ERROR_FILE_COPY,
	ERROR_GAME_CODE_COPY,
	ERROR_FINAL_GAME_FILES_COPY,
	ERROR_WSL_COMPILATION,
	ERROR_WSL_ENGINE_CODE_COPY,
	ERROR_WSL_ENGINE_LIBS_INCLUDE_COPY,
	ERROR_WSL_CMAKELISTS_COPY,
	ERROR_DOCKER_NOT_FOUND,
	ERROR_DOCKER_NOT_RUNNING,
	ERROR_DOCKER_COMPILATION,
	ERROR_DOCKER_MISSING_IMAGE,
	ERROR_DOCKER_COULD_NOT_START,
	ERROR_COMPILER_AVAILABILITY);

ENUM(CompilerAvailability,
	AVAILABLE = 1,
	MISSING_COMPILER_SOFTWARE = 2,
	MISSING_ENGINE_COMPILED_LIB = 4,
	MISSING_PPSSPP = 8);

ENUM(DockerState,
	NOT_INSTALLED,
	NOT_RUNNING,
	MISSING_IMAGE,
	RUNNING);

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

struct CopyEntry 
{
	bool isFolder = false;
	std::string sourcePath = "";
	std::string destPath = "";
};

class Compiler
{
public:

	/**
	* Initialize the compiler
	*/
	static void Init();

	/**
	* Check if the compiler has all needed files to start a compilation
	*/
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

	/**
	* Get the event when the compilation ends
	*/
	static Event<CompilerParams, bool>& GetOnCompilationEndedEvent()
	{
		return OnCompilationEndedEvent;
	}

	/**
	* Get the event when the compilation starts
	*/
	static Event<CompilerParams>& GetOnCompilationStartedEvent()
	{
		return OnCompilationStartedEvent;
	}

	/**
	* Check if Docker is installed and running
	* @param callback Event to call when the check is done if the check is async
	* @return Docker state
	*/
	static DockerState CheckDockerState(Event<DockerState>* callback);

	/**
	* Create a Docker image to install all needed sdk and tools
	* @return True if the image was created
	*/
	static bool CreateDockerImage();

private:

	/**
	* Export all game's files into the build folder
	* @param params Compilation parameters
	* @return True if the export was successful
	*/
	static bool ExportProjectFiles(const CompilerParams& params);

	/**
	* Execute all created copy entries and clear the list
	* @return True if all copy entries were successful
	*/
	static bool ExecuteCopyEntries();

	/**
	* Create a new copy to be executed later
	* @param isFolder True if the source is a folder
	* @param source Source path
	* @param dest Destination path
	*/
	static void AddCopyEntry(bool isFolder, const std::string& source, const std::string& dest);

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
	 * @return Compilation result
	 */
	static CompileResult CompileGame(
		Platform platform,
		BuildType buildType,
		const std::string &exportPath);

	/**
	 * Compile code for Windows
	 * @param params Compilation parameters
	 * @return Compilation result
	 */
	static CompileResult CompileWindows(const CompilerParams &params);

	/**
	 * Compile code in WSL for PSP or PsVita
	 * @param params Compilation parameters
	 * @return Compilation result
	 */
	static CompileResult CompileWSL(const CompilerParams &params);

	/**
	 * Compile code in WSL for PSP or PsVita
	 * @param params Compilation parameters
	 * @return Compilation result
	 */
	static CompileResult CompileInDocker(const CompilerParams& params);

	/**
	 * To call when the compile function ends
	 * @param result Compilation result
	 * @param params Compilation parameters
	 */
	static void OnCompileEnd(CompileResult result, CompilerParams& params);

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
	 * @param params Compilation parameters
	 */
	static std::string GetNavToEngineFolderCommand(const CompilerParams &params);

	/**
	 * Get the command to compile the game as a dynamic library
	 * @param params Compilation parameters
	 * @param sourceDestFolders Source code destination folders
	 */
	static std::string GetCompileGameLibCommand(const CompilerParams &params, const std::vector<std::string>& sourceDestFolders);

	/**
	 * Get the command to compile the game as an executable file
	 * @param params Compilation parameters
	 */
	static std::string GetCompileExecutableCommand(const CompilerParams &params);

	/**
	 * Start game for build and run (PsVita not supported)
	 * @param platform Platform target
	 * @param exportPath Folder location of the build
	 */
	static void StartGame(Platform platform, const std::string &exportPath);

	static std::vector<CopyEntry> copyEntries;

	static Event<CompilerParams, bool> OnCompilationEndedEvent;
	static Event<CompilerParams> OnCompilationStartedEvent;

	static std::string compilerExecFileName;
	static std::string engineFolderLocation;
	static std::string engineProjectLocation;
};
