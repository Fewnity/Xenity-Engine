#pragma once

#include "../graphics/tile_map.h"
#include "../graphics/camera.h"

class PathFinding
{
public:
	void Init();
	void Loop();
private:
	void LoadGameData();
	GameObject* cameraGameObject = new GameObject("cameraGameObject");
	Camera* camera = new Camera();
	int** nodes;
};

