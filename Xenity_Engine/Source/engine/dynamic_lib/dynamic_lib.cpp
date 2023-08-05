#if defined(_WIN32) || defined(_WIN64)
#include "dynamic_lib.h"
#include "../game_interface.h"
#include <iostream>
#include <windows.h>
#include "../debug/debug.h"
#include <thread>
#include "../asset_management/project_manager.h"
#include <filesystem>

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