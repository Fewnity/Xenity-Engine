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

/*
* @brief Class used to manage the engine (Init, Stop...)
*/
class Engine
{
public:
	/**
	 * @brief Init Engine
	 */
	[[nodiscard]] API static int Init();

	/**
	 * @brief Stop engine
	 */
	API static void Stop();

	/**
	 * @brief Engine loop
	 */
	API static void Loop();

	/**
	 * @brief Quit game
	 */
	API static void Quit();

	/**
	 * @brief Get if the engine is running
	 * @param checkRenderer Check if the renderer exists
	 */
	static bool IsRunning(bool checkRenderer)
	{
		return isRunning && (!checkRenderer || renderer != nullptr);
	}

	static std::unique_ptr<GameInterface> game;

	static bool canUpdateAudio;

	/**
	 * @brief Get the renderer
	 */
	API static Renderer &GetRenderer()
	{
		return *renderer;
	}

	/**
	 * @brief Get the OnWindowFocus event
	 */
	static Event<>* GetOnWindowFocusEvent() 
	{
		return OnWindowFocusEvent;
	}

private:
	static Event<>* OnWindowFocusEvent;

	/**
	 * @brief Close signal, called when the software is closing
	 */
	static void OnCloseSignal(int);

	/**
	 * @brief Create some benchmark for profiling
	 */
	static void CreateBenchmarks();

	/**
	 * @brief Check event (SDL)
	 */
	static void CheckEvents();

	static std::unique_ptr<Renderer> renderer;
	static bool isRunning;
};