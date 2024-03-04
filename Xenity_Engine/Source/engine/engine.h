#pragma once
#include <engine/api.h>

#include <string>
#include <vector>
#include <memory>
#include <engine/platform.h>
#include <engine/event_system/event_system.h>
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

#define ENGINE_VERSION "0.1"
#define ENGINE_DLL_VERSION "2" // Increase by one this number to invalidate compiled Dlls of games projects

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
	 * Get if the engine is running
	 */
	static bool IsRunning(bool checkRenderer)
	{
		return isRunning && (!checkRenderer || renderer != nullptr);
	}

	static std::unique_ptr<GameInterface> game;

	static bool canUpdateAudio;

	/**
	 * Get the renderer
	 */
	API static Renderer &GetRenderer()
	{
		return *renderer;
	}

	static Event<>* GetOnWindowFocusEvent() 
	{
		return OnWindowFocusEvent;
	}

private:
	static Event<>* OnWindowFocusEvent;

	static void OnCloseSignal(int);

	/**
	 * Create some benchmark for profiling
	 */
	static void CreateBenchmarks();

	/**
	 * Check event (SDL)
	 */
	static void CheckEvents();

	static std::unique_ptr<Renderer> renderer;
	static bool isRunning;
};