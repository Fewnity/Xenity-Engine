// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "gameplay_manager.h"

#if defined(EDITOR)
#include <editor/editor.h>
#include <editor/ui/menus/basic/game_menu.h>
#include <editor/command/command_manager.h>
#endif

#include <engine/scene_management/scene_manager.h>
#include <engine/game_elements/gameobject.h>
#include <engine/component.h>
#include <engine/tools/scope_benchmark.h>
#include <engine/debug/performance.h>
#include <engine/debug/stack_debug_object.h>
#include <engine/time/time.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/game_elements/component_manager.h>

int GameplayManager::gameObjectCount = 0;
bool GameplayManager::componentsListDirty = true;
bool GameplayManager::componentsInitListDirty = true;
std::vector<std::shared_ptr<GameObject>> GameplayManager::gameObjects;
#if defined(EDITOR)
int GameplayManager::gameObjectEditorCount = 0;
std::vector<std::shared_ptr<GameObject>> GameplayManager::gameObjectsEditor;
#endif
std::vector<std::weak_ptr<GameObject>> GameplayManager::gameObjectsToDestroy;
std::vector<std::shared_ptr<Component>> GameplayManager::componentsToDestroy;
std::weak_ptr<Component> GameplayManager::s_lastUpdatedComponent;
Event<> GameplayManager::s_OnPlayEvent;

GameState GameplayManager::s_gameState = GameState::Stopped;

std::unordered_map<size_t, std::unique_ptr<BaseComponentList>> ComponentManager::componentLists;

void GameplayManager::Stop()
{
	s_lastUpdatedComponent.reset();
	componentsToDestroy.clear();
	gameObjectsToDestroy.clear();
	gameObjects.clear();
#if defined(EDITOR)
	gameObjectsEditor.clear();
#endif
	gameObjectCount = 0;
#if defined(EDITOR)
	gameObjectEditorCount = 0;
#endif
	componentsListDirty = true;
	componentsInitListDirty = true;
	s_gameState = GameState::Stopped;
	s_OnPlayEvent.UnbindAll();;
}

void GameplayManager::AddGameObject(const std::shared_ptr<GameObject>& gameObject)
{
	XASSERT(gameObject != nullptr, "[GameplayManager::AddGameObject] gameObject is nullptr");

	gameObjects.push_back(gameObject);
	gameObjectCount++;
}

#if defined(EDITOR)
void GameplayManager::AddGameObjectEditor(const std::shared_ptr<GameObject>& gameObject)
{
	XASSERT(gameObject != nullptr, "[GameplayManager::AddGameObjectEditor] gameObject is nullptr");

	gameObjectsEditor.push_back(gameObject);
	gameObjectEditorCount++;
}
#endif

const std::vector<std::shared_ptr<GameObject>>& GameplayManager::GetGameObjects()
{
	return GameplayManager::gameObjects;
}

void GameplayManager::SetGameState(GameState newGameState, bool restoreScene)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

#if defined(EDITOR)
	if (newGameState == GameState::Playing && s_gameState == GameState::Stopped) // Start game
	{
		s_gameState = GameState::Starting;
		SceneManager::SaveScene(SaveSceneType::SaveSceneForPlayState);
		SceneManager::RestoreScene();
		s_gameState = newGameState;
		s_OnPlayEvent.Trigger();
		Time::Reset();
	}
	else if (newGameState == GameState::Stopped && s_gameState != GameState::Stopped) // Stop game
	{
		CommandManager::ClearInGameCommands();
		s_gameState = newGameState;
		if (restoreScene)
			SceneManager::RestoreScene();

		AssetManager::ReloadAllMaterials();
	}
	else if ((newGameState == GameState::Paused && s_gameState == GameState::Playing) ||
		(newGameState == GameState::Playing && s_gameState == GameState::Paused)) // Pause / UnPause
	{
		s_gameState = newGameState;
	}
	else if ((newGameState == GameState::Paused && s_gameState == GameState::Paused)) // UnPause
	{
		s_gameState = GameState::Playing;
	}
	else 
	{
		s_gameState = newGameState;
	}

	if (auto menu = Editor::s_lastFocusedGameMenu.lock())
	{
		std::dynamic_pointer_cast<GameMenu>(menu)->needUpdateCamera = true;
	}
#else
	s_gameState = newGameState;
#endif
}

void GameplayManager::UpdateComponents()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	SCOPED_PROFILER("GameplayManager::UpdateComponents", scopeBenchmark);
	// Order components and initialise new components
	if (componentsListDirty)
	{
		componentsListDirty = false;

		OrderComponents();
		componentsInitListDirty = true;
	}

	if (componentsInitListDirty) 
	{
		if (GetGameState() == GameState::Playing) 
		{
			componentsInitListDirty = false;
			InitialiseComponents();
		}
	}

	if (GetGameState() == GameState::Playing)
	{
		// Update components
		ComponentManager::UpdateComponentLists(s_lastUpdatedComponent);
	}
	s_lastUpdatedComponent.reset();
}

void GameplayManager::OrderComponents()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);
}

void GameplayManager::InitialiseComponents()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	ComponentManager::InitComponentLists();
}

void GameplayManager::RemoveDestroyedGameObjects()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	// Remove destroyed GameObjects from the Engine's GameObjects list
	const size_t gameObjectToDestroyCount = gameObjectsToDestroy.size();
	for (size_t i = 0; i < gameObjectToDestroyCount; i++)
	{
		for (int gIndex = 0; gIndex < gameObjectCount; gIndex++)
		{
			const std::shared_ptr<GameObject>& gameObjectToCheck = gameObjects[gIndex];
			if (gameObjectToCheck == gameObjectsToDestroy[i].lock())
			{
				gameObjects.erase(gameObjects.begin() + gIndex);
				break;
			}
		}
		gameObjectCount--;
	}
	gameObjectsToDestroy.clear();
}

void GameplayManager::RemoveDestroyedComponents()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	for (auto& component : componentsToDestroy)
	{
		component->RemoveReferences();
		ComponentManager::RemoveComponent(component);
	}
	componentsToDestroy.clear();
}