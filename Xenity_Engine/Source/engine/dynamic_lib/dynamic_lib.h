#if defined(_WIN32) || defined(_WIN64)
#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once
#include <string>
#include "../platform.h"

class GameInterface;


class API DynamicLibrary
{
public:

	/**
	* Load a game DLL
	*/
	static void LoadGameLibrary(const std::string& libraryName);

	/**
	* Unload the game DLL
	*/
	static void UnloadGameLibrary();

	/**
	* Create a game interface from the DLL
	*/
	static GameInterface* CreateGame();
private:

};
#endif