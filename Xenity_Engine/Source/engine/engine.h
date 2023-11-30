#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <engine/platform.h>
#include <mutex>

class Renderer;
class GameObject;
class Component;
class Transform;
class GameInterface;
class FileReference;
class ProjectDirectory;
class Texture;
class Shader;
class Material;

class Engine
{
public:

	/**
	* Init Engine
	*/
	[[nodiscard]] API static int Init();

	/**
	* Stop engine
	*/
	API static void Stop();

	/**
	* Engine loop
	*/
	API static void Loop();

	/**
	* Quit game
	*/
	API static void Quit();

	/**
	* Register all engine components
	*/
	API static void RegisterEngineComponents();

	/**
	* Get if the engine is running
	*/
	static bool IsRunning()
	{
		return isRunning;
	}

	static std::unique_ptr<GameInterface> game;

	static bool canUpdateAudio;
	static std::vector<std::shared_ptr<FileReference>> threadLoadedFiles;
	static std::mutex threadLoadingMutex;

	/**
	* Get the renderer
	*/
	API static Renderer& GetRenderer()
	{
		return *renderer;
	}

private:

	/**
	* Create some benchmark for profiling
	*/
	API static void CreateBenchmarks();

	/**
	* Delete pointers when the reference count is low
	*/
	API static void RemoveUnusedFiles();

	/**
	* Finish file loading when files are loaded with a thread
	*/
	API static void FinishThreadedFileLoading();

	/**
	* Reset transforms states
	*/
	API static void ResetTransformsStates();

	/**
	* Check event (SDL)
	*/
	API static void CheckEvents();

	static std::unique_ptr<Renderer> renderer;
	static bool isRunning;
};