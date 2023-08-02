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

class GameInterface;

enum BuildType {
	EditorHotReloading,
	BuildGame,
	BuildAndRunGame,
};

class API DynamicLibrary
{
public:
	static void LoadGameLibrary(std::string libraryName);
	static void UnloadGameLibrary();
	static void CompileGame(BuildType buildType);
	static void StartGame();
	static GameInterface *CreateGame();
	static std::string GetStartCompilerCommand();
	static std::string GetAddNextCommand();
	static std::string GetNavToEngineFolderCommand();
};
#endif