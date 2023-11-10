#include "gameplay_manager.h"
#include "../engine.h"
#include "../../xenity.h"

int GameplayManager::gameObjectCount = 0;
int GameplayManager::gameObjectEditorCount = 0;
bool GameplayManager::componentsListDirty = true;
std::vector<std::weak_ptr<Component>> GameplayManager::orderedComponents;
int GameplayManager::componentsCount = 0;
std::vector<std::shared_ptr<GameObject>> GameplayManager::gameObjects;
std::vector<std::shared_ptr<GameObject>> GameplayManager::gameObjectsEditor;
std::vector<std::weak_ptr<GameObject>> GameplayManager::gameObjectsToDestroy;
std::vector<std::shared_ptr<Component>> GameplayManager::componentsToDestroy;

GameState GameplayManager::gameState = Stopped;

void GameplayManager::AddGameObject(const std::shared_ptr<GameObject>& gameObject)
{
	gameObjects.push_back(gameObject);
	gameObjectCount++;
}

void GameplayManager::AddGameObjectEditor(const std::shared_ptr<GameObject>& gameObject)
{
	gameObjectsEditor.push_back(gameObject);
	gameObjectEditorCount++;
}

std::vector<std::shared_ptr<GameObject>> GameplayManager::GetGameObjects()
{
	return std::vector<std::shared_ptr<GameObject>>(GameplayManager::gameObjects);
}

void GameplayManager::SetGameState(GameState _gameState, bool restoreScene)
{
#if defined(EDITOR)
	if (_gameState == Playing)
	{
		gameState = Starting;
		SceneManager::SaveScene(SaveSceneForPlayState);
		SceneManager::RestoreScene();
		gameState = _gameState;
	}
	else if (_gameState == Stopped)
	{
		gameState = _gameState;
		if(restoreScene)
			SceneManager::RestoreScene();
	}
#else
	gameState = _gameState;
#endif
}

void GameplayManager::UpdateComponents()
{
	// Order components and initialise new components
	if (componentsListDirty)
	{
		componentsListDirty = false;
		orderedComponents.clear();

		componentsCount = 0;
		OrderComponents();

		if (GetGameState() == GameState::Playing)
			InitialiseComponents();
	}

	if (GetGameState() == GameState::Playing)
	{
		// Update components
		for (int i = 0; i < componentsCount; i++)
		{
			if (auto component = orderedComponents[i].lock())
			{
				if (component->GetGameObject()->GetLocalActive() && component->GetIsEnabled())
				{
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
}

void GameplayManager::OrderComponents()
{
	for (int gIndex = 0; gIndex < gameObjectCount; gIndex++)
	{
		std::shared_ptr<GameObject>& gameObjectToCheck = gameObjects[gIndex];
		if (gameObjectToCheck)
		{
			if (gameObjectToCheck->GetActive())
			{
				int goComponentCount = (int)gameObjectToCheck->GetComponentCount();
				bool placeFound = false;
				for (int cIndex = 0; cIndex < goComponentCount; cIndex++)
				{
					std::shared_ptr<Component>& componentToCheck = gameObjectToCheck->components[cIndex];
					if (componentToCheck)
					{
						for (int i = 0; i < componentsCount; i++)
						{
							// Check if the checked has a higher priority (lower value) than the component in the list
							if (componentToCheck->updatePriority <= orderedComponents[i].lock()->updatePriority)
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
}

void GameplayManager::InitialiseComponents()
{
	int componentsToInitCount = 0;
	std::vector<std::shared_ptr<Component>> orderedComponentsToInit;
	// Find uninitiated components and order them
	for (int i = 0; i < componentsCount; i++)
	{
		if (auto componentToCheck = orderedComponents[i].lock())
		{
			if (!componentToCheck->initiated && componentToCheck->GetIsEnabled())
			{
				orderedComponentsToInit.push_back(componentToCheck);
				componentsToInitCount++;
			}
		}
	}

	// Init components
	for (int i = 0; i < componentsToInitCount; i++)
	{
		orderedComponentsToInit[i]->Start();
		orderedComponentsToInit[i]->initiated = true;
	}
}

void GameplayManager::RemoveDestroyedGameObjects()
{
	// Remove destroyed GameObjects from the Engine's GameObjects list
	int gameObjectToDestroyCount = (int)gameObjectsToDestroy.size();
	for (int i = 0; i < gameObjectToDestroyCount; i++)
	{
		for (int gIndex = 0; gIndex < gameObjectCount; gIndex++)
		{
			std::shared_ptr<GameObject> gameObjectToCheck = gameObjects[gIndex];
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
	int componentToDestroyCount = (int)componentsToDestroy.size();
	for (int i = 0; i < componentToDestroyCount; i++)
	{
		std::shared_ptr<Component> component = componentsToDestroy[i];
		if (component)
		{
			Engine::RemoveComponentReferences(component);
		}
	}
	componentsToDestroy.clear();
}