#pragma once

#include <string>
#include <vector>
#include <memory>

class Renderer;
class GameObject;
class Component;

void Destroy(std::weak_ptr<GameObject> gameObject);
void Destroy(Component* component);
std::weak_ptr<GameObject> CreateGameObject();
std::weak_ptr<GameObject> CreateGameObject(std::string name);

class Engine {
public:
	static int Init(const std::string exePath);
	static void Stop();
	static void AddGameObject(std::shared_ptr<GameObject> gameObject);
	static std::vector<std::weak_ptr<GameObject>> GetGameObjects();
	static void Loop();
	static bool componentsListDirty;
	static bool drawOrderListDirty;
	static std::vector<Component*> orderedComponents;
	static int componentsCount;
	static std::weak_ptr<GameObject> selectedGameObject;
	static int gameObjectCount;
	static Renderer* renderer;
	static void SetSelectedGameObject(std::weak_ptr<GameObject> go);
	static std::vector<std::shared_ptr<GameObject>> gameObjects;
	static std::vector<std::weak_ptr<GameObject>> gameObjectsToDestroy;
	static std::vector<Component*> componentsToDestroy;

private:
	static void UpdateComponents();
};