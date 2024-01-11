#pragma once
#if defined(_WIN32) || defined(_WIN64)

#include <string>
#include <engine/platform.h>
#include <memory>

class GameInterface;

class DynamicLibrary
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
	static std::unique_ptr<GameInterface> CreateGame();
private:

};
#endif