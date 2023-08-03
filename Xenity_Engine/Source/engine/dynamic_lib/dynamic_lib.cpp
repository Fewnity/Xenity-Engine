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

#if defined(VISUAL_STUDIO)
	library = LoadLibrary((LPCWSTR)libraryName.c_str()); //Visual Studio
#else
	library = LoadLibrary(libraryName.c_str()); // MSVC Compiler
#endif
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

std::string DynamicLibrary::GetCompileGameLibCommand(BuildType buildType)
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

std::string DynamicLibrary::GetCompileGameExeCommand() 
{
	std::string command;
	command = "cl /FeGame.exe /std:c++20 /MP /EHsc -I \"C:\\Users\\elect\\Documents\\GitHub\\Xenity-Engine\\Xenity_Engine\\include\" Source/main.cpp engine_game.lib"; //Buid game exe
	//command += " >nul"; // Mute output
	return command;
}

void DynamicLibrary::CompileGame(BuildType buildType)
{
	std::string command;
	command = GetStartCompilerCommand();
	command += GetAddNextCommand();
	command += GetNavToEngineFolderCommand();
	command += GetAddNextCommand();
	command += GetCompileGameLibCommand(buildType);
	command += GetAddNextCommand();

	if (buildType != EditorHotReloading)
		command += GetCompileGameExeCommand();

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
			Debug::PrintError("Cannot create gam");
		}
	}
	else
	{
		Debug::PrintError("Cannot create game");
	}

	return gameInterface;
}
#endif