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

// Editor
#include <editor/editor.h>
#include <editor/ui/menus/compiling_menu.h>

namespace fs = std::filesystem;

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
	std::string root = fs::current_path().string();
	params.tempPath = MakePathAbsolute(params.tempPath, root);
	params.sourcePath = MakePathAbsolute(params.sourcePath, root);
	params.exportPath = MakePathAbsolute(params.exportPath, root);

	// Print parameters
	Debug::Print(
		"[Compiler::Compile] Preparing:" 
		"\n- Platform: " + std::to_string( (int)params.platform )
		+ "\n- Build Type: " + std::to_string( (int)params.buildType )
		+ "\n- Temporary Path: " + params.tempPath
		+ "\n- Source Path: " + params.sourcePath
		+ "\n- Export Path: " + params.exportPath
		+ "\n- Library Name: " + params.libraryName
		+ "\n- Editor DLL: " + params.getEditorDynamicLibraryName()
		+ "\n- Runtime DLL: " + params.getDynamicLibraryName()
	);

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

	// Send compile result
	OnCompileEnd(result);
	return result;
}

CompileResult Compiler::CompilePlugin(Platform platform, const std::string& pluginPath)
{
	std::string plugin_name = fs::path( pluginPath ).parent_path().filename().string();

	CompilerParams params {};
	params.libraryName = "plugin_" + plugin_name;
	params.platform = platform;
	params.buildType = BuildType::EditorHotReloading;
	params.sourcePath = pluginPath;
	params.tempPath = "plugins\\.build\\";
	params.exportPath = "plugins\\";

	CompileResult result = Compile(params);
	return result;
}

CompileResult Compiler::CompileGame(Platform platform, BuildType buildType, const std::string& exportPath)
{
	CompilerParams params {};
	params.libraryName = "game";
	params.platform = platform;
	params.buildType = buildType;
	params.sourcePath = ProjectManager::GetAssetFolderPath();
	params.tempPath = ProjectManager::GetProjectFolderPath() + ".build\\";
	params.exportPath = exportPath;

	// Compile
	CompileResult result = Compile(params);
	if (result != CompileResult::SUCCESS) return result;

	// Copy assets
	if(params.buildType != BuildType::EditorHotReloading)
	{
		fs::copy(
			ProjectManager::GetAssetFolderPath(), 
			params.exportPath + "assets\\", 
			fs::copy_options::overwrite_existing | fs::copy_options::recursive
		);
		fs::copy(
			ProjectManager::GetEngineAssetFolderPath(), 
			params.exportPath + "engine_assets\\", 
			fs::copy_options::overwrite_existing | fs::copy_options::recursive
		);
		fs::copy_file(
			ProjectManager::GetProjectFolderPath() + PROJECT_SETTINGS_FILE_NAME, 
			params.exportPath + PROJECT_SETTINGS_FILE_NAME, 
			fs::copy_options::overwrite_existing
		);
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
	Editor::compilingMenu->OpenPopup();
	std::thread t = std::thread(CompileGame, platform, buildType, exportPath);
	t.detach();
}

void Compiler::HotReloadGame()
{
#if defined(_WIN32) || defined(_WIN64)
	Engine::game.reset();
	Engine::game = nullptr;

	// Prepare scene
	SceneManager::SaveScene(SaveSceneForHotReloading);
	SceneManager::ClearScene();

	// Reset registeries
	ClassRegistry::Reset();
	Engine::RegisterEngineComponents();

	// Unload library
	DynamicLibrary::UnloadGameLibrary();

	// Compile game
	Compiler::CompileGame(
		Platform::P_Windows, 
		BuildType::EditorHotReloading, 
		ProjectManager::GetProjectFolderPath()
	);
	
	// Reload game
	DynamicLibrary::LoadGameLibrary(ProjectManager::GetProjectFolderPath() + "game_editor");
	
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
		Debug::Print("Game compiled successfully!");
		break;
	case CompileResult::ERROR_UNKNOWN:
		Debug::PrintError("[Compiler::OnCompileEnd] Unable to compile (unkown error)");
		break;
	case CompileResult::ERROR_ENGINE_GAME_LIB_MISSING:
		Debug::PrintError("[Compiler::OnCompileEnd] Missing engine_game.lib");
		break;
	case CompileResult::ERROR_ENGINE_EDITOR_LIB_MISSING:
		Debug::PrintError("[Compiler::OnCompileEnd] Missing engine_editor.lib");
		break;
	case CompileResult::ERROR_LIB_DLLS_MISSING:
		Debug::PrintError("[Compiler::OnCompileEnd] Missing one of Dlls");
		break;
	case CompileResult::ERROR_ENGINE_HEADERS_COPY:
		Debug::PrintError("[Compiler::OnCompileEnd] Error when copying engine headers");
		break;
	case CompileResult::ERROR_GAME_CODE_COPY:
		Debug::PrintError("[Compiler::OnCompileEnd] Error when copying game's code");
		break;
	case CompileResult::ERROR_FINAL_GAME_FILES_COPY:
		Debug::PrintError("[Compiler::OnCompileEnd] Error when copying game's files");
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

	default:
		Debug::PrintError("[Compiler::OnCompileEnd] Unable to compile (unkown error)");
		break;
	}

	if (Editor::compilingMenu)
	{
		Editor::compilingMenu->ClosePopup();
	}
}

std::string WindowsPathToWSL(const std::string& path) 
{
	std::string newPath = path;
	newPath[0] = tolower(newPath[0]);
	int pathSize = (int)path.size();
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

CompileResult Compiler::CompileWindows( const CompilerParams& params )
{
	if (params.buildType == BuildType::EditorHotReloading) // In hot reloading mode:
	{
		std::string engineLibPath = EngineSettings::engineProjectPath + "engine_editor.lib";

		try
		{
			// Copy engine editor lib to the temp build folder
			fs::copy_file(
				engineLibPath, 
				params.tempPath + "engine_editor.lib", 
				fs::copy_options::overwrite_existing
			);
		}
		catch (const std::exception&)
		{
			return CompileResult::ERROR_ENGINE_EDITOR_LIB_MISSING;
		}

		// Copy engine headers
		try
		{
			fs::copy(
				EngineSettings::engineProjectPath + "Source\\engine\\", 
				params.tempPath + "engine\\", 
				fs::copy_options::overwrite_existing | fs::copy_options::recursive
			);
			fs::copy(
				EngineSettings::engineProjectPath + "Source\\editor\\", 
				params.tempPath + "editor\\", 
				fs::copy_options::overwrite_existing | fs::copy_options::recursive
			);
			fs::copy_file(
				EngineSettings::engineProjectPath + "Source\\xenity.h", 
				params.tempPath + "xenity.h", 
				fs::copy_options::overwrite_existing
			);
			fs::copy_file(
				EngineSettings::engineProjectPath + "Source\\xenity_editor.h", 
				params.tempPath + "xenity_editor.h", 
				fs::copy_options::overwrite_existing
			);
		}
		catch (const std::exception&)
		{
			return CompileResult::ERROR_ENGINE_HEADERS_COPY;
		}
	}
	else // In build mode:
	{
		std::string engineLibPath = EngineSettings::engineProjectPath + "engine_game.lib";
		std::string engineDllPath = EngineSettings::engineProjectPath + "engine_game.dll";
		std::string sdlDllPath = EngineSettings::engineProjectPath + "SDL2.dll";
		std::string glfwDllPath = EngineSettings::engineProjectPath + "glfw3.dll";

		// Copy engine game lib to the temp build folder
		try
		{
			fs::copy_file(
				engineLibPath, 
				params.tempPath + "engine_game.lib", 
				fs::copy_options::overwrite_existing
			);
		}
		catch (const std::exception&)
		{
			return CompileResult::ERROR_ENGINE_GAME_LIB_MISSING;
		}

		// Copy all DLLs to the export folder
		try
		{
			fs::copy_file(
				engineDllPath, 
				params.exportPath + "engine_game.dll", 
				fs::copy_options::overwrite_existing
			);
			fs::copy_file(
				sdlDllPath, 
				params.exportPath + "SDL2.dll", 
				fs::copy_options::overwrite_existing
			);
			fs::copy_file(
				glfwDllPath, 
				params.exportPath + "glfw3.dll", 
				fs::copy_options::overwrite_existing
			);
		}
		catch (const std::exception&)
		{
			return CompileResult::ERROR_LIB_DLLS_MISSING;
		}

		// Copy engine headers to the temp build folder
		try
		{
			fs::copy(
				EngineSettings::engineProjectPath + "Source\\engine\\", 
				params.tempPath + "engine\\",
				fs::copy_options::overwrite_existing | fs::copy_options::recursive
			);
			fs::copy_file(
				EngineSettings::engineProjectPath + "Source\\xenity.h", 
				params.tempPath + "xenity.h", 
				fs::copy_options::overwrite_existing
			);

			fs::copy_file(
				EngineSettings::engineProjectPath + "Source\\main.cpp", 
				params.tempPath + "main.cpp", 
				fs::copy_options::overwrite_existing
			);
		}
		catch (const std::exception&)
		{
			return CompileResult::ERROR_ENGINE_HEADERS_COPY;
		}
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
			std::string ext = file.path().extension().string();
			if (ext != ".h" && ext != ".cpp") continue;

			// Copy file
			std::string path = file.path().string();
			std::string fileName = file.path().filename().string();
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

	// Run compilation
	int buildResult = system(command.c_str());
	if (buildResult != 0)
	{
		Debug::PrintError("[Compiler::Compile] Command: " + command);
		return CompileResult::ERROR_UNKNOWN;
	}

	// Copy DLLs to export path
	try
	{
		const std::string dll_name = params.getDynamicLibraryName();
		const std::string editor_dll_name = params.getEditorDynamicLibraryName();

		if (params.buildType == BuildType::EditorHotReloading)
		{
			fs::copy_file(
				params.tempPath + editor_dll_name, 
				params.exportPath + editor_dll_name, 
				fs::copy_options::overwrite_existing
			);
		}
		else
		{
			fs::copy_file(
				params.tempPath + dll_name, 
				params.exportPath + dll_name, 
				fs::copy_options::overwrite_existing
			);
			fs::copy_file(
				params.tempPath + params.libraryName + ".exe", 
				params.exportPath + params.libraryName + ".exe", 
				fs::copy_options::overwrite_existing
			);
		}
	}
	catch (const std::exception&)
	{
		return CompileResult::ERROR_FINAL_GAME_FILES_COPY;
	}

	return CompileResult::SUCCESS;
}

CompileResult Compiler::CompileWSL(const CompilerParams& params)
{
	std::string convertedEnginePath = WindowsPathToWSL(EngineSettings::engineProjectPath);
	// Clear compilation folder
	int clearFolderResult = system("wsl sh -c 'rm -rf ~/XenityTestProject'");

	// Create folders
	int createProjectFolderResult = system("wsl sh -c 'mkdir ~/XenityTestProject'");
	int createBuildFolderResult = system("wsl sh -c 'mkdir ~/XenityTestProject/build'");

	// Copy files
	std::string copyEngineSourceCommand = "wsl sh -c 'cp -R /mnt/" + convertedEnginePath + "Source ~/XenityTestProject'";
	int copyCodeResult = system(copyEngineSourceCommand.c_str()); // Engine's source code + (game's code but to change later)
	std::string copyEngineLibrariesCommand = "wsl sh -c 'cp -R /mnt/" + convertedEnginePath + "include ~/XenityTestProject'";
	int copyLibrariesResult = system(copyEngineLibrariesCommand.c_str()); // Engine's libraries
	std::string copyCmakeCommand = "wsl sh -c 'cp -R /mnt/" + convertedEnginePath + "CMakeLists.txt ~/XenityTestProject'";
	int copyCmakelistsResult = system(copyCmakeCommand.c_str()); // Cmakelists file

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
	int compileResult = system(compileCommand.c_str());
	if (compileResult != 0)
	{
		return CompileResult::ERROR_WSL_COMPILATION;
	}

	std::string compileFolderPath = params.exportPath;
	compileFolderPath = compileFolderPath.erase(1, 1);
	size_t pathSize = compileFolderPath.size();
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

	int copyGameResult = system(copyGameCommand.c_str());
	if (copyGameResult != 0)
	{
		return CompileResult::ERROR_FINAL_GAME_FILES_COPY;
	}

	// Copy game assets
	try
	{
		fs::copy(
			ProjectManager::GetAssetFolderPath(), 
			params.exportPath + "assets\\", 
			fs::copy_options::overwrite_existing | fs::copy_options::recursive
		);
		fs::copy(
			ProjectManager::GetEngineAssetFolderPath(), 
			params.exportPath + "engine_assets\\", 
			fs::copy_options::overwrite_existing | fs::copy_options::recursive
		);
		fs::copy_file(
			ProjectManager::GetProjectFolderPath() + PROJECT_SETTINGS_FILE_NAME, 
			params.exportPath + PROJECT_SETTINGS_FILE_NAME, 
			fs::copy_options::overwrite_existing
		);
	}
	catch (const std::exception&)
	{
		return CompileResult::ERROR_FINAL_GAME_FILES_COPY;
	}

	return CompileResult::SUCCESS;
}

std::string Compiler::GetStartCompilerCommand()
{
	std::string path = EngineSettings::compilerPath;

	std::string command;
	if (fs::path(path).is_absolute())
	{
		command += path.substr(0,2) + " && "; // Go to the compiler folder
	}
	command += "cd \"" + EngineSettings::compilerPath + "\""; // Go to the compiler folder
	command += " && vcvarsamd64_x86.bat"; // Start the compiler
	//command += " >nul";	// Mute output
	return command;
}

std::string Compiler::GetAddNextCommand()
{
	std::string command = " && ";
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
	command += "cl /std:c++20 /MP /EHsc /DIMPORT"; // Start compilation
	if (params.buildType == BuildType::EditorHotReloading)
	{
		command += " /DEDITOR";
	}
	
	// Add include directories
	command += " -I \"" + EngineSettings::engineProjectPath + "include\"";
	command += " -I \"" + EngineSettings::engineProjectPath + "Source\"";

	std::string folder = params.tempPath + "source\\";
	command += " /LD \"" + folder + "*.cpp\"";
	if (params.buildType != BuildType::EditorHotReloading)
	{
		command += " engine_game.lib";
	}
	else
	{
		command += " engine_editor.lib";
	}
	command += " /link";
	command += " /implib:" + params.libraryName + ".lib";
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
	command += " main.cpp engine_game.lib";
	//command += " >nul"; // Mute output
	return command;
}

void Compiler::StartGame(Platform platform, const std::string& exportPath)
{
	if (platform == Platform::P_Windows)
	{
		std::string fileName = ProjectManager::GetGameName();
		std::string command = "cd \"" + exportPath + "\"" + " && " + "\"" + fileName + ".exe\"";

		system(command.c_str());
	}
	else if (platform == Platform::P_PSP)
	{
		std::string command = "(\"C:\\Program Files\\PPSSPP\\PPSSPPWindows.exe\" \"" + exportPath + "EBOOT.PBP\")";
		system(command.c_str());
	}
}