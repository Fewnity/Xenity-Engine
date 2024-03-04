#if defined(_WIN32) || defined(_WIN64)
#include "dynamic_lib.h"
#include <engine/game_interface.h>
#include <windows.h>
#include <engine/debug/debug.h>
#include <engine/asset_management/project_manager.h>

typedef GameInterface* (__cdecl* CreateGameFunction)();
HINSTANCE library;

void DynamicLibrary::LoadGameLibrary(const std::string& libraryName)
{
	const std::string fileName = libraryName + ".dll";

	//Disable error popup
	SetErrorMode(SEM_FAILCRITICALERRORS);
#if defined(VISUAL_STUDIO)
	library = LoadLibraryA(fileName.c_str()); // Visual Studio
#else
	library = LoadLibrary(fileName.c_str()); // MSVC Compiler
#endif
	SetErrorMode(0);

	const bool result = library != NULL;
	if (!result)
	{
		// https://learn.microsoft.com/en-us/windows/win32/debug/system-error-codes--0-499-
		const int errorCode = GetLastError();
		if(errorCode == 127)
			Debug::PrintError("[DynamicLibrary::LoadGameLibrary] Failed to load library (wrong version): " + fileName, true);
		else
			Debug::PrintError("[DynamicLibrary::LoadGameLibrary] Library not found: " + fileName, true);
	}

	ProjectManager::projectSettings.isCompiled = result;
}

void DynamicLibrary::UnloadGameLibrary()
{
	if (library != NULL)
	{
		if (FreeLibrary(library))
		{
			Debug::Print("Library freed", true);
		}
		else
		{
			Debug::PrintError("[DynamicLibrary::UnloadGameLibrary] Library cannot be freed", true);
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
			Debug::PrintError("[DynamicLibrary::CreateGame] Cannot create game", true);
		}
	}
	else
	{
		Debug::PrintError("[DynamicLibrary::CreateGame] Cannot create game", true);
	}

	return std::unique_ptr<GameInterface> (gameInterface);
}
#endif