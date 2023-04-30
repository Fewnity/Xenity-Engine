#pragma once

#include <vector>
#include "../game_elements/gameobject.h"

class Scene
{
public:
	Scene();
	~Scene();
	std::vector<GameObject*> gameObjects;
	void Load();
	void Unload();
};

