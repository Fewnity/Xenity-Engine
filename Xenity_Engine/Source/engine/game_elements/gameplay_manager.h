#pragma once

/**
 * [Internal]
 */

#include <string>
#include <vector>
#include <memory>
#include <engine/event_system/event_system.h>

class GameObject;
class Component;

enum class GameState
{
	Stopped,
	Paused,
	Starting,
	Playing
};

class GameplayManager
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
	static std::weak_ptr<Component> lastUpdatedComponent;

	/**
	* Update all active components
	*/
	static void UpdateComponents();

	/**
	* Order components by their update order
	*/
	static void OrderComponents();

	/**
	* Initialise all components
	*/
	static void InitialiseComponents();

	/**
	* Remove destroyed GameObjects
	*/
	static void RemoveDestroyedGameObjects();

	/**
	* Remove destroyed components
	*/
	static void RemoveDestroyedComponents();

	/**
	* Set game state
	* @param _gameState New game state
	* @param restoreScene If true, the scene will be restored
	*/
	static void SetGameState(GameState _gameState, bool restoreScene);

	/**
	* Get game state
	*/
	static GameState GetGameState()
	{
		return gameState;
	}

	/**
	* Get the OnPlay event
	*/
	static Event<>& GetOnPlayEvent() 
	{
		return OnPlayEvent;
	}

private:
	static Event<> OnPlayEvent;

	static GameState gameState;

};

