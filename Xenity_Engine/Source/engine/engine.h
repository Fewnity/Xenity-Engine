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

API void Destroy(std::weak_ptr<GameObject> gameObject);
API void Destroy(std::weak_ptr<Component> component);

template <typename T>
bool IsValid(std::shared_ptr<T> pointer)
{
	return IsValid(std::weak_ptr<T>(pointer));
}

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
	Playing
};

class Engine
{
public:
	API static int Init();
	API static void Stop();
	API static void AddGameObject(std::shared_ptr<GameObject> gameObject);
	API static void AddGameObjectEditor(std::shared_ptr<GameObject> gameObject);
	API static std::vector<std::shared_ptr<GameObject>> GetGameObjects();
	API static void Loop();

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
	API static void SetSelectedGameObject(std::weak_ptr<GameObject> go);
	static std::vector<std::shared_ptr<GameObject>> gameObjects;
	static std::vector<std::shared_ptr<GameObject>> gameObjectsEditor;
	static std::vector<std::weak_ptr<GameObject>> gameObjectsToDestroy;
	static std::vector<std::weak_ptr<Component>> componentsToDestroy;
	static bool valueFree;
	API static void Quit();
	API static void EmptyScene();
	API static void RegisterEngineComponents();
	API static void SetGameState(GameState _gameState);

	API static GameState GetGameState() 
	{
		return gameState;
	}

	static GameInterface* game;
private:
	API static void UpdateComponents();
	static bool isRunning;
	static GameState gameState;
};