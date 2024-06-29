// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

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
	* @brief Add a component into the game
	* @param gameObject GameObject to add
	*/
	static void AddGameObject(const std::shared_ptr<GameObject>& gameObject);

	/**
	* @brief Add a component into the engine only (Not visible from the game)
	* @param gameObject GameObject to add
	*/
	static void AddGameObjectEditor(const std::shared_ptr<GameObject>& gameObject);

	/**
	* @brief Get all GameObjects
	*/
	static const std::vector<std::shared_ptr<GameObject>>& GetGameObjects();

	static bool componentsListDirty;
	static bool componentsInitListDirty;
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
		return gameState;
	}

	/**
	* @brief Get the OnPlay event
	*/
	static inline Event<>& GetOnPlayEvent()
	{
		return OnPlayEvent;
	}

private:
	static Event<> OnPlayEvent;

	static GameState gameState;

};

