#pragma once

#include <string>
#include <vector>
#include <memory>

class Renderer;
class GameObject;
class Component;

void Destroy(std::weak_ptr<GameObject> gameObject);
void Destroy(std::weak_ptr<Component> component);

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