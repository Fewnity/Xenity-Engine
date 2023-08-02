#if defined(_WIN32) || defined(_WIN64)
#include "dynamic_lib.h"
#include "../game_interface.h"
#include <iostream>
#include <windows.h>
#include "../debug/debug.h"
#include <thread>

typedef void(__cdecl* MYPROC2)();
typedef GameInterface* (__cdecl* CreateGameFunction)();
HINSTANCE library;

void DynamicLibrary::LoadGameLibrary(std::string libraryName)
{
	libraryName += ".dll";

	library = LoadLibrary((LPCWSTR)libraryName.c_str()); //Visual Studio
	//library = LoadLibrary(libraryName.c_str()); // MSVC Compiler
	if (library != NULL)
		Debug::Print("Library found");
	else
		Debug::PrintError("Library not found");
}

void DynamicLibrary::UnloadGameLibrary()
{
	if (library != NULL)
	{
		if (FreeLibrary(library))
		{
			Debug::Print("Library freed");
		}
		else
		{
			Debug::PrintError("Library cannot be freed");
		}
	}
}

void DynamicLibrary::StartGame()
{
	system("C:\\Users\\elect\\Documents\\GitHub\\Xenity-Engine\\Xenity_Engine\\Game.exe");
}

std::string DynamicLibrary::GetStartCompilerCommand()
{
	std::string command;
	command = "cd C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Auxiliary\\Build"; // Go to the compiler folder
	command += " && vcvarsamd64_x86.bat"; // Start the compiler
	//command += " >nul";	// Mute output
	return command;
}

std::string DynamicLibrary::GetAddNextCommand()
{
	std::string command = " && ";
	return command;
}

std::string DynamicLibrary::GetNavToEngineFolderCommand() 
{
	std::string command = "cd C:\\Users\\elect\\Documents\\GitHub\\Xenity-Engine\\Xenity_Engine";
	return command;
}

void DynamicLibrary::CompileGame(BuildType buildType)
{
	std::string command;
	command = GetStartCompilerCommand();
	command += GetAddNextCommand();
	command += GetNavToEngineFolderCommand();

	//command += " && cd C:\\Users\\elect\\Documents\\GitHub\\Xenity-Engine\\Xenity_Engine";																					   // Go in the game project folder

	command += " && cl /std:c++20 /EHsc /DIMPORT"; // Start compilation
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
		command += " /implib:game.lib";
	}
	else
	{
		command += " /implib:game_editor.lib";
	}
	//command += " >nul";
	if (buildType != EditorHotReloading)
	{
		command += " /out:game.dll";
		//command += " >nul";
		command += " && cl /FeGame.exe /std:c++20 /EHsc -I \"C:\\Users\\elect\\Documents\\GitHub\\Xenity-Engine\\Xenity_Engine\\include\" Source/main.cpp engine_game.lib"; //Buid game exe
		//command += " >nul";
	}
	else
	{
		command += " /out:game_editor.dll";
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

GameInterface* DynamicLibrary::CreateGame()
{
	GameInterface* gameInterface = nullptr;
	if (library != NULL)
	{
		CreateGameFunction ProcAdd = (CreateGameFunction)GetProcAddress(library, "CreateGame");
		if (ProcAdd)
		{
			gameInterface = (ProcAdd)();
		}
		else
		{
			std::cout << "[ERROR] Cannot create game" << std::endl;
		}
	}
	else
	{
		std::cout << "[ERROR] Cannot create game" << std::endl;
	}

	return gameInterface;
}
#endif