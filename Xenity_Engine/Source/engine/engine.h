#pragma once

#include <string>
#include <vector>
#include <memory>

class Renderer;
class GameObject;
class Component;
class Transform;

//#include "game_elements/gameobject.h"
//#include "component.h"

void Destroy(std::weak_ptr<GameObject> gameObject);
void Destroy(std::weak_ptr<Component> component);

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

class Engine
{
public:
	static int Init(const std::string exePath);
	static void Stop();
	static void AddGameObject(std::shared_ptr<GameObject> gameObject);
	static std::vector<std::weak_ptr<GameObject>> GetGameObjects();
	static void Loop();
	static bool componentsListDirty;
	static bool drawOrderListDirty;
	static std::vector<std::weak_ptr<Component>> orderedComponents;
	static int componentsCount;
	static std::weak_ptr<GameObject> selectedGameObject;
	static int gameObjectCount;
	static Renderer *renderer;
	static void SetSelectedGameObject(std::weak_ptr<GameObject> go);
	static std::vector<std::shared_ptr<GameObject>> gameObjects;
	static std::vector<std::weak_ptr<GameObject>> gameObjectsToDestroy;
	static std::vector<std::weak_ptr<Component>> componentsToDestroy;
	static bool valueFree;

private:
	static void UpdateComponents();
};