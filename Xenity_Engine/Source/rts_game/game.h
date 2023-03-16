#pragma once

#include "../graphics/tile_map.h"
#include "../graphics/camera.h"

class Game
{
public:
	void Init();
	void Loop();
private:
	GameObject* cameraGameObject = new GameObject("cameraGameObject");

	Camera* camera = new Camera();

	GameObject* gameObjectSprite = new GameObject("gameObjectSprite");
	GameObject* gameObjectTileMap = new GameObject("TileMap");
	TileMap* tileMap;
	TileMap* tileMapProps;
};

