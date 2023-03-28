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
private:
	static std::vector<GameObject*> gameObjects;
	static int gameObjectCount;
	static void UpdateComponents();

};