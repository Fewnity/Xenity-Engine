// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

/**
 * [Internal]
 */

#include <vector>
#include <memory>
#include <engine/event_system/event_system.h>
#include <bitset>

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
	static void Stop();

	/**
	* @brief Add a component into the game
	* @param gameObject GameObject to add
	*/
	static void AddGameObject(const std::shared_ptr<GameObject>& gameObject);

#if defined(EDITOR)
	/**
	* @brief Add a component into the engine only (Not visible from the game)
	* @param gameObject GameObject to add
	*/
	static void AddGameObjectEditor(const std::shared_ptr<GameObject>& gameObject);
#endif

	/**
	* @brief Get all GameObjects
	*/
	static const std::vector<std::shared_ptr<GameObject>>& GetGameObjects();

	static bool componentsListDirty;
	static bool componentsInitListDirty;
	static int gameObjectCount;
	static std::vector<std::shared_ptr<GameObject>> gameObjects;
#if defined(EDITOR)
	static int gameObjectEditorCount;
	static std::vector<std::shared_ptr<GameObject>> gameObjectsEditor;
#endif
	static std::vector<std::weak_ptr<GameObject>> gameObjectsToDestroy;
	static std::vector<std::shared_ptr<Component>> componentsToDestroy;

	/**
	* @brief Update all active components
	*/
	static void UpdateComponents();

	/**
	* @brief Order components by their update order
	*/
	static void OrderComponents();

	/**
	* @brief Initialise all components
	*/
	static void InitialiseComponents();

	/**
	* @brief Remove destroyed GameObjects
	*/
	static void RemoveDestroyedGameObjects();

	/**
	* @brief Remove destroyed components
	*/
	static void RemoveDestroyedComponents();

	/**
	* @brief Set game state
	* @param _gameState New game state
	* @param restoreScene If true, the scene will be restored
	*/
	static void SetGameState(GameState _gameState, bool restoreScene);

	/**
	* @brief Get game state
	*/
	static inline GameState GetGameState()
	{
		return s_gameState;
	}

	/**
	* @brief Get the OnPlay event
	*/
	static inline Event<>& GetOnPlayEvent()
	{
		return s_OnPlayEvent;
	}

	/**
	* Get a weak pointer to the last updated component (only for debug)
	*/
	static const std::weak_ptr<Component>& GetLastUpdatedComponent()
	{
		return s_lastUpdatedComponent;
	}

private:
	static std::weak_ptr<Component> s_lastUpdatedComponent;

	static Event<> s_OnPlayEvent;

	static GameState s_gameState;

};

