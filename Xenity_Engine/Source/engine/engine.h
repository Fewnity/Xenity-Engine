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

class Renderer;
class GameObject;
class Component;
class Transform;
class GameInterface;
class FileReference;
class ProjectDirectory;

/**
* Destroy a gameObject
*/
API void Destroy(std::weak_ptr<GameObject> gameObject);

/**
* Destroy a component
*/
API void Destroy(std::weak_ptr<Component> component);

/**
* Check if a GameObject or a Component is valid
*/
template <typename T>
bool IsValid(std::shared_ptr<T> pointer)
{
	return IsValid(std::weak_ptr<T>(pointer));
}

/**
* Check of a GameObject or a Component is valid
*/
template <typename T>
bool IsValid(std::weak_ptr<T> pointer)
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

enum GameState 
{
	Stopped,
	Paused,
	Starting,
	Playing
};

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
	* Add a component into the game
	* @param gameObject GameObject to add
	*/
	API static void AddGameObject(std::shared_ptr<GameObject> gameObject);

	/**
	* Add a component into the engine only (Not visible from the game)
	* @param gameObject GameObject to add
	*/
	API static void AddGameObjectEditor(std::shared_ptr<GameObject> gameObject);

	/**
	* Get all GameObjects
	*/
	API static std::vector<std::shared_ptr<GameObject>> GetGameObjects();

	/**
	* Engine loop
	*/
	API static void Loop();

	/**
	* Set selected GameObject
	* @param go New selected GameObject
	*/
	API static void SetSelectedGameObject(std::weak_ptr<GameObject> go);

	/**
	* Quit game
	*/
	API static void Quit();

	/**
	* Register all engine components
	*/
	API static void RegisterEngineComponents();

	/**
	* Set game state
	* @param _gameState New game state
	*/
	API static void SetGameState(GameState _gameState);

	/**
	* Get game state
	*/
	API static GameState GetGameState() 
	{
		return gameState;
	}

	static bool IsRunning()
	{
		return isRunning;
	}

	static GameInterface* game;
	static bool componentsListDirty;
	static bool drawOrderListDirty;
	static std::vector<std::weak_ptr<Component>> orderedComponents;
	static int componentsCount;
	static std::weak_ptr<GameObject> selectedGameObject;
	static FileReference* selectedFileReference;
	static ProjectDirectory* currentProjectDirectory;
	static int gameObjectCount;
	static int gameObjectEditorCount;
	static Renderer *renderer;
	static std::vector<std::shared_ptr<GameObject>> gameObjects;
	static std::vector<std::shared_ptr<GameObject>> gameObjectsEditor;
	static std::vector<std::weak_ptr<GameObject>> gameObjectsToDestroy;
	static std::vector<std::weak_ptr<Component>> componentsToDestroy;
	static bool canUpdateAudio;

private:

	/**
	* Update all active components
	*/
	API static void UpdateComponents();

	/**
	* Set Cpu speed to the max (PSP and PsVita)
	*/
	API static void SetMaxCpuSpeed();

	/**
	* Load and start game (Executed when compiled as a game build)
	*/
	API static int LoadGame();

	/**
	* Check event (SDL)
	*/
	API static void CheckEvents();

	static bool isRunning;
	static GameState gameState;
};