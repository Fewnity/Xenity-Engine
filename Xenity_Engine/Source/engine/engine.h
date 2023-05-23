#pragma once

#include <string>
#include <vector>

class Renderer;
class GameObject;
class Component;

void Destroy(GameObject* gameObject);
void Destroy(Component* component);

class Engine {
public:
	static int Init(const std::string exePath);
	static void Stop();
	static void AddGameObject(GameObject* gameObject);
	static std::vector<GameObject*> GetGameObjects();
	static void Loop();
	static bool componentsListDirty;
	static bool drawOrderListDirty;
	static std::vector<Component*> orderedComponents;
	static int componentsCount;
	static GameObject* selectedGameObject;
	static int gameObjectCount;
	static Renderer* renderer;
	static void SetSelectedGameObject(GameObject*);
private:
	static std::vector<GameObject*> gameObjects;
	static void UpdateComponents();
};