// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "gameplay_manager.h"

#if defined(EDITOR)
#include <editor/editor.h>
#include <editor/ui/menus/game_menu.h>
#include <editor/command/command_manager.h>
#endif

#include <engine/scene_management/scene_manager.h>
#include <engine/game_elements/gameobject.h>
#include <engine/component.h>
#include <engine/tools/scope_benchmark.h>
#include <engine/debug/performance.h>
#include <engine/debug/stack_debug_object.h>
#include <engine/time/time.h>

int GameplayManager::gameObjectCount = 0;
bool GameplayManager::componentsListDirty = true;
bool GameplayManager::componentsInitListDirty = true;
std::vector<std::weak_ptr<Component>> GameplayManager::orderedComponents;
int GameplayManager::componentsCount = 0;
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

	if (auto menu = Editor::lastFocusedGameMenu.lock())
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
		orderedComponents.clear();

		componentsCount = 0;
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
		for (int i = 0; i < componentsCount; i++)
		{
			if (const std::shared_ptr<Component> component = orderedComponents[i].lock())
			{
				if (component->GetGameObjectRaw()->IsLocalActive() && component->IsEnabled())
				{
#if defined(_WIN32) || defined(_WIN64)
					s_lastUpdatedComponent = component;
#endif
					component->Update();
				}
			}
			else
			{
				orderedComponents.erase(orderedComponents.begin() + i);
				i--;
				componentsCount--;
			}
		}
	}
	s_lastUpdatedComponent.reset();
}

void GameplayManager::OrderComponents()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	for(const std::shared_ptr<GameObject>& gameObjectToCheck : gameObjects)
	{
		if (gameObjectToCheck)
		{
			const int goComponentCount = gameObjectToCheck->GetComponentCount();
			bool placeFound = false;
			for (int cIndex = 0; cIndex < goComponentCount; cIndex++)
			{
				const std::shared_ptr<Component>& componentToCheck = gameObjectToCheck->m_components[cIndex];
				if (componentToCheck)
				{
					for (int i = 0; i < componentsCount; i++)
					{
						// Check if the checked has a higher priority (lower value) than the component in the list
						if (componentToCheck->m_updatePriority <= orderedComponents[i].lock()->m_updatePriority)
						{
							orderedComponents.insert(orderedComponents.begin() + i, componentToCheck);
							placeFound = true;
							break;
						}
					}
					// if the priority is lower than all components's priorities in the list, add it the end of the list
					if (!placeFound)
					{
						orderedComponents.push_back(componentToCheck);
					}
					componentsCount++;
				}
			}
		}
	}
}

void GameplayManager::InitialiseComponents()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	int componentsToInitCount = 0;
	std::vector<std::shared_ptr<Component>> orderedComponentsToInit;
	// Find uninitiated components and order them
	for (int i = 0; i < componentsCount; i++)
	{
		if (auto componentToCheck = orderedComponents[i].lock())
		{
			if (!componentToCheck->m_initiated && componentToCheck->IsEnabled() && componentToCheck->GetGameObject()->IsLocalActive())
			{
				orderedComponentsToInit.push_back(componentToCheck);
				componentsToInitCount++;
			}
		}
	}

	// Init components
	for (int i = 0; i < componentsToInitCount; i++)
	{
		s_lastUpdatedComponent = orderedComponentsToInit[i];
		orderedComponentsToInit[i]->Start();
		orderedComponentsToInit[i]->m_initiated = true;
	}
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

	const size_t componentToDestroyCount = componentsToDestroy.size();
	for (size_t i = 0; i < componentToDestroyCount; i++)
	{
		const std::shared_ptr<Component>& component = componentsToDestroy[i];
		if (component)
		{
			component->RemoveReferences();
		}
	}
	componentsToDestroy.clear();
}