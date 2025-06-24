// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
#include "dynamic_lib.h"
#include <engine/game_interface.h>
#include <engine/debug/debug.h>
#include <engine/asset_management/project_manager.h>
#include <engine/assertions/assertions.h>

#if defined(_WIN32) || defined(_WIN64) 
#include <windows.h>
typedef GameInterface* (__cdecl* CreateGameFunction)();
HINSTANCE library;
#endif

void DynamicLibrary::LoadGameLibrary(const std::string& libraryName)
{
	XASSERT(!libraryName.empty(), "[DynamicLibrary::LoadGameLibrary] libraryName is empty");

	const std::string fileName = libraryName + ".dll";

#if defined(_WIN32) || defined(_WIN64)
	//Disable error popup
	SetErrorMode(SEM_FAILCRITICALERRORS);
#if defined(VISUAL_STUDIO)
	library = LoadLibraryA(fileName.c_str()); // Visual Studio
#else
	library = LoadLibrary(fileName.c_str()); // MSVC Compiler
#endif
	SetErrorMode(0);

	const bool result = library != nullptr;
	if (!result)
	{
		// "https://learn.microsoft.com/en-us/windows/win32/debug/system-error-codes--0-499-"
		const int errorCode = GetLastError();
		if(errorCode == 127)
			Debug::PrintError("[DynamicLibrary::LoadGameLibrary] Failed to load library (wrong version): " + fileName, true);
		else
			Debug::PrintError("[DynamicLibrary::LoadGameLibrary] Library not found, error code:" + std::to_string(errorCode) + " file: " + fileName, true);
	}

	ProjectManager::s_projectSettings.isCompiled = result;
#endif
}

void DynamicLibrary::UnloadGameLibrary()
{
#if defined(_WIN32) || defined(_WIN64)
	if (library != nullptr)
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
#endif
}

std::unique_ptr<GameInterface> DynamicLibrary::CreateGame()
{	
	GameInterface* gameInterface = nullptr;

#if defined(_WIN32) || defined(_WIN64)
	if (library != nullptr)
	{
		// Find the "CreateGame" function
		CreateGameFunction ProcAdd = reinterpret_cast<CreateGameFunction>(GetProcAddress(library, "CreateGame"));
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
#endif
	return std::unique_ptr<GameInterface> (gameInterface);
}
#endif