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
#include "platform.h"
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

/**
* Destroy a gameObject
*/
API void Destroy(const std::weak_ptr<GameObject>& gameObject);

/**
* Destroy a component
*/
API void Destroy(const std::weak_ptr<Component>& component);

/**
*Destroy a gameObject
*/
API void Destroy(const std::shared_ptr<GameObject>&gameObject);

/**
* Destroy a component
*/
API void Destroy(const std::shared_ptr<Component>& component);

/**
* Check if a GameObject or a Component is valid
*/
template <typename T>
bool IsValid(const std::shared_ptr<T>& pointer)
{
	return IsValid(std::weak_ptr<T>(pointer));
}

/**
* Check of a GameObject or a Component is valid
*/
template <typename T>
bool IsValid(const std::weak_ptr<T>& pointer)
{
	bool valid = true;
	if (auto lockPointer = pointer.lock())
	{
		if (auto component = std::dynamic_pointer_cast<Component>(lockPointer))
		{
			if (component->waitingForDestroy)
			{
				valid = false;
			}
		}
		else if (auto gameObject = std::dynamic_pointer_cast<GameObject>(lockPointer))
		{
			if (gameObject->waitingForDestroy)
			{
				valid = false;
			}
		}
		else if (auto transform = std::dynamic_pointer_cast<Transform>(lockPointer))
		{
			if (!IsValid(transform->GetGameObject()))
			{
				valid = false;
			}
		}
	}
	else
	{
		valid = false;
	}
	return valid;
}

class Engine
{
public:

	/**
	* Init Engine
	*/
	API static int Init();

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

	static bool IsRunning()
	{
		return isRunning;
	}

	static std::unique_ptr<GameInterface> game;

	static bool canUpdateAudio;
	static std::vector<std::shared_ptr<FileReference>> threadLoadedFiles;
	static std::mutex threadLoadingMutex;

	/**
	* Destroy a component
	*/
	API static void RemoveComponentReferences(const std::shared_ptr<Component>& component);

	API static Renderer& GetRenderer()
	{
		return *renderer;
	}

private:
	static std::unique_ptr<Renderer> renderer;

	API static void CreateBenchmarks();


	API static void RemoveUnusedFiles();

	API static void FinishThreadedFileLoading();
	API static void ResetTransformsStates();

	/**
	* Load and start game (Executed when compiled as a game build)
	*/
	API static int LoadGame();

	/**
	* Check event (SDL)
	*/
	API static void CheckEvents();

	static bool isRunning;
};