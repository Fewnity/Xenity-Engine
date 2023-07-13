#if defined(_WIN32) || defined(_WIN64)
#include "dynamic_lib.h"
#include "../game_interface.h"
#include <iostream>
#include <windows.h>
#include "../debug/debug.h"

typedef void(__cdecl *MYPROC2)();
typedef GameInterface *(__cdecl *CreateGameFunction)();
HINSTANCE library;

void DynamicLibrary::LoadGameLibrary(std::string libraryName)
{
	libraryName += ".dll";

	// library = LoadLibrary((LPCWSTR)libraryName.c_str()); //Visual Studio
	library = LoadLibrary(libraryName.c_str()); // MSVC Compiler
	if (library != NULL)
	{
		Debug::Print("Library found");
	}
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

void DynamicLibrary::CompileGame()
{
	// dllName += ".dll";
	std::string command;
	command = "cd C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Auxiliary\\Build";																		   // Go in the compiler folder
	command += " && vcvarsamd64_x86.bat";																																	   // Start the compiler
	command += " >nul";
	command += " && cd C:\\Users\\elect\\Documents\\GitHub\\Xenity-Engine\\Xenity_Engine";																					   // Go in the game project folder
	command += " && cl /DIMPORT -I \"C:\\Users\\elect\\Documents\\GitHub\\Xenity-Engine\\Xenity_Engine\\include\" /LD Source/game_test/\*.cpp engine.lib /link /out:game.dll"; // Start compilation
	command += " >nul";
	// Debug::Print(command);
	system(command.c_str());
}

GameInterface *DynamicLibrary::CreateGame()
{
	GameInterface *gameInterface = nullptr;
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