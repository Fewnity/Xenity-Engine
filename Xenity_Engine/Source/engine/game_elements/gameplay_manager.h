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

class GameObject;
class Component;

enum GameState
{
	Stopped,
	Paused,
	Starting,
	Playing
};

class API GameplayManager
{
public:
	/**
	* Add a component into the game
	* @param gameObject GameObject to add
	*/
	static void AddGameObject(const std::shared_ptr<GameObject>& gameObject);

	/**
	* Add a component into the engine only (Not visible from the game)
	* @param gameObject GameObject to add
	*/
	static void AddGameObjectEditor(const std::shared_ptr<GameObject>& gameObject);

	/**
	* Get all GameObjects
	*/
	static std::vector<std::shared_ptr<GameObject>> GetGameObjects();

	static bool componentsListDirty;

	static std::vector<std::weak_ptr<Component>> orderedComponents;
	static int componentsCount;
	static int gameObjectCount;
	static int gameObjectEditorCount;
	static std::vector<std::shared_ptr<GameObject>> gameObjects;
	static std::vector<std::shared_ptr<GameObject>> gameObjectsEditor;
	static std::vector<std::weak_ptr<GameObject>> gameObjectsToDestroy;
	static std::vector<std::shared_ptr<Component>> componentsToDestroy;

	/**
	* Update all active components
	*/
	static void UpdateComponents();

	static void OrderComponents();
	static void InitialiseComponents();
	static void RemoveDestroyedGameObjects();
	static void RemoveDestroyedComponents();

	/**
	* Set game state
	* @param _gameState New game state
	*/
	static void SetGameState(GameState _gameState, bool restoreScene);

	/**
	* Get game state
	*/
	static GameState GetGameState()
	{
		return gameState;
	}

private:

	static GameState gameState;

};

