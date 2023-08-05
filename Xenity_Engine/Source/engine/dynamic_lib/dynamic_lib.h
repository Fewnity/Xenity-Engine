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
	static void LoadGameLibrary(std::string libraryName);
	static void UnloadGameLibrary();
	static GameInterface *CreateGame();
private:

};
#endif