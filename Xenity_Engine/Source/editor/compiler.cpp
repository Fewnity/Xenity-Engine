// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "compiler.h"

#include <thread>
#include <filesystem>

// Engine
#include <engine/engine_settings.h>
#include <engine/asset_management/project_manager.h>
#include <engine/debug/debug.h>
#include <engine/scene_management/scene_manager.h>
#include <engine/engine.h>
#include <engine/dynamic_lib/dynamic_lib.h>
#include <engine/class_registry/class_registry.h>
#include <engine/game_interface.h>
#include <engine/tools/string_tag_finder.h>
#include <engine/file_system/directory.h>
#include <engine/file_system/file.h>
#include <engine/graphics/texture.h>
#include <engine/assertions/assertions.h>

#include <stb_image.h>
#include <stb_image_write.h>

// Editor
#include <editor/editor.h>
#include "ui/menus/docker_config_menu.h"
#include "ui/menus/build_settings_menu.h"

Event<CompilerParams, bool> Compiler::OnCompilationEndedEvent;
Event<CompilerParams> Compiler::OnCompilationStartedEvent;

std::string Compiler::engineFolderLocation = "";
std::string Compiler::engineProjectLocation = "";
std::string  Compiler::compilerExecFileName = "";
namespace fs = std::filesystem;

std::vector<CopyEntry> Compiler::copyEntries;

CompilationMethod Compiler::compilationMethod = CompilationMethod::MSVC;
bool Compiler::isCompilationCancelled = false;

void Compiler::AddCopyEntry(bool isFolder, const std::string& source, const std::string& dest)
{
	XASSERT(!source.empty(), "[Compiler::AddCopyEntry] source is empty")
	XASSERT(!dest.empty(), "[Compiler::AddCopyEntry] dest is empty")

	if (source.empty() || dest.empty())
	{
		return;
	}

	CopyEntry entry;
	entry.isFolder = isFolder;
	entry.sourcePath = source;
	entry.destPath = dest;

	copyEntries.push_back(entry);
}

bool Compiler::ExecuteCopyEntries()
{
	bool success = true;
	for (const CopyEntry& entry : copyEntries)
	{
		try
		{
			if (entry.isFolder)
			{
				fs::copy(
					entry.sourcePath,
					entry.destPath,
					fs::copy_options::overwrite_existing | fs::copy_options::recursive
				);
			}
			else
			{
				fs::copy_file(
					entry.sourcePath,
					entry.destPath,
					fs::copy_options::overwrite_existing
				);
			}
		}
		catch (const std::exception&)
		{
			Debug::PrintError("[Compiler::ExecuteCopyEntries] Cannot copy " + entry.sourcePath + " to " + entry.destPath, true);
			success = false;
		}
	}
	copyEntries.clear();
	return success;
}

#define ENGINE_EDITOR "Xenity_Editor"
#define ENGINE_GAME "Xenity_Engine"
#define ASSETS_FOLDER "assets\\"
#define ENGINE_ASSETS_FOLDER "engine_assets\\"
#define PUBLIC_ENGINE_ASSETS_FOLDER "public_engine_assets\\"
#define MSVC_START_FILE_64BITS "vcvars64.bat"
#define MSVC_START_FILE_32BITS "vcvars32.bat"

std::string MakePathAbsolute(const std::string& path, const std::string& root)
{
	if (!fs::path(path).is_absolute())
	{
		return root + "\\" + path;
	}

	return path;
}

CompileResult Compiler::Compile(CompilerParams params)
{
	Debug::ClearDebugLogs();
	isCompilationCancelled = false;

	// Ensure path are absolute
	const std::string root = fs::current_path().string();
	params.tempPath = MakePathAbsolute(params.tempPath, root);
	params.sourcePath = MakePathAbsolute(params.sourcePath, root);
	params.exportPath = MakePathAbsolute(params.exportPath, root);

	// Print parameters
	Debug::Print(
		"[Compiler::Compile] Preparing:"
		"\n- Platform: " + (EnumHelper::GetEnumStringsLists()[typeid(Platform).hash_code()])[(int)params.buildPlatform.platform].name.substr(2)
		+ "\n- Build Type: " + (EnumHelper::GetEnumStringsLists()[typeid(BuildType).hash_code()])[(int)params.buildType].name
		+ "\n- Temporary Path: " + params.tempPath
		+ "\n- Source Path: " + params.sourcePath
		+ "\n- Export Path: " + params.exportPath
		+ "\n- Library Name: " + params.libraryName
		+ "\n- Editor DLL: " + params.getEditorDynamicLibraryName()
		+ "\n- Runtime DLL: " + params.getDynamicLibraryName()
		, true);

	const CompilerAvailability availability = CheckCompilerAvailability(params);
	if (availability != CompilerAvailability::AVAILABLE)
	{
		OnCompileEnd(CompileResult::ERROR_COMPILER_AVAILABILITY, params);
		return CompileResult::ERROR_COMPILER_AVAILABILITY;
	}

	// Clean temporary directory
	try
	{
		fs::remove_all(params.tempPath);
		fs::create_directory(params.tempPath);
		fs::create_directory(engineProjectLocation + "Source\\game_code\\");
	}
	catch (const std::exception&)
	{
		Debug::PrintWarning("[Compiler::Compile] Unable to clear the compilation folder", true);
	}

	// Compile depending on platform
	CompileResult result = CompileResult::ERROR_UNKNOWN;
	switch (params.buildPlatform.platform)
	{
	case Platform::P_Windows:
		result = CompileWindows(params);
		break;
	case Platform::P_PSP:
	case Platform::P_PsVita:
		//result = CompileWSL(params); // Deprecated
		result = CompileInDocker(params);
		break;
	default:
		Debug::PrintError("[Compiler::Compile] No compile method for this platform!", true);
		break;
	}

	// Delete temp compiler folder content
	try
	{
		fs::remove_all(params.tempPath);
	}
	catch (const std::exception&) {}

	// Open build folder if success
	if (params.buildType == BuildType::BuildGame && result == CompileResult::SUCCESS)
	{
		Editor::OpenExplorerWindow(params.exportPath, false);
	}

	// Send compile result
	OnCompileEnd(result, params);
	return result;
}

void Compiler::Init()
{
	const std::string root = fs::current_path().string();
	engineFolderLocation = root + "\\";
	engineProjectLocation = engineFolderLocation;
#if defined (VISUAL_STUDIO)
	const size_t backSlashPos = engineProjectLocation.substr(0, engineProjectLocation.size() - 1).find_last_of('\\');
	engineProjectLocation = engineProjectLocation.erase(backSlashPos + 1) + "Xenity_Engine\\";
#endif
#if defined(_WIN64) 
	compilerExecFileName = MSVC_START_FILE_64BITS;
#else
	compilerExecFileName = MSVC_START_FILE_32BITS;
#endif
}

CompilerAvailability Compiler::CheckCompilerAvailability(const CompilerParams& params)
{
	int error = 0;

	// Check if the compiler executable exists
	if (!fs::exists(EngineSettings::values.compilerPath + compilerExecFileName))
	{
		error |= (int)CompilerAvailability::MISSING_COMPILER_SOFTWARE;
	}

	// Check if the engine compiled library exists
	if (params.buildPlatform.platform == Platform::P_Windows)
	{
		if (params.buildType == BuildType::EditorHotReloading)
		{
			if (!fs::exists(engineFolderLocation + ENGINE_EDITOR + ".lib") ||
				!fs::exists(engineFolderLocation + ENGINE_EDITOR + ".dll"))
			{
				error |= (int)CompilerAvailability::MISSING_ENGINE_COMPILED_LIB;
			}
		}
		else
		{
			if (!fs::exists(engineFolderLocation + ENGINE_GAME + ".lib") ||
				!fs::exists(engineFolderLocation + ENGINE_GAME + ".dll"))
			{
				error |= (int)CompilerAvailability::MISSING_ENGINE_COMPILED_LIB;
			}
		}
	}
	else if (params.buildPlatform.platform == Platform::P_PSP)
	{
		if (!fs::exists(EngineSettings::values.ppssppExePath))
		{
			error |= (int)CompilerAvailability::MISSING_PPSSPP;
		}
	}

	if (error == 0)
	{
		error |= (int)CompilerAvailability::AVAILABLE;
	}
	else
	{
		if (error & (int)CompilerAvailability::MISSING_COMPILER_SOFTWARE)
		{
			Debug::PrintError("[Compiler::CheckCompilerAvailability] Compiler executable " + std::string(compilerExecFileName) + " not found in " + EngineSettings::values.compilerPath, true);
		}
		if (error & (int)CompilerAvailability::MISSING_ENGINE_COMPILED_LIB)
		{
			Debug::PrintError("[Compiler::CheckCompilerAvailability] Compiled engine library not found in " + engineFolderLocation, true);
		}
		if (error & (int)CompilerAvailability::MISSING_PPSSPP)
		{
			Debug::PrintError("[Compiler::CheckCompilerAvailability] PPSSPP emulator not found at " + EngineSettings::values.ppssppExePath, true);
		}
	}
	return (CompilerAvailability)error;
}

CompileResult Compiler::CompilePlugin(Platform platform, const std::string& pluginPath)
{
	XASSERT(!pluginPath.empty(), "[Compiler::CompilePlugin] pluginPath is empty")

	if (pluginPath.empty())
		return CompileResult::ERROR_UNKNOWN;

	const std::string plugin_name = fs::path(pluginPath).parent_path().filename().string();

	CompilerParams params{};
	params.libraryName = "plugin_" + plugin_name;
	params.buildPlatform = BuildSettingsMenu::GetBuildPlatform(platform);
	params.buildType = BuildType::EditorHotReloading;
	params.sourcePath = pluginPath;
	params.tempPath = "plugins\\.build\\";
	params.exportPath = "plugins\\";

	const CompileResult result = Compile(params);
	return result;
}

DockerState Compiler::CheckDockerState(Event<DockerState>* callback)
{
	// 2>nul Silent error
	// 1>nul Silent standard output
	DockerState result = DockerState::NOT_INSTALLED;

	const int checkDockerInstallResult = system("docker 2>nul 1>nul");
	if (checkDockerInstallResult != 0)
	{
		result = DockerState::NOT_INSTALLED;
	}
	else
	{
		const int checkDockerRunningResult = system("docker ps 2>nul 1>nul");
		if (checkDockerRunningResult != 0)
		{
			result = DockerState::NOT_RUNNING;
		}
		else
		{
			const int checkDockerImageResult = system("docker image inspect ubuntu_test 2>nul 1>nul");
			if (checkDockerImageResult != 0)
			{
				result = DockerState::MISSING_IMAGE;
			}
			else
			{
				result = DockerState::RUNNING;
			}
		}
	}

	if (callback)
	{
		callback->Trigger(result);
	}

	return result;
}

bool Compiler::ExportProjectFiles(const std::string& exportPath)
{
	XASSERT(!exportPath.empty(), "[Compiler::ExportProjectFiles] exportPath is empty")
	if (exportPath.empty())
		return false;

	const std::string projectAssetFolder = ProjectManager::GetProjectFolderPath() + ASSETS_FOLDER;
	const size_t projectFolderPathLen = projectAssetFolder.size();
	const std::vector<uint64_t> ids = ProjectManager::GetAllUsedFileByTheGame();
	const size_t idsCount = ids.size();
	for (size_t i = 0; i < idsCount; i++)
	{
		const FileInfo* fileInfo = ProjectManager::GetFileById(ids[i]);
		if (fileInfo)
		{
			if (fileInfo->file->GetUniqueId() <= UniqueId::reservedFileId)
				continue;

			const std::string newPath = fileInfo->path.substr(projectFolderPathLen, fileInfo->path.size() - projectFolderPathLen);
			AddCopyEntry(false, fileInfo->path, exportPath + ASSETS_FOLDER + newPath);
			AddCopyEntry(false, fileInfo->path + ".meta", exportPath + ASSETS_FOLDER + newPath + ".meta");

			std::string folderToCreate = (exportPath + ASSETS_FOLDER + newPath);
			folderToCreate = folderToCreate.substr(0, folderToCreate.find_last_of('\\'));
			fs::create_directories(folderToCreate);
		}
	}

	AddCopyEntry(true, ProjectManager::GetEngineAssetFolderPath(), exportPath + ENGINE_ASSETS_FOLDER);
	AddCopyEntry(true, ProjectManager::GetPublicEngineAssetFolderPath(), exportPath + PUBLIC_ENGINE_ASSETS_FOLDER);
	AddCopyEntry(false, ProjectManager::GetProjectFolderPath() + PROJECT_SETTINGS_FILE_NAME, exportPath + PROJECT_SETTINGS_FILE_NAME);
	const bool copyResult = ExecuteCopyEntries();
	return copyResult;
}

CompileResult Compiler::CompileGame(const BuildPlatform buildPlatform, BuildType buildType, const std::string& exportPath)
{
	XASSERT(!exportPath.empty(), "[Compiler::CompileGame] exportPath is empty")
	if (exportPath.empty())
		return CompileResult::ERROR_UNKNOWN;

	CompilerParams params{};
	params.libraryName = "game";
	params.buildPlatform = buildPlatform;
	params.buildType = buildType;
	params.sourcePath = ProjectManager::GetAssetFolderPath();
	params.tempPath = ProjectManager::GetProjectFolderPath() + ".build\\";
	params.exportPath = exportPath;

	OnCompilationStartedEvent.Trigger(params);

	// Compile
	const CompileResult result = Compile(params);
	if (result != CompileResult::SUCCESS) return result;

	// Copy assets
	if (params.buildType != BuildType::EditorHotReloading)
	{
		if (buildPlatform.platform != Platform::P_PsVita) // PsVita files are included in the .vpk file
		{
			const bool copyResult = ExportProjectFiles(params.exportPath);
			if (!copyResult)
			{
				return CompileResult::ERROR_FILE_COPY;
			}
		}
	}

	// Launch game
	if (params.buildType == BuildType::BuildAndRunGame)
	{
		auto t = std::thread(StartGame, params.buildPlatform.platform, params.exportPath);
		t.detach();
	}

	return result;
}

void Compiler::CompileGameThreaded(const BuildPlatform buildPlatform, BuildType buildType, const std::string& exportPath)
{
	std::thread t = std::thread(CompileGame, buildPlatform, buildType, exportPath);
	t.detach();
}

void Compiler::HotReloadGame()
{
#if defined(_WIN32) || defined(_WIN64)
	Engine::game.reset();
	Engine::game = nullptr;

	// Prepare scene
	SceneManager::SaveScene(SaveSceneType::SaveSceneForHotReloading);
	SceneManager::ClearScene();

	// Reset registery and re-add basic components
	ClassRegistry::Reset();
	ClassRegistry::RegisterEngineComponents();

	// Unload library
	DynamicLibrary::UnloadGameLibrary();

	// Compile game
	Compiler::CompileGame(
		BuildSettingsMenu::GetBuildPlatform(Platform::P_Windows),
		BuildType::EditorHotReloading,
		ProjectManager::GetProjectFolderPath() + "temp\\"
	);

	// Reload game
	DynamicLibrary::LoadGameLibrary(ProjectManager::GetProjectFolderPath() + "temp\\" + "game_editor");

	// Create game instance
	Engine::game = DynamicLibrary::CreateGame();
	if (Engine::game)
	{
		Debug::Print("Game compilation done");
		Engine::game->Start();
	}
	else
	{
		Debug::PrintError("[Compiler::HotReloadGame] Game compilation failed");
	}
	SceneManager::RestoreSceneHotReloading();
#endif
}

void Compiler::OnCompileEnd(CompileResult result, CompilerParams& params)
{
	switch (result)
	{
	case CompileResult::SUCCESS:
		Debug::Print("[Compiler::OnCompileEnd] Code compiled successfully!");
		break;
	case CompileResult::ERROR_UNKNOWN:
		Debug::PrintError("[Compiler::OnCompileEnd] Unable to compile (unkown error)");
		break;
	case CompileResult::ERROR_GAME_CODE_COPY:
		Debug::PrintError("[Compiler::OnCompileEnd] Error when copying game's code");
		break;
	case CompileResult::ERROR_FINAL_GAME_FILES_COPY:
		Debug::PrintError("[Compiler::OnCompileEnd] Error when copying game's files");
		break;
	case CompileResult::ERROR_FILE_COPY:
		Debug::PrintError("[Compiler::OnCompileEnd] Error when copying files");
		break;

		// Specific to WSL
	case CompileResult::ERROR_WSL_COMPILATION:
		Debug::PrintError("[Compiler::OnCompileEnd] Unable to compile on WSL (probably a C++ error)");
		break;
	case CompileResult::ERROR_WSL_ENGINE_CODE_COPY:
		Debug::PrintError("[Compiler::OnCompileEnd] Error when copying engine's code");
		break;
	case CompileResult::ERROR_WSL_ENGINE_LIBS_INCLUDE_COPY:
		Debug::PrintError("[Compiler::OnCompileEnd] Error when copying engine's libraries files");
		break;
	case CompileResult::ERROR_WSL_CMAKELISTS_COPY:
		Debug::PrintError("[Compiler::OnCompileEnd] Error when copying CMakeLists.txt file");
		break;
	case CompileResult::ERROR_COMPILER_AVAILABILITY:
		Debug::PrintError("[Compiler::OnCompileEnd] The compiler is not correctly setup. Please check compiler settings at [Window->Engine Settings]");
		break;

		// Specific to Docker
	case CompileResult::ERROR_DOCKER_COMPILATION:
		Debug::PrintError("[Compiler::OnCompileEnd] Unable to compile on Docker (probably a C++ error)");
		break;
	case CompileResult::ERROR_DOCKER_NOT_FOUND:
		Debug::PrintError("[Compiler::OnCompileEnd] Unable to find Docker");
		break;
	case CompileResult::ERROR_DOCKER_NOT_RUNNING:
		Debug::PrintError("[Compiler::OnCompileEnd] Docker is not running");
		break;
	case CompileResult::ERROR_DOCKER_MISSING_IMAGE:
		Debug::PrintError("[Compiler::OnCompileEnd] Docker image is missing");
		break;
	case CompileResult::ERROR_DOCKER_COULD_NOT_START:
		Debug::PrintError("[Compiler::OnCompileEnd] Docker path is not correctly setup. Please check compiler settings at [Window->Engine Settings]");
		break;

	case CompileResult::ERROR_COMPILATION_CANCELLED:
		Debug::PrintError("[Compiler::OnCompileEnd] The compilation has been cancelled");
		break;

	default:
		Debug::PrintError("[Compiler::OnCompileEnd] Unable to compile (unkown error)");
		break;
	}

	OnCompilationEndedEvent.Trigger(params, result == CompileResult::SUCCESS);
}

std::string WindowsPathToWSL(const std::string& path)
{
	XASSERT(!path.empty(), "[Compiler::WindowsPathToWSL] path is empty")
	if (path.empty())
		return "";

	std::string newPath = path;
	newPath[0] = tolower(newPath[0]);
	const int pathSize = (int)path.size();
	for (int i = 1; i < pathSize; i++)
	{
		if (newPath[i] == '\\')
		{
			newPath[i] = '/';
		}
	}
	newPath.erase(newPath.begin() + 1);
	return newPath;
}

std::vector<std::string> CopyGameSource(const CompilerParams& params)
{
	std::vector<std::string> sourceDestFolders;

	// Copy source code

	fs::create_directory(params.tempPath + "source\\");

	const size_t sourcePathLen = params.sourcePath.size();
	std::shared_ptr<Directory> gameSourceDir = std::make_shared<Directory>(params.sourcePath);
	const std::vector<std::shared_ptr<File>> files = gameSourceDir->GetAllFiles(true);

	const size_t fileCount = files.size();
	for (size_t i = 0; i < fileCount; i++)
	{
		const std::string ext = files[i]->GetFileExtension();

		// Check extension
		if (ext != ".h" && ext != ".cpp")
			continue;

		// Copy file
		const std::string filePathToCopy = files[i]->GetPath();
		const std::string destFolder = params.tempPath + "source\\" + files[i]->GetFolderPath().substr(sourcePathLen);
		fs::create_directories(destFolder);

		fs::copy_file(
			filePathToCopy,
			params.tempPath + "source\\" + filePathToCopy.substr(sourcePathLen), // substr to remove all folders including the locations of the project/source files
			fs::copy_options::overwrite_existing
		);

		// Check if the destination folder is already in the list
		bool found = false;
		const size_t sourceDestFoldersSize = sourceDestFolders.size();
		for (size_t x = 0; x < sourceDestFoldersSize; x++)
		{
			if (sourceDestFolders[x] == destFolder)
			{
				found = true;
				break;
			}
		}

		// if not, add the destination folder in the list for later use
		if (!found)
			sourceDestFolders.push_back(destFolder);
	}

	return sourceDestFolders;
}

CompileResult Compiler::CompileWindows(const CompilerParams& params)
{
	compilationMethod = CompilationMethod::MSVC;

	if (params.buildType == BuildType::EditorHotReloading) // In hot reloading mode:
	{
		const std::string engineLibPath = engineFolderLocation + ENGINE_EDITOR + ".lib";

		// Copy engine editor lib to the temp build folder
		AddCopyEntry(false, engineLibPath, params.tempPath + ENGINE_EDITOR + ".lib");
		// Copy editor header
		AddCopyEntry(false, engineProjectLocation + "Source\\xenity_editor.h", params.tempPath + "xenity_editor.h");
	}
	else // In build mode:
	{
		const std::string engineLibPath = engineFolderLocation + ENGINE_GAME + ".lib";
		const std::string engineDllPath = engineFolderLocation + ENGINE_GAME + ".dll";
		const std::string sdlDllPath = engineFolderLocation + "SDL3.dll";
		const std::string glfwDllPath = engineFolderLocation + "glfw3.dll";
		const std::string freetypeDllPath = engineFolderLocation + "freetype.dll";

		// Copy engine game lib to the temp build folder
		AddCopyEntry(false, engineLibPath, params.tempPath + ENGINE_GAME + ".lib");
		// Copy all DLLs to the export folder
		AddCopyEntry(false, engineDllPath, params.exportPath + ENGINE_GAME + ".dll");
		AddCopyEntry(false, sdlDllPath, params.exportPath + "SDL3.dll");
		AddCopyEntry(false, glfwDllPath, params.exportPath + "glfw3.dll");
		AddCopyEntry(false, freetypeDllPath, params.exportPath + "freetype.dll");
	}

	// Copy engine headers to the temp build folder
	AddCopyEntry(true, engineProjectLocation + "Source\\engine\\", params.tempPath + "engine\\");
	AddCopyEntry(false, engineProjectLocation + "Source\\xenity.h", params.tempPath + "xenity.h");
	AddCopyEntry(false, engineFolderLocation + "main.cpp", params.tempPath + "main.cpp");
	const bool headerCopyResult = ExecuteCopyEntries();
	if (!headerCopyResult)
	{
		return CompileResult::ERROR_FILE_COPY;
	}

	std::shared_ptr<PlatformSettingsWindows> platformSettings = std::dynamic_pointer_cast<PlatformSettingsWindows>(params.buildPlatform.settings);
	if (platformSettings && platformSettings->icon)
	{
		// Copy game icon
		AddCopyEntry(false, platformSettings->icon->file->GetPath(), params.tempPath + "logo.ico");
	}
	else
	{
		// Copy default icon
		AddCopyEntry(false, engineFolderLocation + "logo.ico", params.tempPath + "logo.ico");
	}
	AddCopyEntry(false, engineFolderLocation + "res.rc", params.tempPath + "res.rc");
	const bool iconCopyResult = ExecuteCopyEntries();
	if (!iconCopyResult)
	{
		return CompileResult::ERROR_FILE_COPY;
	}

	std::vector<std::string> sourceDestFolders;

	// Copy source code
	try
	{
		sourceDestFolders = CopyGameSource(params);
	}
	catch (const std::exception&)
	{
		return CompileResult::ERROR_GAME_CODE_COPY;
	}

	// Setup compiler command
	std::string command = GetStartCompilerCommand();
	command += GetAddNextCommand();
	command += GetNavToEngineFolderCommand(params);
	command += GetAddNextCommand();
	command += GetCompileGameLibCommand(params, sourceDestFolders);
	if (params.buildType != BuildType::EditorHotReloading)
	{
		command += GetAddNextCommand();
		command += GetCompileIconCommand(params);
		command += GetAddNextCommand();
		command += GetCompileExecutableCommand(params);
	}

	Debug::Print("[Compiler::Compile] Command: " + command);
	// Run compilation
	const int buildResult = system(command.c_str());
	if (buildResult != 0)
	{
		return CompileResult::ERROR_UNKNOWN;
	}

	// Copy compiled files to export path
	if (params.buildType == BuildType::EditorHotReloading)
	{
		const std::string editor_dll_name = params.getEditorDynamicLibraryName();
		AddCopyEntry(false, params.tempPath + editor_dll_name, params.exportPath + editor_dll_name);
	}
	else
	{
		const std::string dll_name = params.getDynamicLibraryName();
		AddCopyEntry(false, params.tempPath + dll_name, params.exportPath + dll_name);
		AddCopyEntry(false, params.tempPath + params.libraryName + ".exe", params.exportPath + params.libraryName + ".exe");
	}
	const bool gameCopyResult = ExecuteCopyEntries();
	if (!gameCopyResult)
	{
		return CompileResult::ERROR_FINAL_GAME_FILES_COPY;
	}

	return CompileResult::SUCCESS;
}

// Deprecated
CompileResult Compiler::CompileWSL(const CompilerParams& params)
{
	compilationMethod = CompilationMethod::WSL;

	const std::string convertedEnginePath = WindowsPathToWSL(engineProjectLocation);
	const std::string convertedEngineExePath = WindowsPathToWSL(engineFolderLocation);
	// Clear compilation folder
	const int clearFolderResult = system("wsl sh -c 'rm -rf ~/XenityTestProject'");

	// Create folders
	const int createProjectFolderResult = system("wsl sh -c 'mkdir ~/XenityTestProject'");
	const int createBuildFolderResult = system("wsl sh -c 'mkdir ~/XenityTestProject/build'");

	// Copy files
	const std::string copyEngineSourceCommand = "wsl sh -c 'cp -R /mnt/" + convertedEnginePath + "Source ~/XenityTestProject'";
	const int copyCodeResult = system(copyEngineSourceCommand.c_str()); // Engine's source code + (game's code but to change later)
	const std::string copyEngineLibrariesCommand = "wsl sh -c 'cp -R /mnt/" + convertedEnginePath + "include ~/XenityTestProject'";
	const int copyLibrariesResult = system(copyEngineLibrariesCommand.c_str()); // Engine's libraries
	const std::string copyCmakeCommand = "wsl sh -c 'cp -R /mnt/" + convertedEngineExePath + "CMakeLists.txt ~/XenityTestProject'";
	const int copyCmakelistsResult = system(copyCmakeCommand.c_str()); // Cmakelists file

	if (copyCodeResult != 0)
	{
		return CompileResult::ERROR_WSL_ENGINE_CODE_COPY;
	}
	else if (copyLibrariesResult != 0)
	{
		return CompileResult::ERROR_WSL_ENGINE_LIBS_INCLUDE_COPY;
	}
	else if (copyCmakelistsResult != 0)
	{
		return CompileResult::ERROR_WSL_CMAKELISTS_COPY;
	}

	// get the thread number of the cpu
	unsigned int threadNumber = std::thread::hardware_concurrency();
	if (threadNumber == 0) // This function may returns 0, use 1 instead
	{
		threadNumber = 1;
	}

	std::string compileCommand = "wsl bash -c -i \"cd ~/XenityTestProject/build";
	if (params.buildPlatform.platform == Platform::P_PSP)
		compileCommand += " && psp-cmake -DMODE=psp ..";
	else if (params.buildPlatform.platform == Platform::P_PsVita)
		compileCommand += " && cmake -DMODE=psvita ..";

	compileCommand += " && cmake --build . -j" + std::to_string(threadNumber) + "\""; // Use thread number to increase compilation speed

	// Start compilation
	const int compileResult = system(compileCommand.c_str());
	if (compileResult != 0)
	{
		return CompileResult::ERROR_WSL_COMPILATION;
	}

	std::string compileFolderPath = params.exportPath;
	compileFolderPath = compileFolderPath.erase(1, 1);
	const size_t pathSize = compileFolderPath.size();
	for (size_t i = 0; i < pathSize; i++)
	{
		if (compileFolderPath[i] == '\\')
		{
			compileFolderPath[i] = '/';
		}
	}
	compileFolderPath[0] = tolower(compileFolderPath[0]);
	compileFolderPath = "/mnt/" + compileFolderPath;
	std::string copyGameCommand;
	if (params.buildPlatform.platform == Platform::P_PSP)
		copyGameCommand = "wsl sh -c 'cp ~/\"XenityTestProject/build/EBOOT.PBP\" \"" + compileFolderPath + "/EBOOT.PBP\"'";
	else if (params.buildPlatform.platform == Platform::P_PsVita)
		copyGameCommand = "wsl sh -c 'cp ~/\"XenityTestProject/build/hello.vpk\" \"" + compileFolderPath + "/hello.vpk\"'";

	const int copyGameResult = system(copyGameCommand.c_str());
	if (copyGameResult != 0)
	{
		return CompileResult::ERROR_FINAL_GAME_FILES_COPY;
	}

	// Copy game assets
	const bool gameCopyResult = ExportProjectFiles(params.exportPath);
	if (!gameCopyResult)
	{
		return CompileResult::ERROR_FINAL_GAME_FILES_COPY;
	}

	return CompileResult::SUCCESS;
}

bool Compiler::CreateDockerImage()
{
	const int result = system("docker build -t ubuntu_test . 1>nul");
	return result == 0;
}

void Compiler::CancelCompilation()
{
	if (compilationMethod == CompilationMethod::DOCKER) 
	{
		system("docker stop -t 0 XenityEngineBuild");
		isCompilationCancelled = true;
	}
}

CompileResult Compiler::CompileInDocker(const CompilerParams& params)
{
	compilationMethod = CompilationMethod::DOCKER;

	DockerState state = CheckDockerState(nullptr);
	if (state == DockerState::NOT_INSTALLED)
	{
		// Open the docker config menu if docker is not installed
		Editor::GetMenu<DockerConfigMenu>()->SetActive(true);
		Editor::GetMenu<DockerConfigMenu>()->Focus();
		return CompileResult::ERROR_DOCKER_NOT_FOUND;
	}
	else if (state == DockerState::NOT_RUNNING)
	{
		const bool startResult = Editor::OpenExecutableFile(EngineSettings::values.dockerExePath);
		if (startResult)
		{
			// Check every 3 seconds if docker is running
			for (int i = 0; i < 10; i++)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(3000));
				state = CheckDockerState(nullptr);
				if (state != DockerState::NOT_RUNNING)
					break;
			}

			if (state == DockerState::NOT_RUNNING)
				return CompileResult::ERROR_DOCKER_NOT_RUNNING;
			else // Wait a little bit to be sure docker is operational
				std::this_thread::sleep_for(std::chrono::milliseconds(5000));
		}
		else
		{
			return CompileResult::ERROR_DOCKER_COULD_NOT_START;
		}
	}

	if (state == DockerState::MISSING_IMAGE)
	{
		Debug::PrintWarning("The docker image is missing, creating image... (This may takes few minutes)");
		if (!CreateDockerImage())
		{
			return CompileResult::ERROR_DOCKER_MISSING_IMAGE;
		}
	}

	// We have to stop and remove the container to recreate it
	const int stopResult = system("docker stop XenityEngineBuild");
	const int removeResult = system("docker remove XenityEngineBuild");

	std::string prepareCompileCommand = "";
	if (params.buildPlatform.platform == Platform::P_PSP) 
	{
		std::shared_ptr<PlatformSettingsPSP> platformSettings = std::dynamic_pointer_cast<PlatformSettingsPSP>(params.buildPlatform.settings);
		std::string debugDefine = platformSettings->isDebugMode ? " -DDEBUG=1" : "";
		prepareCompileCommand = "psp-cmake -DMODE=psp -DGAME_NAME=" + ProjectManager::GetGameName() + debugDefine + " ..";
	}
	else if (params.buildPlatform.platform == Platform::P_PsVita)
	{
		std::shared_ptr<PlatformSettingsPsVita> platformSettings = std::dynamic_pointer_cast<PlatformSettingsPsVita>(params.buildPlatform.settings);
		std::string debugDefine = platformSettings->isDebugMode ? " -DDEBUG=1" : "";
		prepareCompileCommand = "cmake -DMODE=psvita -DGAME_NAME=" + ProjectManager::GetGameName() + " -DVITA_TITLEID=" + platformSettings->gameId + debugDefine + " ..";
	}

	unsigned int threadNumber = std::thread::hardware_concurrency();
	if (threadNumber == 0) // This function may returns 0, use 1 instead
	{
		threadNumber = 1;
	}

	std::string createCommand = "docker create --name XenityEngineBuild ubuntu_test /bin/bash -c -it \"cd /home/XenityBuild/build/ ; " + prepareCompileCommand + " ; cmake --build . -j" + std::to_string(threadNumber) + "\"";
	const int createResult = system(createCommand.c_str());

	const std::string copyEngineSourceCommand = "docker cp \"" + engineProjectLocation + "Source\" XenityEngineBuild:\"/home/XenityBuild/\"";
	const int copyCodeResult = system(copyEngineSourceCommand.c_str()); // Engine's source code + (game's code but to change later)
	const std::string copyEngineLibrariesCommand = "docker cp \"" + engineProjectLocation + "include\" XenityEngineBuild:\"/home/XenityBuild/\"";
	const int copyLibrariesResult = system(copyEngineLibrariesCommand.c_str()); // Engine's libraries
	const std::string copyMainCommand = "docker cp \"" + engineFolderLocation + "main.cpp\" XenityEngineBuild:\"/home/XenityBuild/Source/\"";
	const int copyMainResult = system(copyMainCommand.c_str()); // main.cpp file
	const std::string copyCmakeCommand = "docker cp \"" + engineFolderLocation + "CMakeLists.txt\" XenityEngineBuild:\"/home/XenityBuild/\"";
	const int copyCmakelistsResult = system(copyCmakeCommand.c_str()); // Cmakelists file

	// Copy source code in the build folder
	try
	{
		CopyGameSource(params);
	}
	catch (const std::exception&)
	{
		return CompileResult::ERROR_GAME_CODE_COPY;
	}

	// Copy game source from the build folder to docker
	const std::string copyGameSourceCommand = "docker cp \"" + params.tempPath + "source\" XenityEngineBuild:\"/home/XenityBuild/Source/game_code/\"";
	const int copyGameSourceResult = system(copyGameSourceCommand.c_str());

	if (isCompilationCancelled)
		return CompileResult::ERROR_COMPILATION_CANCELLED;

	// Copy XMB/Livearea images
	if (params.buildPlatform.platform == Platform::P_PsVita)
	{
		CopyAssetsToDocker(params);
	}
	else if (params.buildPlatform.platform == Platform::P_PSP)
	{
		CopyAssetsToDocker(params);
	}

	if (isCompilationCancelled)
		return CompileResult::ERROR_COMPILATION_CANCELLED;

	const std::string startCommand = "docker start -a XenityEngineBuild";
	const int startResult = system(startCommand.c_str());

	if (isCompilationCancelled)
		return CompileResult::ERROR_COMPILATION_CANCELLED;

	std::string fileName = "";
	if (params.buildPlatform.platform == Platform::P_PSP)
		fileName = "EBOOT.PBP";
	else if (params.buildPlatform.platform == Platform::P_PsVita)
		fileName = ProjectManager::GetGameName() + ".vpk";

	// Copy final file
	const std::string copyGameFileCommand = "docker cp XenityEngineBuild:\"/home/XenityBuild/build/" + fileName + "\" \"" + params.exportPath + fileName + "\"";
	const int copyGameFileResult = system(copyGameFileCommand.c_str()); // Engine's source code + (game's code but to change later)

	// Copy prx file for build and run on psp hardware
	if (params.buildPlatform.platform == Platform::P_PSP)
	{
		std::string fileName2 = "hello.prx";
		const std::string copyGameFileCommand2 = "docker cp XenityEngineBuild:\"/home/XenityBuild/build/" + fileName2 + "\" \"" + params.exportPath + fileName2 + "\"";
		const int copyGameFileResult2 = system(copyGameFileCommand2.c_str()); // Engine's source code + (game's code but to change later)
	}

	if (copyGameFileResult != 0)
	{
		return CompileResult::ERROR_DOCKER_COMPILATION;
	}

	return CompileResult::SUCCESS;
}

void Compiler::CopyAssetsToDocker(const CompilerParams& params)
{
	if (params.buildPlatform.platform == Platform::P_PSP)
	{
		std::shared_ptr<PlatformSettingsPSP> platformSettings = std::dynamic_pointer_cast<PlatformSettingsPSP>(params.buildPlatform.settings);

		//---------------- PSP compiler will look for images in the build folder ----------------
		// Copy default psp images
		const std::string copyImagesCommand = "docker cp \"" + engineFolderLocation + "psp_images\" XenityEngineBuild:\"/home/XenityBuild/build/\"";
		const int copyImagesResult = system(copyImagesCommand.c_str());
		if (platformSettings)
		{
			// Copy and replace images with custom ones
			if (platformSettings->backgroundImage)
			{
				const std::string copyBgImageCommand = "docker cp \"" + platformSettings->backgroundImage->file->GetPath() + "\" XenityEngineBuild:\"/home/XenityBuild/build/psp_images/BG.PNG\"";
				const int copyBgImageResult = system(copyBgImageCommand.c_str());
			}
			if (platformSettings->iconImage)
			{
				const std::string copyIconImageCommand = "docker cp \"" + platformSettings->iconImage->file->GetPath() + "\" XenityEngineBuild:\"/home/XenityBuild/build/psp_images/ICON.PNG\"";
				const int copyIconImageResult = system(copyIconImageCommand.c_str());
			}
			if (platformSettings->previewImage)
			{
				const std::string copyPreviewImageCommand = "docker cp \"" + platformSettings->previewImage->file->GetPath() + "\" XenityEngineBuild:\"/home/XenityBuild/build/psp_images/PREVIEW.PNG\"";
				const int copyPreviewImageResult = system(copyPreviewImageCommand.c_str());
			}
		}
	}
	else if (params.buildPlatform.platform == Platform::P_PsVita)
	{
		std::shared_ptr<PlatformSettingsPsVita> platformSettings = std::dynamic_pointer_cast<PlatformSettingsPsVita>(params.buildPlatform.settings);
		// Copy default psp images
		const std::string copyImagesCommand = "docker cp \"" + engineFolderLocation + "psvita_images\" XenityEngineBuild:\"/home/XenityBuild/\"";
		const int copyImagesResult = system(copyImagesCommand.c_str());

		const std::string copyGameEngineAssetsCommand = "docker cp \"" + ProjectManager::GetEngineAssetFolderPath().substr(0, ProjectManager::GetEngineAssetFolderPath().size() - 1) + "\" XenityEngineBuild:\"/home/XenityBuild/\"";
		const int copyGameEngineAssetsResult = system(copyGameEngineAssetsCommand.c_str());

		const std::string copyGamePublicEngineAssetsCommand = "docker cp \"" + ProjectManager::GetPublicEngineAssetFolderPath().substr(0, ProjectManager::GetPublicEngineAssetFolderPath().size() - 1) + "\" XenityEngineBuild:\"/home/XenityBuild/\"";
		const int copyGamePublicEngineAssetsResult = system(copyGamePublicEngineAssetsCommand.c_str());

		const std::string copyProjectSettingsCommand = "docker cp \"" + ProjectManager::GetProjectFolderPath() + PROJECT_SETTINGS_FILE_NAME + "\" XenityEngineBuild:\"/home/XenityBuild/" + PROJECT_SETTINGS_FILE_NAME + "\"";
		const int copyProjectSettingsResult = system(copyProjectSettingsCommand.c_str());

		const bool copyResult = ExportProjectFiles(params.tempPath);
		const std::string copyGameAssetsCommand = "docker cp \"" + params.tempPath + "assets\" XenityEngineBuild:\"/home/XenityBuild/\"";
		const int copyGameAssetsResult = system(copyGameAssetsCommand.c_str());

		if (platformSettings)
		{
			// Copy and replace images with custom ones
			if (platformSettings->backgroundImage)
			{
				const std::string copyBgImageCommand = "docker cp \"" + platformSettings->backgroundImage->file->GetPath() + "\" XenityEngineBuild:\"/home/XenityBuild/psvita_images/bg.png\"";
				const int copyBgImageResult = system(copyBgImageCommand.c_str());
			}
			if (platformSettings->iconImage)
			{
				const std::string copyIconImageCommand = "docker cp \"" + platformSettings->iconImage->file->GetPath() + "\" XenityEngineBuild:\"/home/XenityBuild/psvita_images/icon0.png\"";
				const int copyIconImageResult = system(copyIconImageCommand.c_str());
			}
			if (platformSettings->startupImage)
			{
				const std::string copyPreviewImageCommand = "docker cp \"" + platformSettings->startupImage->file->GetPath() + "\" XenityEngineBuild:\"/home/XenityBuild/psvita_images/startup.png\"";
				const int copyPreviewImageResult = system(copyPreviewImageCommand.c_str());
			}
		}
	}
}

std::string Compiler::GetStartCompilerCommand()
{
	const std::string path = EngineSettings::values.compilerPath;

	std::string command;
	if (fs::path(path).is_absolute())
	{
		command += path.substr(0, 2) + " && "; // Go to the compiler folder
	}
	command += "cd \"" + EngineSettings::values.compilerPath + "\""; // Go to the compiler folder
	command += " && " + compilerExecFileName; // Start the compiler
	//command += " >nul";	// Mute output
	return command;
}

std::string Compiler::GetAddNextCommand()
{
	const std::string command = " && ";
	return command;
}

std::string Compiler::GetNavToEngineFolderCommand(const CompilerParams& params)
{
	std::string command;
	command += params.tempPath.substr(0, 2) + " && "; // Change current drive
	command += "cd \"" + params.tempPath + "\"";
	return command;
}

std::string Compiler::GetCompileGameLibCommand(const CompilerParams& params, const std::vector<std::string>& sourceDestFolders)
{
	std::string command = "";
	// MP for multithreading (faster compilation)
	// EHsc for exceptions
	// MD Use dll to compile (MDd for debug mode)
	// DIMPORT define "IMPORT"
#if defined(DEBUG)
	command += "cl /std:c++20 /MP /EHsc /MDd /DIMPORT"; // Start compilation
#else
	command += "cl /std:c++20 /MP /EHsc /MD /DIMPORT"; // Start compilation
#endif

	// Define "EDITOR" if compiled to play game in editor
	if (params.buildType == BuildType::EditorHotReloading)
	{
		command += " /DEDITOR";
	}

	// Add include directories
	command += " -I \"" + engineProjectLocation + "include\"";
	command += " -I \"" + engineProjectLocation + "Source\"";

	// Create DLL
	command += " /LD";

	// Add all source folders
	const size_t sourceDestFoldersSize = sourceDestFolders.size();
	for (size_t i = 0; i < sourceDestFoldersSize; i++)
	{
		command += " \"" + sourceDestFolders[i] + "*.cpp\"";
	}

	// Add the .lib file to use
	if (params.buildType != BuildType::EditorHotReloading)
	{
		command += " " + std::string(ENGINE_GAME) + ".lib";
	}
	else
	{
		command += " " + std::string(ENGINE_EDITOR) + ".lib";
	}

	command += " /link";
	// Set .lib output file name
	command += " /implib:" + params.libraryName + ".lib";
	//command += " /DEBUG"; ///????
	// Set dll output file name
	if (params.buildType != BuildType::EditorHotReloading)
	{
		command += " /out:" + params.getDynamicLibraryName();
	}
	else
	{
		command += " /out:" + params.getEditorDynamicLibraryName();
	}

	//command += " >nul"; // Mute output
	return command;
}

std::string Compiler::GetCompileIconCommand(const CompilerParams& params)
{
	std::string command;
	//Buid game resource
	command = "rc res.rc";
	//command += " >nul"; // Mute output
	return command;
}

std::string Compiler::GetCompileExecutableCommand(const CompilerParams& params)
{
	std::string command;
	//Buid game exe
	command = "cl /Fe\"" + params.libraryName + ".exe\" res.res /std:c++20 /MP /EHsc";
	command += " -I \"" + engineProjectLocation + "include\"";
	command += " -I \"" + engineProjectLocation + "Source\"";
	command += " main.cpp " + std::string(ENGINE_GAME) + ".lib";
	//command += " >nul"; // Mute output
	return command;
}

void Compiler::StartGame(Platform platform, const std::string& exportPath)
{
	XASSERT(!exportPath.empty(), "[Compiler::StartGame] exportPath is empty")
	if (exportPath.empty())
		return;

	std::string command = "";
	if (platform == Platform::P_Windows)
	{
		const std::string fileName = ProjectManager::GetGameName();
		command = "cd \"" + exportPath + "\"" + " && " + "\"" + fileName + ".exe\"";
	}
	else if (platform == Platform::P_PSP)
	{
		command = "(\"" + EngineSettings::values.ppssppExePath + "\" \"" + exportPath + "EBOOT.PBP\")";
	}

	if (!command.empty())
		system(command.c_str());
}