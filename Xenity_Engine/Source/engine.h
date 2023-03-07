#pragma once

#include "lighting/lighting.h"
#include "vectors/vector2.h"
#include "vectors/vector3.h"
#include "vectors/vector4.h"
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