#if defined(_WIN32) || defined(_WIN64)
#include "dynamic_lib.h"
#include <engine/game_interface.h>
#include <windows.h>
#include <engine/debug/debug.h>

typedef GameInterface* (__cdecl* CreateGameFunction)();
HINSTANCE library;

void DynamicLibrary::LoadGameLibrary(const std::string& libraryName)
{
	std::string fileName = libraryName + ".dll";

	//Disable error popup
	SetErrorMode(0x0001);
#if defined(VISUAL_STUDIO)
	library = LoadLibraryA(fileName.c_str()); // Visual Studio
#else
	library = LoadLibrary(fileName.c_str()); // MSVC Compiler
#endif
	SetErrorMode(0);

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

std::unique_ptr<GameInterface> DynamicLibrary::CreateGame()
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

	return std::unique_ptr<GameInterface> (gameInterface);
}
#endif