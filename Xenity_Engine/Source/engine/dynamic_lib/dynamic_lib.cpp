#if defined(_WIN32) || defined(_WIN64)
#include "dynamic_lib.h"
#include "../game_interface.h"
#include <windows.h>
#include "../debug/debug.h"

typedef GameInterface* (__cdecl* CreateGameFunction)();
HINSTANCE library;

void DynamicLibrary::LoadGameLibrary(const std::string& libraryName)
{
	std::string fileName = libraryName + ".dll";

#if defined(VISUAL_STUDIO)
	library = LoadLibrary((LPCWSTR)fileName.c_str()); // Visual Studio
#else
	library = LoadLibrary(fileName.c_str()); // MSVC Compiler
#endif

	if (library == NULL)
		Debug::PrintError("[DynamicLibrary::LoadGameLibrary] Library not found: " + fileName);
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
			Debug::PrintError("[DynamicLibrary::UnloadGameLibrary] Library cannot be freed");
		}
	}
}

GameInterface* DynamicLibrary::CreateGame()
{
	GameInterface* gameInterface = nullptr;
	if (library != NULL)
	{
		// Find the "CreateGame" function
		CreateGameFunction ProcAdd = (CreateGameFunction)GetProcAddress(library, "CreateGame");
		if (ProcAdd)
		{
			gameInterface = (ProcAdd)();
		}
		else
		{
			Debug::PrintError("[DynamicLibrary::CreateGame] Cannot create game");
		}
	}
	else
	{
		Debug::PrintError("[DynamicLibrary::CreateGame] Cannot create game");
	}

	return gameInterface;
}
#endif