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

// Editor
#include <editor/editor.h>
#include <editor/ui/menus/compiling_menu.h>

namespace fs = std::filesystem;

std::vector<CopyEntry> Compiler::copyEntries;

void Compiler::AddCopyEntry(bool isFolder, const std::string& source, const std::string& dest)
{
	CopyEntry entry;
	entry.isFolder = isFolder;
	entry.sourcePath = source;
	entry.destPath = dest;

	copyEntries.push_back(entry);
}

bool Compiler::ExecuteCopyEntries()
{
	bool success = true;
	for (CopyEntry entry : copyEntries)
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
			Debug::PrintError("[Compiler::ExecuteCopyEntries] Cannot copy " + entry.sourcePath + " to " + entry.destPath);
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
#define MSVC_START_FILE "vcvars64.bat"

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
	// Ensure path are absolute
	const std::string root = fs::current_path().string();
	params.tempPath = MakePathAbsolute(params.tempPath, root);
	params.sourcePath = MakePathAbsolute(params.sourcePath, root);
	params.exportPath = MakePathAbsolute(params.exportPath, root);

	// Print parameters
	Debug::Print(
		"[Compiler::Compile] Preparing:"
		"\n- Platform: " + std::to_string((int)params.platform)
		+ "\n- Build Type: " + std::to_string((int)params.buildType)
		+ "\n- Temporary Path: " + params.tempPath
		+ "\n- Source Path: " + params.sourcePath
		+ "\n- Export Path: " + params.exportPath
		+ "\n- Library Name: " + params.libraryName
		+ "\n- Editor DLL: " + params.getEditorDynamicLibraryName()
		+ "\n- Runtime DLL: " + params.getDynamicLibraryName()
	);

	const CompilerAvailability availability = CheckCompilerAvailability(params);
	if (availability != CompilerAvailability::AVAILABLE)
	{
		OnCompileEnd(CompileResult::ERROR_COMPILER_AVAILABILITY);
		return CompileResult::ERROR_COMPILER_AVAILABILITY;
	}

	// Clean temporary directory
	try
	{
		fs::remove_all(params.tempPath);
		fs::create_directory(params.tempPath);
	}
	catch (const std::exception&)
	{
		Debug::PrintWarning("[Compiler::Compile] Unable to clear the compilation folder");
	}

	// Compile depending on platform
	CompileResult result = CompileResult::ERROR_UNKNOWN;
	switch (params.platform)
	{
	case Platform::P_Windows:
		result = CompileWindows(params);
		break;
	case Platform::P_PSP:
	case Platform::P_PsVita:
		result = CompileWSL(params);
		break;
	default:
		Debug::PrintError("[Compiler::Compile] No compile method for this platform!");
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
	OnCompileEnd(result);
	return result;
}

CompilerAvailability Compiler::CheckCompilerAvailability(const CompilerParams& params)
{
	int error = 0;

	// Check if the compiler executable exists
	if (!fs::exists(EngineSettings::compilerPath + MSVC_START_FILE))
	{
		error |= (int)CompilerAvailability::MISSING_COMPILER_SOFTWARE;
	}

	// Check if the engine compiled library exists
	if (params.platform == Platform::P_Windows)
	{
		if (params.buildType == BuildType::EditorHotReloading)
		{
			if (!fs::exists(EngineSettings::engineProjectPath + ENGINE_EDITOR + ".lib") ||
				!fs::exists(EngineSettings::engineProjectPath + ENGINE_EDITOR + ".dll"))
			{
				error |= (int)CompilerAvailability::MISSING_ENGINE_COMPILED_LIB;
			}
		}
		else
		{
			if (!fs::exists(EngineSettings::engineProjectPath + ENGINE_GAME + ".lib") ||
				!fs::exists(EngineSettings::engineProjectPath + ENGINE_GAME + ".dll"))
			{
				error |= (int)CompilerAvailability::MISSING_ENGINE_COMPILED_LIB;
			}
		}
	}
	else if (params.platform == Platform::P_PSP)
	{
		if (!fs::exists(EngineSettings::ppssppExePath))
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
			Debug::PrintError("[Compiler::CheckCompilerAvailability] Compiler executable " + std::string(MSVC_START_FILE) + " not found in " + EngineSettings::compilerPath);
		}
		if (error & (int)CompilerAvailability::MISSING_ENGINE_COMPILED_LIB)
		{
			Debug::PrintError("[Compiler::CheckCompilerAvailability] Compiled engine library not found in " + EngineSettings::engineProjectPath);
		}
		if (error & (int)CompilerAvailability::MISSING_PPSSPP)
		{
			Debug::PrintError("[Compiler::CheckCompilerAvailability] PPSSPP emulator not found at " + EngineSettings::ppssppExePath);
		}
	}
	return (CompilerAvailability)error;
}

CompileResult Compiler::CompilePlugin(Platform platform, const std::string& pluginPath)
{
	const std::string plugin_name = fs::path(pluginPath).parent_path().filename().string();

	CompilerParams params{};
	params.libraryName = "plugin_" + plugin_name;
	params.platform = platform;
	params.buildType = BuildType::EditorHotReloading;
	params.sourcePath = pluginPath;
	params.tempPath = "plugins\\.build\\";
	params.exportPath = "plugins\\";

	const CompileResult result = Compile(params);
	return result;
}

bool Compiler::ExportProjectFiles(const CompilerParams& params)
{
	std::string projectFolder = ProjectManager::GetProjectFolderPath() + ASSETS_FOLDER;
	int projectFolderPathLen = projectFolder.size();
	std::vector<uint64_t> ids = ProjectManager::GetAllUsedFileByTheGame();
	int idsCount = ids.size();
	for (int i = 0; i < idsCount; i++)
	{
		FileAndPath* filePath = ProjectManager::GetFileById(ids[i]);
		if (filePath)
		{
			std::string newPath = filePath->path.substr(projectFolderPathLen, filePath->path.size() - projectFolderPathLen);
			AddCopyEntry(false, filePath->path, params.exportPath + ASSETS_FOLDER + newPath);
			AddCopyEntry(false, filePath->path + ".meta", params.exportPath + ASSETS_FOLDER + newPath + ".meta");

			std::string folderToCreate = (params.exportPath + ASSETS_FOLDER + newPath);
			folderToCreate = folderToCreate.substr(0, folderToCreate.find_last_of("\\"));
			fs::create_directories(folderToCreate);
		}
	}

	AddCopyEntry(true, ProjectManager::GetEngineAssetFolderPath(), params.exportPath + ENGINE_ASSETS_FOLDER);
	AddCopyEntry(false, ProjectManager::GetProjectFolderPath() + PROJECT_SETTINGS_FILE_NAME, params.exportPath + PROJECT_SETTINGS_FILE_NAME);
	const bool copyResult = ExecuteCopyEntries();
	return copyResult;
}

CompileResult Compiler::CompileGame(Platform platform, BuildType buildType, const std::string& exportPath)
{
	CompilerParams params{};
	params.libraryName = "game";
	params.platform = platform;
	params.buildType = buildType;
	params.sourcePath = ProjectManager::GetAssetFolderPath();
	params.tempPath = ProjectManager::GetProjectFolderPath() + ".build\\";
	params.exportPath = exportPath;

	// Compile
	const CompileResult result = Compile(params);
	if (result != CompileResult::SUCCESS) return result;

	// Copy assets
	if (params.buildType != BuildType::EditorHotReloading)
	{
		const bool copyResult = ExportProjectFiles(params);
		if (!copyResult)
		{
			return CompileResult::ERROR_FILE_COPY;
		}
	}

	// Launch game
	if (params.buildType == BuildType::BuildAndRunGame)
	{
		auto t = std::thread(StartGame, params.platform, params.exportPath);
		t.detach();
	}

	return result;
}

void Compiler::CompileGameThreaded(Platform platform, BuildType buildType, const std::string& exportPath)
{
	auto compilingMenu = Editor::GetMenu<CompilingMenu>();
	compilingMenu->OpenPopup();
	std::thread t = std::thread(CompileGame, platform, buildType, exportPath);
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
		Platform::P_Windows,
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
		SceneManager::RestoreSceneHotReloading();
	}
	else
	{
		Debug::PrintError("[Compiler::HotReloadGame] Game compilation failed");
		SceneManager::RestoreSceneHotReloading();
	}
#endif
}

void Compiler::OnCompileEnd(CompileResult result)
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
		Debug::PrintError("[Compiler::OnCompileEnd] Unable to compile on WSL");
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

	default:
		Debug::PrintError("[Compiler::OnCompileEnd] Unable to compile (unkown error)");
		break;
	}

	auto compilingMenu = Editor::GetMenu<CompilingMenu>();
	if (compilingMenu)
	{
		compilingMenu->ClosePopup();
	}
}

std::string WindowsPathToWSL(const std::string& path)
{
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

CompileResult Compiler::CompileWindows(const CompilerParams& params)
{
	if (params.buildType == BuildType::EditorHotReloading) // In hot reloading mode:
	{
		const std::string engineLibPath = EngineSettings::engineProjectPath + ENGINE_EDITOR + ".lib";

		// Copy engine editor lib to the temp build folder
		AddCopyEntry(false, engineLibPath, params.tempPath + ENGINE_EDITOR + ".lib");
		// Copy editor header
		AddCopyEntry(false, EngineSettings::engineProjectPath + "Source\\xenity_editor.h", params.tempPath + "xenity_editor.h");
	}
	else // In build mode:
	{
		const std::string engineLibPath = EngineSettings::engineProjectPath + ENGINE_GAME + ".lib";
		const std::string engineDllPath = EngineSettings::engineProjectPath + ENGINE_GAME + ".dll";
		const std::string sdlDllPath = EngineSettings::engineProjectPath + "SDL2.dll";
		const std::string glfwDllPath = EngineSettings::engineProjectPath + "glfw3.dll";
		const std::string freetypeDllPath = EngineSettings::engineProjectPath + "freetype.dll";

		// Copy engine game lib to the temp build folder
		AddCopyEntry(false, engineLibPath, params.tempPath + ENGINE_GAME + ".lib");
		// Copy all DLLs to the export folder
		AddCopyEntry(false, engineDllPath, params.exportPath + ENGINE_GAME + ".dll");
		AddCopyEntry(false, sdlDllPath, params.exportPath + "SDL2.dll");
		AddCopyEntry(false, glfwDllPath, params.exportPath + "glfw3.dll");
		AddCopyEntry(false, freetypeDllPath, params.exportPath + "freetype.dll");
	}

	// Copy engine headers to the temp build folder
	AddCopyEntry(true, EngineSettings::engineProjectPath + "Source\\engine\\", params.tempPath + "engine\\");
	AddCopyEntry(false, EngineSettings::engineProjectPath + "Source\\xenity.h", params.tempPath + "xenity.h");
	AddCopyEntry(false, EngineSettings::engineProjectPath + "Source\\main.cpp", params.tempPath + "main.cpp");
	const bool headerCopyResult = ExecuteCopyEntries();
	if (!headerCopyResult)
	{
		return CompileResult::ERROR_FILE_COPY;
	}

	// Copy source code
	try
	{
		fs::create_directory(params.tempPath + "source\\");

		for (const auto& file : fs::directory_iterator(params.sourcePath))
		{
			// Check is file
			if (!file.is_regular_file()) continue;

			// Check extension
			const std::string ext = file.path().extension().string();
			if (ext != ".h" && ext != ".cpp") continue;

			// Copy file
			const std::string path = file.path().string();
			const std::string fileName = file.path().filename().string();
			fs::copy_file(
				path,
				params.tempPath + "source\\" + fileName,
				fs::copy_options::overwrite_existing
			);
		}
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
	command += GetCompileGameLibCommand(params);
	if (params.buildType != BuildType::EditorHotReloading)
	{
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
	const std::string dll_name = params.getDynamicLibraryName();
	const std::string editor_dll_name = params.getEditorDynamicLibraryName();
	if (params.buildType == BuildType::EditorHotReloading)
	{
		AddCopyEntry(false, params.tempPath + editor_dll_name, params.exportPath + editor_dll_name);
	}
	else
	{
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

CompileResult Compiler::CompileWSL(const CompilerParams& params)
{
	const std::string convertedEnginePath = WindowsPathToWSL(EngineSettings::engineProjectPath);
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
	const std::string copyCmakeCommand = "wsl sh -c 'cp -R /mnt/" + convertedEnginePath + "CMakeLists.txt ~/XenityTestProject'";
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
	if (params.platform == Platform::P_PSP)
		compileCommand += " && psp-cmake -DMODE=psp ..";
	else if (params.platform == Platform::P_PsVita)
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
	if (params.platform == Platform::P_PSP)
		copyGameCommand = "wsl sh -c 'cp ~/\"XenityTestProject/build/EBOOT.PBP\" \"" + compileFolderPath + "/EBOOT.PBP\"'";
	else if (params.platform == Platform::P_PsVita)
		copyGameCommand = "wsl sh -c 'cp ~/\"XenityTestProject/build/hello.vpk\" \"" + compileFolderPath + "/hello.vpk\"'";

	const int copyGameResult = system(copyGameCommand.c_str());
	if (copyGameResult != 0)
	{
		return CompileResult::ERROR_FINAL_GAME_FILES_COPY;
	}

	// Copy game assets
	const bool gameCopyResult = ExportProjectFiles(params);
	if (!gameCopyResult)
	{
		return CompileResult::ERROR_FINAL_GAME_FILES_COPY;
	}

	return CompileResult::SUCCESS;
}

std::string Compiler::GetStartCompilerCommand()
{
	const std::string path = EngineSettings::compilerPath;

	std::string command;
	if (fs::path(path).is_absolute())
	{
		command += path.substr(0, 2) + " && "; // Go to the compiler folder
	}
	command += "cd \"" + EngineSettings::compilerPath + "\""; // Go to the compiler folder
	command += " && " + std::string(MSVC_START_FILE); // Start the compiler
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

std::string Compiler::GetCompileGameLibCommand(const CompilerParams& params)
{
	std::string command = "";
	command += "cl /std:c++20 /MP /EHsc /MDd /DIMPORT"; // Start compilation
	if (params.buildType == BuildType::EditorHotReloading)
	{
		command += " /DEDITOR";
	}

	// Add include directories
	command += " -I \"" + EngineSettings::engineProjectPath + "include\"";
	command += " -I \"" + EngineSettings::engineProjectPath + "Source\"";

	const std::string folder = params.tempPath + "source\\";
	command += " /LD \"" + folder + "*.cpp\"";
	if (params.buildType != BuildType::EditorHotReloading)
	{
		command += " " + std::string(ENGINE_GAME) + ".lib";
	}
	else
	{
		command += " " + std::string(ENGINE_EDITOR) + ".lib";
	}
	command += " /link";
	command += " /implib:" + params.libraryName + ".lib";
	command += " /DEBUG";
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

std::string Compiler::GetCompileExecutableCommand(const CompilerParams& params)
{
	std::string command;
	//Buid game exe
	command = "cl /Fe\"" + params.libraryName + ".exe\" /std:c++20 /MP /EHsc";
	command += " -I \"" + EngineSettings::engineProjectPath + "include\"";
	command += " -I \"" + EngineSettings::engineProjectPath + "Source\"";
	command += " main.cpp " + std::string(ENGINE_GAME) + ".lib";
	//command += " >nul"; // Mute output
	return command;
}

void Compiler::StartGame(Platform platform, const std::string& exportPath)
{
	std::string command = "";
	if (platform == Platform::P_Windows)
	{
		const std::string fileName = ProjectManager::GetGameName();
		command = "cd \"" + exportPath + "\"" + " && " + "\"" + fileName + ".exe\"";
	}
	else if (platform == Platform::P_PSP)
	{
		command = "(\"" + EngineSettings::ppssppExePath + "\" \"" + exportPath + "EBOOT.PBP\")";
	}

	if (!command.empty())
		system(command.c_str());
}