#pragma once

#include <vector>
#include "scene.h"

class SceneManager {
private:
	std::vector<Scene*> loadedScenes;
	Scene* usedScene;
public:
	void addGameObjectToScene(GameObject* gameObject);
	void setCurrentScene(Scene* scene);
};