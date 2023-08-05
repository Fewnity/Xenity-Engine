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
		r7 = system("wsl bash -c -i \"cd ~/XenityTestProject/build && psp-cmake -DMODE=psp .. && cmake --build .\"");
	else if (platform == P_PsVita)
		r7 = system("wsl bash -c -i \"cd ~/XenityTestProject/build && cmake -DMODE=psvita .. && cmake --build .\"");

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

	exportPath = exportPath.erase(1, 1);
	int pathSize = exportPath.size();
	for (int i = 0; i < pathSize; i++)
	{
		if (exportPath[i] == '\\')
		{
			exportPath[i] = '/';
		}
	}
	exportPath[0] = tolower(exportPath[0]);
	exportPath = "/mnt/" + exportPath;
	std::string copyGameCommand;
	if (platform == P_PSP)
		copyGameCommand = "wsl sh -c 'cp ~/\"XenityTestProject/build/EBOOT.PBP\" \"" + exportPath + "/EBOOT.PBP\"'";
	else if (platform == P_PsVita)
		copyGameCommand = "wsl sh -c 'cp ~/\"XenityTestProject/build/hello.vpk\" \"" + exportPath + "/hello.vpk\"'";

	//Debug::Print(copyGameCommand);
	int r8 = system(copyGameCommand.c_str());
	std::string copyGameCommand2;
	copyGameCommand2 = "wsl sh -c 'cp -R /mnt/c/Users/elect/Documents/GitHub/Xenity-Engine/Xenity_Engine/project \"" + exportPath + "\"'";
	system(copyGameCommand2.c_str());
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
	std::string command = "cd C:\\Users\\elect\\Documents\\GitHub\\Xenity-Engine\\Xenity_Engine";
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
	command += " -I \"C:\\Users\\elect\\Documents\\GitHub\\Xenity-Engine\\Xenity_Engine\\include\" /LD Source/game_test/\*.cpp";
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
	command = "cl /Fe" + fileName + ".exe /std:c++20 /MP /EHsc -I \"C:\\Users\\elect\\Documents\\GitHub\\Xenity-Engine\\Xenity_Engine\\include\" Source/main.cpp engine_game.lib"; //Buid game exe
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
	if (platform == P_Windows)
	{
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
			if (buildType == BuildAndRunGame)
			{
				auto t = std::thread(StartGame);
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
	}
	Editor::compilingMenu->ClosePopup();
}

void Compiler::StartGame()
{
	std::string fileName = ProjectManager::GetGameName();
	std::string command = "C:\\Users\\elect\\Documents\\GitHub\\Xenity-Engine\\Xenity_Engine\\" + fileName + ".exe";
	system(command.c_str());
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
	DynamicLibrary::LoadGameLibrary("game_editor");
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