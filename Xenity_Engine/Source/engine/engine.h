#pragma once
#include <engine/api.h>

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

private:
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