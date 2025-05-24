// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "compiler.h"

#include <thread>
#include <filesystem>

// Editor
#include <editor/editor.h>
#include <editor/ui/menus/compilation/docker_config_menu.h>
#include <editor/ui/menus/compilation/build_settings_menu.h>
#include <editor/cooker/cooker.h>
#include <editor/utils/copy_utils.h>
#include <editor/compilation/compiler_cache.h>

// Engine
#include <engine/engine_settings.h>
#include <engine/asset_management/project_manager.h>
#include <engine/debug/debug.h>
#include <engine/scene_management/scene_manager.h>
#include <engine/engine.h>
#include <engine/dynamic_lib/dynamic_lib.h>
#include <engine/class_registry/class_registry.h>
#include <engine/game_interface.h>
#include <engine/file_system/directory.h>
#include <engine/file_system/file.h>
#include <engine/assertions/assertions.h>
#include <engine/file_system/file_system.h>
#include <engine/application.h>
#include <engine/graphics/texture/texture.h>
#include <engine/constants.h>
#include <engine/graphics/frame_limiter/frame_limiter.h>
#include <engine/tools/benchmark.h>

namespace fs = std::filesystem;

// Note: A docker copy command will create a directory of the source folder has not the same name as the dest folder.
// If both folders have the same name, the content of src will be pasted in dst without creating a new folder

Event<CompilerParams, bool> Compiler::OnCompilationEndedEvent;
Event<CompilerParams> Compiler::OnCompilationStartedEvent;

std::string Compiler::engineFolderLocation = "";
std::string Compiler::engineProjectLocation = "";
std::string  Compiler::compilerExecFileName = "";

CompilationMethod Compiler::compilationMethod = CompilationMethod::MSVC;
bool Compiler::isCompilationCancelled = false;
CompilationTimings Compiler::timings;

std::string MakePathAbsolute(const std::string& path, const std::string& root)
{
	if (!fs::path(path).is_absolute())
	{
		return root + "/" + path;
	}

	return path;
}

CompileResult Compiler::Compile(CompilerParams params)
{
	XASSERT(params.buildPlatform.platform != Platform::P_COUNT, "[Compiler::Compile] Platform is not set");
	XASSERT(!params.tempPath.empty(), "[Compiler::Compile] Temporary path is not set");
	XASSERT(!params.sourcePath.empty(), "[Compiler::Compile] Source path is not set");
	XASSERT(!params.exportPath.empty(), "[Compiler::Compile] Export path is not set");
	XASSERT(!params.libraryName.empty(), "[Compiler::Compile] Library name is not set");

	FrameLimiter::SetIsEnabled(true);
	FrameLimiter::SetWaitTiming(250);

	DeleteTempFiles(params);

	isCompilationCancelled = false;

	// Ensure path are absolute
	const std::string root = fs::current_path().string();
	params.tempPath = MakePathAbsolute(params.tempPath, root);
	params.sourcePath = MakePathAbsolute(params.sourcePath, root);
	params.exportPath = MakePathAbsolute(params.exportPath, root);

	// Print parameters
	Debug::Print(
		"[Compiler::Compile] Preparing:"
		"\n- Platform: " + EnumHelper::EnumAsString(params.buildPlatform.platform).substr(2)
		+ "\n- Build Type: " + EnumHelper::EnumAsString(params.buildType)
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
		fs::create_directory(params.tempPath + "cooked_assets/");
		fs::create_directory(engineProjectLocation + "Source/game_code/");
	}
	catch (const std::exception&)
	{
		Debug::PrintWarning("[Compiler::Compile] Unable to clear the compilation folder", true);
	}

	// Save project settings of the build
	{
		ProjectSettings projectSettingsCopy = ProjectManager::projectSettings;

		ProjectManager::projectSettings.compiledLibEngineVersion = ENGINE_DLL_VERSION;
		bool isDebugMode = false;
#if defined(DEBUG)
		isDebugMode = true;
#endif
		ProjectManager::projectSettings.isLibCompiledForDebug = isDebugMode;

		bool is64Bits = false;
#if defined(_WIN64)
		is64Bits = true;
#endif
		ProjectManager::projectSettings.isLibCompiledFor64Bits = is64Bits;

		ProjectManager::SaveProjectSettings(params.tempPath);

		ProjectManager::projectSettings = projectSettingsCopy;
	}

	bool cookResult = true;

	// Cook assets
	if (params.buildType != BuildType::EditorHotReloading)
	{
		Benchmark cookBenchmark;
		cookBenchmark.Start();
		CookSettings cookSettings;
		cookSettings.exportPath = params.tempPath + "cooked_assets/";
		cookSettings.assetPlatform = Application::PlatformToAssetPlatform(params.buildPlatform.platform);
		cookSettings.platform = params.buildPlatform.platform;
		if (params.buildType == BuildType::BuildShaders)
		{
			cookSettings.exportShadersOnly = true;
		}
		else
		{
			cookSettings.exportShadersOnly = false;
		}
		cookResult = Cooker::CookAssets(cookSettings);
		cookBenchmark.Stop();
		timings.cookTime = cookBenchmark.GetMicroSeconds();
	}

	CleanDestinationFolder(params.exportPath);

	if (!cookResult)
	{
		return CompileResult::ERROR_COOK_FAILED;
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
	case Platform::P_PS3:
		//result = CompileWSL(params); // Deprecated
		result = CompileInDocker(params);
		break;
	default:
		Debug::PrintError("[Compiler::Compile] No compile method for this platform!", true);
		break;
	}
	FrameLimiter::SetIsEnabled(false);
	// Send compile result
	OnCompileEnd(result, params);
	return result;
}

void Compiler::Init()
{
	UpdatePaths();

	CompilerParams params;
	params.buildPlatform = BuildSettingsMenu::GetBuildPlatform(Platform::P_Windows);
	const CompilerAvailability availability = CheckCompilerAvailability(params);
	if (availability == CompilerAvailability::MISSING_COMPILER_SOFTWARE)
	{
		Debug::PrintWarning("[Compiler::Init] The compiler is not correctly setup. Please check compiler settings at [Window->Engine Settings]", false);
	}
}

void Compiler::UpdatePaths()
{
	const std::string root = FileSystem::ConvertWindowsPathToBasicPath(fs::current_path().string());
	engineFolderLocation = root + "/";
	engineProjectLocation = engineFolderLocation;

	const size_t backSlashPos = engineProjectLocation.substr(0, engineProjectLocation.size() - 1).find_last_of('/');
	std::string visualStudioProjectPath = engineProjectLocation.erase(backSlashPos + 1) + "Xenity_Engine/";
	if (fs::exists(visualStudioProjectPath + "Xenity_Engine.vcxproj"))
	{
		engineProjectLocation = engineProjectLocation.erase(backSlashPos + 1) + "Xenity_Engine/";
	}

#if defined(_WIN64) 
	compilerExecFileName = MSVC_START_FILE_64BITS;
#else
	compilerExecFileName = MSVC_START_FILE_32BITS;
#endif
}

CompilerAvailability Compiler::CheckCompilerAvailability(const CompilerParams& params)
{
	UpdatePaths();

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
			if (!fs::exists(engineFolderLocation + ENGINE_EDITOR_FOLDER + ".lib") ||
				!fs::exists(engineFolderLocation + ENGINE_EDITOR_FOLDER + ".dll"))
			{
				error |= (int)CompilerAvailability::MISSING_ENGINE_COMPILED_LIB;
			}
		}
		else
		{
			if (!fs::exists(engineFolderLocation + ENGINE_GAME_FOLDER + ".lib") ||
				!fs::exists(engineFolderLocation + ENGINE_GAME_FOLDER + ".dll"))
			{
				error |= (int)CompilerAvailability::MISSING_ENGINE_COMPILED_LIB;
			}
		}
	}
	else if (params.buildPlatform.platform == Platform::P_PSP && params.buildType == BuildType::BuildAndRunGame)
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
			Debug::PrintError("[Compiler::CheckCompilerAvailability] Compiler executable " + std::string(compilerExecFileName) + " not found in " + EngineSettings::values.compilerPath);
		}
		if (error & (int)CompilerAvailability::MISSING_ENGINE_COMPILED_LIB)
		{
			Debug::PrintError("[Compiler::CheckCompilerAvailability] Compiled engine library not found in " + engineFolderLocation);
		}
		if (error & (int)CompilerAvailability::MISSING_PPSSPP)
		{
			Debug::PrintError("[Compiler::CheckCompilerAvailability] PPSSPP emulator not found at " + EngineSettings::values.ppssppExePath);
		}
	}
	return (CompilerAvailability)error;
}

CompileResult Compiler::CompilePlugin(Platform platform, const std::string& pluginPath)
{
	UpdatePaths();

	XASSERT(!pluginPath.empty(), "[Compiler::CompilePlugin] pluginPath is empty");

	if (pluginPath.empty())
		return CompileResult::ERROR_UNKNOWN;

	const std::string plugin_name = fs::path(pluginPath).parent_path().filename().string();

	CompilerParams params{};
	params.libraryName = "plugin_" + plugin_name;
	params.buildPlatform = BuildSettingsMenu::GetBuildPlatform(platform);
	params.buildType = BuildType::EditorHotReloading;
	params.sourcePath = pluginPath;
	params.tempPath = "plugins/.build/";
	params.exportPath = "plugins/";

	const CompileResult result = Compile(params);

	DeleteTempFiles(params);

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
			system("docker images -f r=u"); // Workaround for docker image inspect not working when resource saver mode is enabled

			const int checkDockerImageResult = system("docker image inspect xenity_1_0_0 2>nul 1>nul");
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
	XASSERT(!exportPath.empty(), "[Compiler::ExportProjectFiles] exportPath is empty");
	if (exportPath.empty())
		return false;

	const std::string projectCookedAssetsFolder = ProjectManager::GetProjectFolderPath() + ".build/cooked_assets/" + ASSETS_FOLDER;
	CopyUtils::AddCopyEntry(true, projectCookedAssetsFolder, exportPath + ASSETS_FOLDER);

	const std::string fileDataBasePath = ProjectManager::GetProjectFolderPath() + ".build/cooked_assets/db.xenb";
	CopyUtils::AddCopyEntry(false, fileDataBasePath, exportPath + "db.xenb");

	const std::string binaryFilePath = ProjectManager::GetProjectFolderPath() + ".build/cooked_assets/data.xenb";
	CopyUtils::AddCopyEntry(false, binaryFilePath, exportPath + "data.xenb");

	const std::string projectCookedPublicEngineAssetsFolder = ProjectManager::GetProjectFolderPath() + ".build/cooked_assets/" + PUBLIC_ENGINE_ASSETS_FOLDER;
	CopyUtils::AddCopyEntry(true, projectCookedPublicEngineAssetsFolder, exportPath + PUBLIC_ENGINE_ASSETS_FOLDER);

	CopyUtils::AddCopyEntry(false, ProjectManager::GetProjectFolderPath() + ".build/" + PROJECT_SETTINGS_FILE_NAME, exportPath + PROJECT_SETTINGS_FILE_NAME);

	const bool copyResult = CopyUtils::ExecuteCopyEntries();
	return copyResult;
}

CompileResult Compiler::CompileGame(const BuildPlatform buildPlatform, BuildType buildType, const std::string& exportPath)
{
	Benchmark totalCompilationBenchmark;
	totalCompilationBenchmark.Start();

	if (exportPath == ProjectManager::GetProjectFolderPath())
	{
		Debug::PrintError("[Compiler::CompileGame] Export path is the same as the project path");
		return CompileResult::ERROR_UNKNOWN;
	}

	UpdatePaths();

	XASSERT(!exportPath.empty(), "[Compiler::CompileGame] exportPath is empty");
	if (exportPath.empty())
		return CompileResult::ERROR_UNKNOWN;

	CompilerParams params{};
	params.libraryName = "game";
	params.buildPlatform = buildPlatform;
	params.buildType = buildType;
	params.sourcePath = ProjectManager::GetAssetFolderPath();
	params.tempPath = ProjectManager::GetProjectFolderPath() + ".build/";
	params.exportPath = exportPath;

	OnCompilationStartedEvent.Trigger(params);

	if (buildType == BuildType::BuildShadersAndGame)
	{
		params.buildType = BuildType::BuildShaders;
		params.tempPath = ProjectManager::GetProjectFolderPath() + ".shaders_build/";

		// Compile shaders
		const CompileResult shaderResult = Compile(params);

		if (shaderResult != CompileResult::SUCCESS)
		{
			DeleteTempFiles(params);
			return shaderResult;
		}
	}

	if (buildType == BuildType::BuildShadersAndGame)
	{
		params.buildType = BuildType::BuildGame;
		params.tempPath = ProjectManager::GetProjectFolderPath() + ".build/";
	}


	// Compile engine and game
	const CompileResult result = Compile(params);

	if (result != CompileResult::SUCCESS)
	{
		DeleteTempFiles(params);
		return result;
	}

	// Copy assets
	if (params.buildType != BuildType::EditorHotReloading)
	{
		if (buildPlatform.platform != Platform::P_PsVita) // PsVita files are included in the .vpk file
		{
			const bool copyResult = ExportProjectFiles(params.exportPath);
			if (!copyResult)
			{
				DeleteTempFiles(params);
				return CompileResult::ERROR_FILE_COPY;
			}
		}
	}

	DeleteTempFiles(params);

	// Open build folder if success
	if (params.buildType == BuildType::BuildGame)
	{
		Editor::OpenExplorerWindow(params.exportPath, false);
	}

	// Launch game
	if (params.buildType == BuildType::BuildAndRunGame)
	{
		auto t = std::thread(StartGame, params.buildPlatform.platform, params.exportPath);
		t.detach();
	}

	totalCompilationBenchmark.Stop();
	timings.totalCompileTime = totalCompilationBenchmark.GetMicroSeconds();

	PrintTimings();

	return result;
}

void Compiler::DeleteTempFiles(const CompilerParams& params)
{
	// Delete temp compiler folder content
	try
	{
		fs::remove_all(params.tempPath);
	}
	catch (const std::exception&) {}
}

void Compiler::CleanDestinationFolder(const std::string& exportPath)
{
	try
	{
		fs::remove(exportPath + "freetype.dll");
		fs::remove(exportPath + "game.dll");
		fs::remove(exportPath + "SDL3.dll");
		fs::remove(exportPath + "Xenity_Engine.dll");

		fs::remove(exportPath + "data.xenb");
		fs::remove(exportPath + "db.xenb");
		fs::remove(exportPath + "project_settings.json");
	}
	catch (const std::exception&) {}
}

void Compiler::PrintTimings()
{
	Debug::Print("Compilation timings:");
	Debug::Print("Cooking time: " + std::to_string(timings.cookTime) + " us (" + std::to_string(timings.cookTime / 1000000.0f) + " s)");
	Debug::Print("Docker preparation time: " + std::to_string(timings.prepareDockerTime) + " us" + " us (" + std::to_string(timings.prepareDockerTime / 1000000.0f) + " s)");
	Debug::Print("Docker code compile time: " + std::to_string(timings.dockerCompileTime) + " us" + " us (" + std::to_string(timings.dockerCompileTime / 1000000.0f) + " s)");
	Debug::Print("Docker shader compile time: " + std::to_string(timings.shaderCompileTime) + " us" + " us (" + std::to_string(timings.shaderCompileTime / 1000000.0f) + " s)");
	Debug::Print("Total compile time: " + std::to_string(timings.totalCompileTime) + " us" + " us (" + std::to_string(timings.totalCompileTime / 1000000.0f) + " s)");
}

void Compiler::CompileGameThreaded(const BuildPlatform buildPlatform, BuildType buildType, const std::string& exportPath)
{
	Debug::ClearDebugLogs();
	std::thread t = std::thread(CompileGame, buildPlatform, buildType, exportPath);
	t.detach();
}

void Compiler::HotReloadGame()
{
#if defined(_WIN32) || defined(_WIN64)
	Engine::s_game.reset();
	Engine::s_game = nullptr;

	// Prepare scene
	SceneManager::SaveScene(SaveSceneType::SaveSceneForHotReloading);
	SceneManager::ClearScene();

	// Reset registery and re-add basic components
	ClassRegistry::Reset();
	ClassRegistry::RegisterEngineComponents();
	ClassRegistry::RegisterEngineFileClasses();

	// Unload library
	DynamicLibrary::UnloadGameLibrary();

	// Compile game
	CompileResult result = Compiler::CompileGame(
		BuildSettingsMenu::GetBuildPlatform(Platform::P_Windows),
		BuildType::EditorHotReloading,
		ProjectManager::GetProjectFolderPath() + "temp/"
	);

	if (result == CompileResult::SUCCESS)
	{
		// Reload game
		DynamicLibrary::LoadGameLibrary(ProjectManager::GetProjectFolderPath() + "temp/" + "game_editor");

		// Create game instance
		Engine::s_game = DynamicLibrary::CreateGame();
		if (Engine::s_game)
		{
			Debug::Print("Game compilation done");
			Engine::s_game->Start();
		}
		else
		{
			// Should not happen here
			Debug::PrintError("[Compiler::HotReloadGame] Game compilation failed");
		}
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
	case CompileResult::ERROR_COOK_FAILED:
		Debug::PrintError("[Compiler::OnCompileEnd] Error when cooking files");
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
	case CompileResult::ERROR_DOCKER_SHADERS_COMPILATION:
		Debug::PrintError("[Compiler::OnCompileEnd] Unable to compile shaders on Docker (error in shader code)");
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

	if (params.buildType != BuildType::BuildShaders || result != CompileResult::SUCCESS)
	{
		OnCompilationEndedEvent.Trigger(params, result == CompileResult::SUCCESS);
	}
}

std::string WindowsPathToWSL(const std::string& path)
{
	XASSERT(!path.empty(), "[Compiler::WindowsPathToWSL] path is empty");
	if (path.empty())
		return "";

	std::string newPath = path;
	newPath[0] = tolower(newPath[0]);
	const int pathSize = (int)path.size();
	for (int i = 1; i < pathSize; i++)
	{
		if (newPath[i] == '/')
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

	fs::create_directory(params.tempPath + "source/");

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
		const std::string destFolder = params.tempPath + "source/" + files[i]->GetFolderPath().substr(sourcePathLen);
		fs::create_directories(destFolder);

		fs::copy_file(
			filePathToCopy,
			params.tempPath + "source/" + filePathToCopy.substr(sourcePathLen), // substr to remove all folders including the locations of the project/source files
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
		const std::string engineLibPath = engineFolderLocation + ENGINE_EDITOR_FOLDER + ".lib";

		// Copy engine editor lib to the temp build folder
		CopyUtils::AddCopyEntry(false, engineLibPath, params.tempPath + ENGINE_EDITOR_FOLDER + ".lib");
		// Copy editor header
		CopyUtils::AddCopyEntry(false, engineProjectLocation + "Source/xenity_editor.h", params.tempPath + "xenity_editor.h");
	}
	else // In build mode:
	{
		const std::string engineLibPath = engineFolderLocation + ENGINE_GAME_FOLDER + ".lib";
		const std::string engineDllPath = engineFolderLocation + ENGINE_GAME_FOLDER + ".dll";
		const std::string sdlDllPath = engineFolderLocation + "SDL3.dll";
		const std::string freetypeDllPath = engineFolderLocation + "freetype.dll";

		// Copy engine game lib to the temp build folder
		CopyUtils::AddCopyEntry(false, engineLibPath, params.tempPath + ENGINE_GAME_FOLDER + ".lib");
		// Copy all DLLs to the export folder
		CopyUtils::AddCopyEntry(false, engineDllPath, params.exportPath + ENGINE_GAME_FOLDER + ".dll");
		CopyUtils::AddCopyEntry(false, sdlDllPath, params.exportPath + "SDL3.dll");
		CopyUtils::AddCopyEntry(false, freetypeDllPath, params.exportPath + "freetype.dll");
	}

	// Copy engine headers to the temp build folder
	CopyUtils::AddCopyEntry(true, engineProjectLocation + "Source/engine/", params.tempPath + "engine/");
	CopyUtils::AddCopyEntry(false, engineProjectLocation + "Source/xenity.h", params.tempPath + "xenity.h");
	CopyUtils::AddCopyEntry(false, engineFolderLocation + "main.cpp", params.tempPath + "main.cpp");
	const bool headerCopyResult = CopyUtils::ExecuteCopyEntries();
	if (!headerCopyResult)
	{
		return CompileResult::ERROR_FILE_COPY;
	}

	std::shared_ptr<PlatformSettingsWindows> platformSettings = std::dynamic_pointer_cast<PlatformSettingsWindows>(params.buildPlatform.settings);
	if (platformSettings && platformSettings->icon)
	{
		// Copy game icon
		CopyUtils::AddCopyEntry(false, platformSettings->icon->m_file->GetPath(), params.tempPath + "logo.ico");
	}
	else
	{
		// Copy default icon
		CopyUtils::AddCopyEntry(false, engineFolderLocation + "logo.ico", params.tempPath + "logo.ico");
	}
	CopyUtils::AddCopyEntry(false, engineFolderLocation + "res.rc", params.tempPath + "res.rc");
	const bool iconCopyResult = CopyUtils::ExecuteCopyEntries();
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
		CopyUtils::AddCopyEntry(false, params.tempPath + editor_dll_name, params.exportPath + editor_dll_name);
	}
	else
	{
		const std::string dll_name = params.getDynamicLibraryName();
		CopyUtils::AddCopyEntry(false, params.tempPath + dll_name, params.exportPath + dll_name);
		CopyUtils::AddCopyEntry(false, params.tempPath + params.libraryName + ".exe", params.exportPath + params.libraryName + ".exe");
	}
	const bool gameCopyResult = CopyUtils::ExecuteCopyEntries();
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
	[[maybe_unused]] const int clearFolderResult = system("wsl sh -c 'rm -rf ~/XenityTestProject'");

	// Create folders
	[[maybe_unused]] const int createProjectFolderResult = system("wsl sh -c 'mkdir ~/XenityTestProject'");
	[[maybe_unused]] const int createBuildFolderResult = system("wsl sh -c 'mkdir ~/XenityTestProject/build'");

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
		if (compileFolderPath[i] == '/')
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
	const int result = system("docker build -t xenity_1_0_0 . 1>nul");
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
	Benchmark dockerPreparationBenchmark;
	dockerPreparationBenchmark.Start();
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
	[[maybe_unused]] const int stopResult = system("docker stop XenityEngineBuild");
	[[maybe_unused]] const int removeResult = system("docker remove XenityEngineBuild");

	std::string gameNameWithoutSpace = ProjectManager::GetGameName();
	std::replace(gameNameWithoutSpace.begin(), gameNameWithoutSpace.end(), ' ', '_');

	std::string prepareCompileCommand = "";
	if (params.buildPlatform.platform == Platform::P_PSP)
	{
		const std::shared_ptr<const PlatformSettingsPSP> platformSettings = std::dynamic_pointer_cast<PlatformSettingsPSP>(params.buildPlatform.settings);
		const std::string debugDefine = platformSettings->isDebugMode ? " -DDEBUG=1" : "";
		const std::string profilerDefine = platformSettings->enableProfiler ? " -DPROFILER=1" : "";
		prepareCompileCommand = "psp-cmake -DMODE=psp -DGAME_NAME=" + gameNameWithoutSpace + debugDefine + profilerDefine + " ..";
	}
	else if (params.buildPlatform.platform == Platform::P_PsVita)
	{
		const std::shared_ptr<const PlatformSettingsPsVita> platformSettings = std::dynamic_pointer_cast<PlatformSettingsPsVita>(params.buildPlatform.settings);
		const std::string debugDefine = platformSettings->isDebugMode ? " -DDEBUG=1" : "";
		const std::string profilerDefine = platformSettings->enableProfiler ? " -DPROFILER=1" : "";
		prepareCompileCommand = "cmake -DMODE=psvita -DGAME_NAME=" + gameNameWithoutSpace + " -DVITA_TITLEID=" + platformSettings->gameId + debugDefine + profilerDefine + " ..";
	}

	unsigned int threadNumber = std::thread::hardware_concurrency();
	if (threadNumber == 0) // This function may returns 0, use 1 instead
	{
		threadNumber = 1;
	}

	std::string createCommand = "docker create --name XenityEngineBuild xenity_1_0_0 /bin/bash -c -it \"cd /home/XenityBuild/build/ ; " + prepareCompileCommand + " ; cmake --build . -j" + std::to_string(threadNumber) + "\"";

	if (params.buildType == BuildType::BuildShaders)
	{
		createCommand = "docker create --name XenityEngineBuild xenity_1_0_0 /bin/bash -c -it \"cd /home/XenityBuild/ ; ./compile_shaders.sh\"";
	}
	else if (params.buildPlatform.platform == Platform::P_PS3)
	{
		std::string removeSourceCommand = "rm -r Source/editor/ ; rm Source/gl.c ; rm -r include/glad/ ; rm -r include/imgui/ ; rm -r include/implot/ ; rm -r include/SDL3/ ; rm -r include/KHR/ ;";

		createCommand = "docker create --name XenityEngineBuild xenity_1_0_0 /bin/bash -c -it \"cd /home/XenityBuild/ ; " + removeSourceCommand + "make -j" + std::to_string(threadNumber) + "\"";
	}

	[[maybe_unused]] const int createResult = system(createCommand.c_str());

	if (params.buildType != BuildType::BuildShaders)
	{
		// Copy source code and libraries
		const std::string copyEngineSourceCommand = "docker cp \"" + engineProjectLocation + "Source\" XenityEngineBuild:\"/home/XenityBuild/\"";
		[[maybe_unused]] const int copyCodeResult = system(copyEngineSourceCommand.c_str()); // Engine's source code + (game's code but to change later)
		const std::string copyEngineLibrariesCommand = "docker cp \"" + engineProjectLocation + "include\" XenityEngineBuild:\"/home/XenityBuild/\"";
		[[maybe_unused]] const int copyLibrariesResult = system(copyEngineLibrariesCommand.c_str()); // Engine's libraries
		const std::string copyMainCommand = "docker cp \"" + engineFolderLocation + "main.cpp\" XenityEngineBuild:\"/home/XenityBuild/Source/\"";
		[[maybe_unused]] const int copyMainResult = system(copyMainCommand.c_str()); // main.cpp file

		// Copy make file or CMakeLists.txt
		if (params.buildPlatform.platform == Platform::P_PS3)
		{
			const std::string copyMakeFileCommand = "docker cp \"" + engineFolderLocation + "Makefile.PS3\" XenityEngineBuild:\"/home/XenityBuild/Makefile\"";
			[[maybe_unused]] const int copyMakeFileCommandResult = system(copyMakeFileCommand.c_str()); // make file
		}
		else
		{
			const std::string copyCmakeCommand = "docker cp \"" + engineFolderLocation + "CMakeLists.txt\" XenityEngineBuild:\"/home/XenityBuild/\"";
			[[maybe_unused]] const int copyCmakelistsResult = system(copyCmakeCommand.c_str()); // Cmakelists file
		}

		// Copy cache for faster compilation
		if (params.buildPlatform.settings->useCompilationCache)
		{
			if (CompilerCache::CheckIfCacheExists(params.buildPlatform.platform))
			{
				const std::string copyCacheCommand = "docker cp \"" + CompilerCache::GetCachePath(params.buildPlatform.platform) + "build/\" XenityEngineBuild:\"/home/XenityBuild/\"";
				[[maybe_unused]] const int copyCacheResult = system(copyCacheCommand.c_str());
			}
		}

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
		[[maybe_unused]] const int copyGameSourceResult = system(copyGameSourceCommand.c_str());
	}
	else
	{
		FixCompileShadersScript();

		const std::string copyCompileShadersCommand = "docker cp \"" + engineFolderLocation + "compile_shaders_fixed.sh\" XenityEngineBuild:\"/home/XenityBuild/compile_shaders.sh\"";
		[[maybe_unused]] const int copyCompileShadersCommandResult = system(copyCompileShadersCommand.c_str()); // compile shaders script file
	}

	if (isCompilationCancelled)
	{
		return CompileResult::ERROR_COMPILATION_CANCELLED;
	}

	// Copy XMB/Livearea images
	if (params.buildPlatform.platform == Platform::P_PsVita ||
		params.buildPlatform.platform == Platform::P_PSP ||
		params.buildPlatform.platform == Platform::P_PS3)
	{
		CopyAssetsToDocker(params);
	}

	if (isCompilationCancelled)
	{
		return CompileResult::ERROR_COMPILATION_CANCELLED;
	}
	dockerPreparationBenchmark.Stop();
	timings.prepareDockerTime = dockerPreparationBenchmark.GetMicroSeconds();

	Benchmark dockerCompilationBenchmark;
	dockerCompilationBenchmark.Start();
	// Start compilation
	const std::string startCommand = "docker start -a XenityEngineBuild";
	[[maybe_unused]] const int startResult = system(startCommand.c_str());
	dockerCompilationBenchmark.Stop();
	timings.dockerCompileTime = dockerCompilationBenchmark.GetMicroSeconds();

	if (isCompilationCancelled)
	{
		return CompileResult::ERROR_COMPILATION_CANCELLED;
	}


	if (params.buildType == BuildType::BuildShaders)
	{
		if (startResult != 0)
		{
			return CompileResult::ERROR_DOCKER_SHADERS_COMPILATION;
		}

		const std::string copyCompiledShadersCommand = "docker cp XenityEngineBuild:\"/home/XenityBuild/shaders_to_compile/\" \"" + params.tempPath + "cooked_assets/" + "\"";
		[[maybe_unused]] const int copyCompiledShadersCommandResult = system(copyCompiledShadersCommand.c_str());
	}
	else
	{
		if (startResult != 0)
		{
			return CompileResult::ERROR_DOCKER_COMPILATION;
		}

		std::string fileName = "";
		if (params.buildPlatform.platform == Platform::P_PSP)
		{
			fileName = "EBOOT.PBP";
		}
		else if (params.buildPlatform.platform == Platform::P_PsVita)
		{
			fileName = gameNameWithoutSpace + ".vpk";
		}
		else if (params.buildPlatform.platform == Platform::P_PS3)
		{
			fileName = "XenityBuild.self";
		}

		// Copy cache files
		const std::string cacheFolder = params.tempPath + "docker_cache/";
		fs::create_directories(cacheFolder);
		const std::string copyCacheCommand = "docker cp XenityEngineBuild:\"/home/XenityBuild/build/\" \"" + cacheFolder + "\"";
		[[maybe_unused]] const int copyCacheCommandResult = system(copyCacheCommand.c_str());

		CompilerCache::UpdateCache(cacheFolder, params.buildPlatform.platform);

		// Copy final file
		std::string copyGameFileCommand = "docker cp XenityEngineBuild:\"/home/XenityBuild/build/" + fileName + "\" \"" + params.exportPath + fileName + "\"";
		if (params.buildPlatform.platform == Platform::P_PS3)
		{
			copyGameFileCommand = "docker cp XenityEngineBuild:\"/home/XenityBuild/" + fileName + "\" \"" + params.exportPath + fileName + "\"";
		}
		const int copyGameFileResult = system(copyGameFileCommand.c_str()); // Engine's source code + (game's code but to change later)

		// Copy prx file for build and run on psp hardware
		if (params.buildPlatform.platform == Platform::P_PSP)
		{
			std::string fileName2 = "hello.prx";
			const std::string copyGameFileCommand2 = "docker cp XenityEngineBuild:\"/home/XenityBuild/build/" + fileName2 + "\" \"" + params.exportPath + fileName2 + "\"";
			[[maybe_unused]] const int copyGameFileResult2 = system(copyGameFileCommand2.c_str()); // Engine's source code + (game's code but to change later)
		}
		else if (params.buildPlatform.platform == Platform::P_PS3)
		{
			std::string fileName2 = "XenityBuild.fake.self";
			const std::string copyGameFileCommand2 = "docker cp XenityEngineBuild:\"/home/XenityBuild/" + fileName2 + "\" \"" + params.exportPath + fileName2 + "\"";
			[[maybe_unused]] const int copyGameFileResult2 = system(copyGameFileCommand2.c_str()); // Engine's source code + (game's code but to change later)
		}

		if (copyGameFileResult != 0)
		{
			return CompileResult::ERROR_DOCKER_COMPILATION;
		}
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
		[[maybe_unused]] const int copyImagesResult = system(copyImagesCommand.c_str());
		if (platformSettings)
		{
			// Copy and replace images with custom ones
			if (platformSettings->backgroundImage)
			{
				const std::string copyBgImageCommand = "docker cp \"" + platformSettings->backgroundImage->m_file->GetPath() + "\" XenityEngineBuild:\"/home/XenityBuild/build/psp_images/BG.PNG\"";
				[[maybe_unused]] const int copyBgImageResult = system(copyBgImageCommand.c_str());
			}
			if (platformSettings->iconImage)
			{
				const std::string copyIconImageCommand = "docker cp \"" + platformSettings->iconImage->m_file->GetPath() + "\" XenityEngineBuild:\"/home/XenityBuild/build/psp_images/ICON.PNG\"";
				[[maybe_unused]] const int copyIconImageResult = system(copyIconImageCommand.c_str());
			}
			if (platformSettings->previewImage)
			{
				const std::string copyPreviewImageCommand = "docker cp \"" + platformSettings->previewImage->m_file->GetPath() + "\" XenityEngineBuild:\"/home/XenityBuild/build/psp_images/PREVIEW.PNG\"";
				[[maybe_unused]] const int copyPreviewImageResult = system(copyPreviewImageCommand.c_str());
			}
		}
	}
	else if (params.buildPlatform.platform == Platform::P_PsVita)
	{
		std::shared_ptr<PlatformSettingsPsVita> platformSettings = std::dynamic_pointer_cast<PlatformSettingsPsVita>(params.buildPlatform.settings);
		// Copy default psp images
		const std::string copyImagesCommand = "docker cp \"" + engineFolderLocation + "psvita_images\" XenityEngineBuild:\"/home/XenityBuild/\"";
		[[maybe_unused]] const int copyImagesResult = system(copyImagesCommand.c_str());

		const std::string copyGameEngineAssetsCommand = "docker cp \"" + ProjectManager::GetEngineAssetFolderPath().substr(0, ProjectManager::GetEngineAssetFolderPath().size() - 1) + "\" XenityEngineBuild:\"/home/XenityBuild/\"";
		[[maybe_unused]] const int copyGameEngineAssetsResult = system(copyGameEngineAssetsCommand.c_str());

		const std::string copyGamePublicEngineAssetsCommand = "docker cp \"" + ProjectManager::GetPublicEngineAssetFolderPath().substr(0, ProjectManager::GetPublicEngineAssetFolderPath().size() - 1) + "\" XenityEngineBuild:\"/home/XenityBuild/\"";
		[[maybe_unused]] const int copyGamePublicEngineAssetsResult = system(copyGamePublicEngineAssetsCommand.c_str());

		const std::string copyProjectSettingsCommand = "docker cp \"" + ProjectManager::GetProjectFolderPath() + ".build/" + PROJECT_SETTINGS_FILE_NAME + "\" XenityEngineBuild:\"/home/XenityBuild/" + PROJECT_SETTINGS_FILE_NAME + "\"";
		[[maybe_unused]] const int copyProjectSettingsResult = system(copyProjectSettingsCommand.c_str());

		const std::string copydbFileCommand = "docker cp \"" + ProjectManager::GetProjectFolderPath() + ".build/cooked_assets/db.xenb" + "\" XenityEngineBuild:\"/home/XenityBuild/" + "db.xenb" + "\"";
		[[maybe_unused]] const int copydbFileResult = system(copydbFileCommand.c_str());

		const std::string copyxenbFileCommand = "docker cp \"" + ProjectManager::GetProjectFolderPath() + ".build/cooked_assets/data.xenb" + "\" XenityEngineBuild:\"/home/XenityBuild/" + "data.xenb" + "\"";
		[[maybe_unused]] const int copyxenbFileResult = system(copyxenbFileCommand.c_str());

		[[maybe_unused]] const bool copyResult = ExportProjectFiles(params.tempPath);
		const std::string copyGameAssetsCommand = "docker cp \"" + params.tempPath + "assets\" XenityEngineBuild:\"/home/XenityBuild/\"";
		[[maybe_unused]] const int copyGameAssetsResult = system(copyGameAssetsCommand.c_str());

		if (platformSettings)
		{
			// Copy and replace images with custom ones
			if (platformSettings->backgroundImage)
			{
				const std::string copyBgImageCommand = "docker cp \"" + platformSettings->backgroundImage->m_file->GetPath() + "\" XenityEngineBuild:\"/home/XenityBuild/psvita_images/bg.png\"";
				[[maybe_unused]] const int copyBgImageResult = system(copyBgImageCommand.c_str());
			}
			if (platformSettings->iconImage)
			{
				const std::string copyIconImageCommand = "docker cp \"" + platformSettings->iconImage->m_file->GetPath() + "\" XenityEngineBuild:\"/home/XenityBuild/psvita_images/icon0.png\"";
				[[maybe_unused]] const int copyIconImageResult = system(copyIconImageCommand.c_str());
			}
			if (platformSettings->startupImage)
			{
				const std::string copyPreviewImageCommand = "docker cp \"" + platformSettings->startupImage->m_file->GetPath() + "\" XenityEngineBuild:\"/home/XenityBuild/psvita_images/startup.png\"";
				[[maybe_unused]] const int copyPreviewImageResult = system(copyPreviewImageCommand.c_str());
			}
		}
	}
	else if (params.buildPlatform.platform == Platform::P_PS3)
	{
		// Copy shaders to compile
		const std::string copyShadersCommand = "docker cp \"" + params.tempPath + "cooked_assets/shaders_to_compile\" XenityEngineBuild:\"/home/XenityBuild\"";
		[[maybe_unused]] const int copyShadersCommandResult = system(copyShadersCommand.c_str());
	}
}

void Compiler::FixCompileShadersScript()
{
	// In case the script has windows line endings, remove them to avoid errors on linux
	std::shared_ptr<File> shaderScriptFile = FileSystem::MakeFile(engineFolderLocation + "compile_shaders.sh");
	if (!shaderScriptFile->Open(FileMode::ReadOnly)) 
	{
		Debug::PrintError("[Compiler::FixCompileShadersScript] Failed to open compile_shaders.sh");
		return;
	}
	std::string scriptText = shaderScriptFile->ReadAll();
	shaderScriptFile->Close();

	size_t scriptTextSize = scriptText.size();
	for (size_t i = 0; i < scriptTextSize; i++)
	{
		if (scriptText[i] == '\r')
		{
			scriptText.erase(i, 1);
			scriptTextSize--;
		}
	}

	FileSystem::s_fileSystem->Delete(engineFolderLocation + "compile_shaders_fixed.sh");
	// Write the new script
	std::shared_ptr<File> updatedShaderScriptFile = FileSystem::MakeFile(engineFolderLocation + "compile_shaders_fixed.sh");
	if (!updatedShaderScriptFile->Open(FileMode::WriteCreateFile))
	{
		Debug::PrintError("[Compiler::FixCompileShadersScript] Failed to create compile_shaders_fixed.sh");
		return;
	}
	updatedShaderScriptFile->Write(scriptText);
	updatedShaderScriptFile->Close();
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
	command += "cl /std:c++17 /MP /EHsc /MDd /DDEBUG /DIMPORT /DNOMINMAX"; // Start compilation
#else
	command += "cl /std:c++17 /O2 /MP /EHsc /MD /DIMPORT /DNOMINMAX"; // Start compilation
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
		command += " " + std::string(ENGINE_GAME_FOLDER) + ".lib";
	}
	else
	{
		command += " " + std::string(ENGINE_EDITOR_FOLDER) + ".lib";
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
	command = "cl /Fe\"" + params.libraryName + ".exe\" res.res /std:c++17 /MP /EHsc /DNOMINMAX";
#if !defined(DEBUG)
	command += " /O2";
#endif
	command += " -I \"" + engineProjectLocation + "include\"";
	command += " -I \"" + engineProjectLocation + "Source\"";
	command += " main.cpp " + std::string(ENGINE_GAME_FOLDER) + ".lib";
	//command += " >nul"; // Mute output
	return command;
}

void Compiler::StartGame(Platform platform, const std::string& exportPath)
{
	XASSERT(!exportPath.empty(), "[Compiler::StartGame] exportPath is empty");
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