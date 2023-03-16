#pragma once

#include "lighting/lighting.h"
#include "audio/audio.h"
#include "scene_manager/scene.h"

class Engine {
public:
	static int Init(const std::string exePath);
	static void Loop();
	static void AddGameObject(GameObject* gameObject);
	static std::vector<GameObject*> GetGameObjects();

private:
	static std::vector<GameObject*> gameObjects;
	static void UpdateComponents();

};