#pragma once

#include <vector>
#include "../gameobject.h"

class Scene
{
public:
	std::vector<GameObject*> gameObjects;
	void addGameObject(GameObject* gameObject);
};

