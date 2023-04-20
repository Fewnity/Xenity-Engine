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
	static void DrawProfiler();
	static void DrawInspector();
	static void DrawHierarchy();
	static void SetSelectedGameObject(GameObject*);

private:
	static std::vector<GameObject*> gameObjects;
	static int gameObjectCount;
	static void UpdateComponents();
	static GameObject* selectedGameObject;
	static void DrawTreeItem(GameObject* child);
};