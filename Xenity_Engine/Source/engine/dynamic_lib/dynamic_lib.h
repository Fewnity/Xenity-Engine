#pragma once

/**
 * [Internal]
 */

#if defined(_WIN32) || defined(_WIN64)

#include <string>
#include <engine/platform.h>
#include <memory>

class GameInterface;

/**
 * Used to load/unload a game DLL (used only on Windows)
 */
class DynamicLibrary
{
public:

	/**
	* Load a game DLL
	* @param libraryName Name of the library
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