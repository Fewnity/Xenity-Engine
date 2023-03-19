#pragma once

#include "../graphics/tile_map.h"
#include "../graphics/camera.h"

class Prop {
public:
	int id = 0;
	int x = 0;
	int y = 0;
	void SetPosition(int x, int y) {
		this->x = x;
		this->y = y;
	}
};


class Game
{
public:
class Tile {
	int groundTileId = 0; // 0 is no tile
	Prop* prop = nullptr;
};
	void Init();
	void Loop();
	Game::Tile* GetTile(int x, int y);
	void GenerateMap();

private:
	GameObject* cameraGameObject = new GameObject("cameraGameObject");

	Camera* camera = new Camera();

	GameObject* gameObjectSprite = new GameObject("gameObjectSprite");
	GameObject* gameObjectTileMap = new GameObject("TileMap");
	TileMap* tileMap;
	TileMap* tileMapProps;
	float cameraZoom = 1.5f; //[1;2.8]
	float cameraArrowMoveSpeed = 3;
	std::vector<Prop*> props;
	std::vector<Texture*> propsTextures;

	Tile* tiles = nullptr;

	//Map settings
	int mapSize = 20;
	int minTreeCount = 5;
	int maxTreeCount = 6;
	int minRockCount = 3;
	int maxRockCount = 6;
	int minRockEmeraldCount = 3;
	int maxRockEmeraldCount = 6;
	int minRockGoldCount = 3;
	int maxRockGoldCount = 6;
	int minCrystalCount = 3;
	int maxCrystalCount = 6;
};

