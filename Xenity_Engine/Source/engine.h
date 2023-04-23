#pragma once

#include "lighting/lighting.h"
#include "audio/audio.h"
#include "scene_manager/scene.h"

class Engine {
public:
	static int Init(const std::string exePath);
	static void AddGameObject(GameObject* gameObject);
	static std::vector<GameObject*> GetGameObjects();
	static void Loop();
	static bool componentsListDirty;
	static std::vector<Component*> orderedComponents;
	static int componentsCount;
	static GameObject* selectedGameObject;
	static int gameObjectCount;

	static void SetSelectedGameObject(GameObject*);
private:
	static std::vector<GameObject*> gameObjects;
	static void UpdateComponents();
};