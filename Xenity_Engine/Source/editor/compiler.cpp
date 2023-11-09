#include "compiler.h"
#include "../engine/asset_management/project_manager.h"
#include "../engine/debug/debug.h"
#include <thread>
#include "../engine/dynamic_lib/dynamic_lib.h"
#include "../engine/engine.h"
#include "../engine/class_registry/class_registry.h"
#include "../engine/game_interface.h"
#include "../xenity_editor.h"
#include "ui/menus/compiling_menu.h"
#include <filesystem>
#include "../engine/scene_management/scene_manager.h"

#define ENGINE_PATH "C:\\Users\\elect\\Documents\\GitHub\\Xenity-Engine\\Xenity_Engine\\"
//#define ENGINE_PATH "D:\\Gregory_Machefer\\Xenity-Engine\\Xenity_Engine\\"
#define COMPILER_PATH "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Auxiliary\\Build"
//#define COMPILER_PATH "C:\\VisualStudio\\VC\\Auxiliary\\Build"

std::string tempCompileFolderPath = "";

CompileResult Compiler::CompileInWSL(Platform platform, const std::string& exportPath)
{
	// Clear compilation folder
	int clearFolderResult = system("wsl sh -c 'rm -rf ~/XenityTestProject'");

	// Create folders
	int createProjectFolderResult = system("wsl sh -c 'mkdir ~/XenityTestProject'");
	int createBuildFolderResult = system("wsl sh -c 'mkdir ~/XenityTestProject/build'");

	// Copy files
	int copyCodeResult = system("wsl sh -c 'cp -R /mnt/c/Users/elect/Documents/GitHub/Xenity-Engine/Xenity_Engine/Source ~/XenityTestProject'"); // Engine's source code + (game's code but to change later)
	int copyLibrariesResult = system("wsl sh -c 'cp -R /mnt/c/Users/elect/Documents/GitHub/Xenity-Engine/Xenity_Engine/include ~/XenityTestProject'"); // Engine's libraries
	int copyCmakelistsResult = system("wsl sh -c 'cp -R /mnt/c/Users/elect/Documents/GitHub/Xenity-Engine/Xenity_Engine/CMakeLists.txt ~/XenityTestProject'"); // Cmakelists file

	if (copyCodeResult == 0)
	{
		return ERROR_WSL_ENGINE_CODE_COPY;
	}
	else if (copyLibrariesResult == 0)
	{
		return ERROR_WSL_ENGINE_LIBS_INCLUDE_COPY;
	}
	else if (copyCmakelistsResult == 0)
	{
		return ERROR_WSL_CMAKELISTS_COPY;
	}

	// get the thread number of the cpu
	unsigned int threadNumber = std::thread::hardware_concurrency();
	if (threadNumber == 0) // This function may returns 0, use 1 instead
	{
		threadNumber = 1;
	}

	std::string compileCommand = "wsl bash -c -i \"cd ~/XenityTestProject/build";
	if (platform == P_PSP)
		compileCommand += " && psp-cmake -DMODE=psp ..";
	else if (platform == P_PsVita)
		compileCommand += " && cmake -DMODE=psvita ..";

	compileCommand += " && cmake --build . -j" + std::to_string(threadNumber) + "\""; // Use thread number to increase compilation speed

	// Start compilation
	int compileResult = system(compileCommand.c_str());
	if (compileResult != 0)
	{
		return ERROR_WSL_COMPILATION;
	}

	std::string compileFolderPath = exportPath;
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
	if (platform == P_PSP)
		copyGameCommand = "wsl sh -c 'cp ~/\"XenityTestProject/build/EBOOT.PBP\" \"" + compileFolderPath + "/EBOOT.PBP\"'";
	else if (platform == P_PsVita)
		copyGameCommand = "wsl sh -c 'cp ~/\"XenityTestProject/build/hello.vpk\" \"" + compileFolderPath + "/hello.vpk\"'";

	int copyGameResult = system(copyGameCommand.c_str());
	if (copyGameResult == 0)
	{
		return ERROR_FINAL_GAME_FILES_COPY;
	}

	// Copy game assets
	try
	{
		std::filesystem::copy(ProjectManager::GetAssetFolderPath(), exportPath + "assets\\", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
		std::filesystem::copy(ProjectManager::GetEngineAssetFolderPath(), exportPath + "engine_assets\\", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
		std::filesystem::copy_file(ProjectManager::GetProjectFolderPath() + PROJECT_SETTINGS_FILE_NAME, exportPath + PROJECT_SETTINGS_FILE_NAME, std::filesystem::copy_options::overwrite_existing);
	}
	catch (const std::exception&)
	{
		return ERROR_FINAL_GAME_FILES_COPY;
	}

	return SUCCESS;
}

std::string Compiler::GetStartCompilerCommand()
{
	std::string command;
	command += "cd " + std::string(COMPILER_PATH); // Go to the compiler folder
	command += " && vcvarsamd64_x86.bat"; // Start the compiler
	//command += " >nul";	// Mute output
	return command;
}

std::string Compiler::GetAddNextCommand()
{
	std::string command = " && ";
	return command;
}

std::string Compiler::GetNavToEngineFolderCommand()
{
	std::string command = "cd " + tempCompileFolderPath;
	return command;
}

std::string Compiler::GetCompileGameLibCommand(BuildType buildType)
{
	std::string command = "";
	command += "cl /std:c++20 /MP /EHsc /DIMPORT"; // Start compilation
	if (buildType == EditorHotReloading)
	{
		command += " /DEDITOR";
	}
	std::string folder = tempCompileFolderPath + "source\\";
	command += " -I \"" + std::string(ENGINE_PATH) + "include\" /LD \"" + folder + "*.cpp\"";
	if (buildType != EditorHotReloading)
	{
		command += " engine_game.lib";
	}
	else
	{
		command += " engine_editor.lib";
	}
	command += " /link";
	if (buildType != EditorHotReloading)
	{
		command += " /implib:game.lib /out:game.dll";
	}
	else
	{
		command += " /implib:game_editor.lib /out:game_editor.dll";
	}
	//command += " >nul"; // Mute output
	return command;
}

std::string Compiler::GetCompileGameExeCommand()
{
	std::string command;
	std::string fileName = ProjectManager::GetGameName();
	command = "cl /Fe\"" + fileName + ".exe\" /std:c++20 /MP /EHsc -I \"" + std::string(ENGINE_PATH) + "include\" main.cpp engine_game.lib"; //Buid game exe

	//command += " >nul"; // Mute output
	return command;
}

void Compiler::CompileGameThreaded(Platform platform, BuildType buildType, const std::string& exportPath)
{
	Editor::compilingMenu->OpenPopup();
	std::thread t = std::thread(CompileGame, platform, buildType, exportPath);
	t.detach();
}

void Compiler::OnCompileEnd(CompileResult result)
{
	switch (result)
	{
	case SUCCESS:
		Debug::Print("Game compiled successfully!");
		break;
	case ERROR_UNKNOWN:
		Debug::PrintError("[Compiler::OnCompileEnd] Unable to compile (unkown error)");
		break;
	case ERROR_ENGINE_GAME_LIB_MISSING:
		Debug::PrintError("[Compiler::OnCompileEnd] Missing engine_game.lib");
		break;
	case ERROR_ENGINE_EDITOR_LIB_MISSING:
		Debug::PrintError("[Compiler::OnCompileEnd] Missing engine_editor.lib");
		break;
	case ERROR_LIB_DLLS_MISSING:
		Debug::PrintError("[Compiler::OnCompileEnd] Missing one of Dlls");
		break;
	case ERROR_ENGINE_HEADERS_COPY:
		Debug::PrintError("[Compiler::OnCompileEnd] Error when copying engine headers");
		break;
	case ERROR_GAME_CODE_COPY:
		Debug::PrintError("[Compiler::OnCompileEnd] Error when copying game's code");
		break;
	case ERROR_FINAL_GAME_FILES_COPY:
		Debug::PrintError("[Compiler::OnCompileEnd] Error when copying game's files");
		break;

		// Specific to WSL
	case ERROR_WSL_COMPILATION:
		Debug::PrintError("[Compiler::OnCompileEnd] Unable to compile on WSL");
		break;
	case ERROR_WSL_ENGINE_CODE_COPY:
		Debug::PrintError("[Compiler::OnCompileEnd] Error when copying engine's code");
		break;
	case ERROR_WSL_ENGINE_LIBS_INCLUDE_COPY:
		Debug::PrintError("[Compiler::OnCompileEnd] Error when copying engine's libraries files");
		break;
	case ERROR_WSL_CMAKELISTS_COPY:
		Debug::PrintError("[Compiler::OnCompileEnd] Error when copying CMakeLists.txt file");
		break;

	default:
		Debug::PrintError("[Compiler::OnCompileEnd] Unable to compile (unkown error)");
		break;
	}
	Editor::compilingMenu->ClosePopup();
}

void Compiler::CompileGame(Platform platform, BuildType buildType, const std::string& exportPath)
{
	// Set the folder path to use for the compilation
	if (buildType == EditorHotReloading)
	{
		tempCompileFolderPath = ProjectManager::GetProjectFolderPath() + "hot_reloading_data\\";
	}
	else
	{
		tempCompileFolderPath = ProjectManager::GetProjectFolderPath() + "temp_build\\";
	}

	// Delete the directory and create a new one
	try
	{
		std::filesystem::remove_all(tempCompileFolderPath);
		std::filesystem::create_directory(tempCompileFolderPath);
	}
	catch (const std::exception&)
	{
		Debug::PrintWarning("[Compiler::CompileGame] Unable to clear the compilation folder");
	}

	if (platform == P_Windows)
	{
		if (buildType == EditorHotReloading) // In hot reloading mode:
		{
			std::string engineLibPath = ENGINE_PATH + std::string("engine_editor.lib");

			try
			{
				// Copy engine editor lib to the temp build folder
				std::filesystem::copy_file(engineLibPath, tempCompileFolderPath + "engine_editor.lib", std::filesystem::copy_options::overwrite_existing);
			}
			catch (const std::exception&)
			{
				OnCompileEnd(ERROR_ENGINE_EDITOR_LIB_MISSING);
				return;
			}

			// Copy engine headers
			try
			{
				std::filesystem::copy(ENGINE_PATH + std::string("Source\\engine\\"), tempCompileFolderPath + "engine\\", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
				std::filesystem::copy_file(ENGINE_PATH + std::string("Source\\xenity.h"), tempCompileFolderPath + "xenity.h", std::filesystem::copy_options::overwrite_existing);
				std::filesystem::copy_file(ENGINE_PATH + std::string("Source\\xenity_editor.h"), tempCompileFolderPath + "xenity_editor.h", std::filesystem::copy_options::overwrite_existing);
			}
			catch (const std::exception&)
			{
				OnCompileEnd(ERROR_ENGINE_HEADERS_COPY);
				return;
			}
		}
		else // In game build mode:
		{
			std::string engineLibPath = ENGINE_PATH + std::string("engine_game.lib");
			std::string engineDllPath = ENGINE_PATH + std::string("engine_game.dll");
			std::string sdlDllPath = ENGINE_PATH + std::string("SDL2.dll");
			std::string glfwDllPath = ENGINE_PATH + std::string("glfw3.dll");

			// Copy engine game lib to the temp build folder
			try
			{
				std::filesystem::copy_file(engineLibPath, tempCompileFolderPath + "engine_game.lib", std::filesystem::copy_options::overwrite_existing);
			}
			catch (const std::exception&)
			{
				OnCompileEnd(ERROR_ENGINE_GAME_LIB_MISSING);
				return;
			}

			// Copy all DLLs to the export folder
			try
			{
				std::filesystem::copy_file(engineDllPath, exportPath + "engine_game.dll", std::filesystem::copy_options::overwrite_existing);
				std::filesystem::copy_file(sdlDllPath, exportPath + "SDL2.dll", std::filesystem::copy_options::overwrite_existing);
				std::filesystem::copy_file(glfwDllPath, exportPath + "glfw3.dll", std::filesystem::copy_options::overwrite_existing);
			}
			catch (const std::exception&)
			{
				OnCompileEnd(ERROR_LIB_DLLS_MISSING);
				return;
			}

			// Copy engine headers to the temp build folder
			try
			{
				std::filesystem::copy(ENGINE_PATH + std::string("Source\\engine\\"), tempCompileFolderPath + "engine\\", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
				std::filesystem::copy_file(ENGINE_PATH + std::string("Source\\xenity.h"), tempCompileFolderPath + "xenity.h", std::filesystem::copy_options::overwrite_existing);

				std::filesystem::copy_file(ENGINE_PATH + std::string("Source\\main.cpp"), tempCompileFolderPath + "main.cpp", std::filesystem::copy_options::overwrite_existing);
				std::filesystem::copy_file(ENGINE_PATH + std::string("Source\\main.h"), tempCompileFolderPath + "main.h", std::filesystem::copy_options::overwrite_existing);
			}
			catch (const std::exception&)
			{
				OnCompileEnd(ERROR_ENGINE_HEADERS_COPY);
				return;
			}
		}

		// Copy game code
		try
		{
			std::filesystem::create_directory(tempCompileFolderPath + "source\\");

			for (const auto& file : std::filesystem::directory_iterator(ProjectManager::GetAssetFolderPath()))
			{
				if (file.is_regular_file())
				{
					std::string ext = file.path().extension().string();
					if (ext == ".h" || ext == ".cpp")
					{
						std::string path = file.path().string();
						std::string fileName = file.path().filename().string();
						std::filesystem::copy_file(path, tempCompileFolderPath + "source\\" + fileName, std::filesystem::copy_options::overwrite_existing);
					}
				}
			}
		}
		catch (const std::exception&)
		{
			OnCompileEnd(ERROR_GAME_CODE_COPY);
			return;
		}

		// Setup windows compileur command
		std::string command;
		command = GetStartCompilerCommand();
		//command += " && d:";
		command += GetAddNextCommand();
		command += GetNavToEngineFolderCommand();
		command += GetAddNextCommand();
		command += GetCompileGameLibCommand(buildType);
		if (buildType != EditorHotReloading)
		{
			command += GetAddNextCommand();
			command += GetCompileGameExeCommand();
		}

		int buildResult = system(command.c_str());
		if (buildResult == 0)
		{
			try
			{
				if (buildType == EditorHotReloading)
				{
					std::filesystem::copy_file(tempCompileFolderPath + "game_editor.dll", ProjectManager::GetProjectFolderPath() + "\\game_editor.dll", std::filesystem::copy_options::overwrite_existing);
				}
				else
				{
					std::filesystem::copy_file(tempCompileFolderPath + ProjectManager::GetGameName() + ".exe", exportPath + ProjectManager::GetGameName() + ".exe", std::filesystem::copy_options::overwrite_existing);
					std::filesystem::copy_file(tempCompileFolderPath + "game.dll", exportPath + "game.dll", std::filesystem::copy_options::overwrite_existing);
					std::filesystem::copy(ProjectManager::GetAssetFolderPath(), exportPath + "assets\\", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
					std::filesystem::copy(ProjectManager::GetEngineAssetFolderPath(), exportPath + "engine_assets\\", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
					std::filesystem::copy_file(ProjectManager::GetProjectFolderPath() + PROJECT_SETTINGS_FILE_NAME, exportPath + PROJECT_SETTINGS_FILE_NAME, std::filesystem::copy_options::overwrite_existing);
				}
			}
			catch (const std::exception&)
			{
				OnCompileEnd(ERROR_FINAL_GAME_FILES_COPY);
				return;
			}

			// Delete temp compiler folder content
			try
			{
				std::filesystem::remove_all(tempCompileFolderPath);
			}
			catch (const std::exception&)
			{
			}

			if (buildType == BuildAndRunGame)
			{
				auto t = std::thread(StartGame, platform, exportPath);
				t.detach();
			}
		}
		else
		{
			OnCompileEnd(ERROR_UNKNOWN);
			return;
		}
	}
	else
	{
		CompileResult result = CompileInWSL(platform, exportPath);
		if (result == SUCCESS && buildType == BuildAndRunGame)
		{
			auto t = std::thread(StartGame, platform, exportPath);
			t.detach();
		}
		else
		{
			OnCompileEnd(result);
			return;
		}
	}

	OnCompileEnd(SUCCESS);
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

void Compiler::HotReloadGame()
{
#if defined(_WIN32) || defined(_WIN64)
	Engine::game.reset();
	Engine::game = nullptr;

	SceneManager::SaveScene(SaveSceneForHotReloading);
	SceneManager::ClearScene();

	ClassRegistry::Reset();
	Engine::RegisterEngineComponents();
	DynamicLibrary::UnloadGameLibrary();
	Compiler::CompileGame(Platform::P_Windows, EditorHotReloading, "");
	DynamicLibrary::LoadGameLibrary(ProjectManager::GetProjectFolderPath() + "game_editor");
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