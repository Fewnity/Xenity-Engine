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

#define ENGINE_PATH "C:\\Users\\elect\\Documents\\GitHub\\Xenity-Engine\\Xenity_Engine\\"

std::string tempCompileFolderPath = "";

void Compiler::CompileInWSL(Platform platform, std::string exportPath)
{
	int r1 = system("wsl sh -c 'rm -rf ~/XenityTestProject'");
	int r2 = system("wsl sh -c 'mkdir ~/XenityTestProject'");
	int r3 = system("wsl sh -c 'mkdir ~/XenityTestProject/build'");
	int r4 = system("wsl sh -c 'cp -R /mnt/c/Users/elect/Documents/GitHub/Xenity-Engine/Xenity_Engine/Source ~/XenityTestProject'");
	int r5 = system("wsl sh -c 'cp -R /mnt/c/Users/elect/Documents/GitHub/Xenity-Engine/Xenity_Engine/include ~/XenityTestProject'");
	int r6 = system("wsl sh -c 'cp -R /mnt/c/Users/elect/Documents/GitHub/Xenity-Engine/Xenity_Engine/CMakeLists.txt ~/XenityTestProject'");
	int r7 = 1;
	if (platform == P_PSP)
		r7 = system("wsl bash -c -i \"cd ~/XenityTestProject/build && psp-cmake -DMODE=psp .. && cmake --build . -j12\"");
	else if (platform == P_PsVita)
		r7 = system("wsl bash -c -i \"cd ~/XenityTestProject/build && cmake -DMODE=psvita .. && cmake --build . -j12\"");

	if (r7 != 0)
	{
		Debug::PrintError("Compilation failed");
	}

	/*Debug::Print(std::to_string(r1));
	Debug::Print(std::to_string(r2));
	Debug::Print(std::to_string(r3));
	Debug::Print(std::to_string(r4));
	Debug::Print(std::to_string(r5));
	Debug::Print(std::to_string(r6));
	Debug::Print(std::to_string(r7));*/
	std::string compileFolderPath = exportPath;
	compileFolderPath = compileFolderPath.erase(1, 1);
	int pathSize = compileFolderPath.size();
	for (int i = 0; i < pathSize; i++)
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

	//Debug::Print(copyGameCommand);
	int r8 = system(copyGameCommand.c_str());

	std::filesystem::copy(ProjectManager::GetAssetFolderPath(), exportPath + "assets\\", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
	std::filesystem::copy(ProjectManager::GetEngineAssetFolderPath(), exportPath + "engine_assets\\", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
	std::filesystem::copy_file(ProjectManager::GetProjectFolderPath() + PROJECT_SETTINGS_FILE_NAME, exportPath + PROJECT_SETTINGS_FILE_NAME, std::filesystem::copy_options::overwrite_existing);

	//Debug::Print(std::to_string(r8));
}

std::string Compiler::GetStartCompilerCommand()
{
	std::string command;
	command = "cd C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Auxiliary\\Build"; // Go to the compiler folder
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
	command += " -I \"C:\\Users\\elect\\Documents\\GitHub\\Xenity-Engine\\Xenity_Engine\\include\" /LD \"" + folder + "\*.cpp\"";
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
	command = "cl /Fe\"" + fileName + ".exe\" /std:c++20 /MP /EHsc -I \"C:\\Users\\elect\\Documents\\GitHub\\Xenity-Engine\\Xenity_Engine\\include\" main.cpp engine_game.lib"; //Buid game exe
	//command += " >nul"; // Mute output
	return command;
}

void Compiler::CompileGameThreaded(Platform platform, BuildType buildType, std::string exportPath)
{
	Editor::compilingMenu->OpenPopup();
	auto t = std::thread(CompileGame, platform, buildType, exportPath);
	t.detach();
}

void Compiler::CompileGame(Platform platform, BuildType buildType, std::string exportPath)
{
	tempCompileFolderPath = ProjectManager::GetProjectFolderPath() + "temp_build\\";
	if (buildType == EditorHotReloading)
	{
		tempCompileFolderPath = ProjectManager::GetProjectFolderPath() + "hot_reloading_data\\";
	}

	std::filesystem::remove_all(tempCompileFolderPath);
	std::filesystem::create_directory(tempCompileFolderPath);
	// Remove all files in the export folder
	//std::filesystem::remove_all(exportPath);
	//std::filesystem::create_directory(exportPath);

	if (platform == P_Windows)
	{
		if (buildType == EditorHotReloading)
		{
			std::string engineLibPath = ENGINE_PATH + std::string("engine_editor.lib");
			std::filesystem::copy_file(engineLibPath, tempCompileFolderPath + "engine_editor.lib", std::filesystem::copy_options::overwrite_existing);

			std::filesystem::copy(ENGINE_PATH + std::string("Source\\engine\\"), tempCompileFolderPath + "engine\\", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
			std::filesystem::copy_file(ENGINE_PATH + std::string("Source\\xenity.h"), tempCompileFolderPath + "xenity.h", std::filesystem::copy_options::overwrite_existing);
		}
		else
		{
			// Copy engine lib to the temp build folder
			std::string engineLibPath = ENGINE_PATH + std::string("engine_game.lib");
			std::filesystem::copy_file(engineLibPath, tempCompileFolderPath + "engine_game.lib", std::filesystem::copy_options::overwrite_existing);

			// Copy all DLLs to the export folder
			std::string engineDllPath = ENGINE_PATH + std::string("engine_game.dll");
			std::filesystem::copy_file(engineDllPath, exportPath + "engine_game.dll", std::filesystem::copy_options::overwrite_existing);
			std::string sdlDllPath = ENGINE_PATH + std::string("SDL2.dll");
			std::filesystem::copy_file(sdlDllPath, exportPath + "SDL2.dll", std::filesystem::copy_options::overwrite_existing);
			std::string glfwDllPath = ENGINE_PATH + std::string("glfw3.dll");
			std::filesystem::copy_file(glfwDllPath, exportPath + "glfw3.dll", std::filesystem::copy_options::overwrite_existing);

			// Copy engine headers to the temp build folder
			std::filesystem::copy(ENGINE_PATH + std::string("Source\\engine\\"), tempCompileFolderPath + "engine\\", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
			std::filesystem::copy_file(ENGINE_PATH + std::string("Source\\xenity.h"), tempCompileFolderPath + "xenity.h", std::filesystem::copy_options::overwrite_existing);

			std::filesystem::copy_file(ENGINE_PATH + std::string("Source\\main.cpp"), tempCompileFolderPath + "main.cpp", std::filesystem::copy_options::overwrite_existing);
			std::filesystem::copy_file(ENGINE_PATH + std::string("Source\\main.h"), tempCompileFolderPath + "main.h", std::filesystem::copy_options::overwrite_existing);
		}
		std::filesystem::create_directory(tempCompileFolderPath + "source\\");

		for (const auto& file : std::filesystem::directory_iterator(ProjectManager::GetAssetFolderPath()))
		{
			if (file.is_regular_file())
			{
				std::string ext = file.path().extension().string();
				if (ext == ".h" || ext == ".hpp" || ext == ".c" || ext == ".cpp")
				{
					std::string path = file.path().string();
					std::string fileName = file.path().filename().string();
					std::filesystem::copy_file(path, tempCompileFolderPath + "source\\" + fileName, std::filesystem::copy_options::overwrite_existing);
				}
			}
		}

		std::string command;
		command = GetStartCompilerCommand();
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
			Debug::Print("Game compiled successfully!");
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
			std::filesystem::remove_all(tempCompileFolderPath);
			if (buildType == BuildAndRunGame)
			{
				auto t = std::thread(StartGame, platform, exportPath);
				t.detach();
			}
		}
		else
		{
			Debug::PrintError("Unable to compile : " + command);
		}
	}
	else
	{
		CompileInWSL(platform, exportPath);
		if (buildType == BuildAndRunGame)
		{
			auto t = std::thread(StartGame, platform, exportPath);
			t.detach();
		}
	}

	Editor::compilingMenu->ClosePopup();
}

void Compiler::StartGame(Platform platform, std::string exportPath)
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
	delete Engine::game;
	Engine::game = nullptr;
	Engine::EmptyScene();

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
	}
	else
	{
		Debug::PrintError("Game compilation failed");
	}
#endif
}