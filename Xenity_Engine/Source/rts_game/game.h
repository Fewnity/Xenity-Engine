#pragma once

#include "../graphics/tile_map.h"
#include "../graphics/camera.h"

class Prop {
public:
	int id = 0;
	int x = 0;
	int y = 0;

	void SetPosition(int x, int y) 
	{
		this->x = x;
		this->y = y;
	}
};

class Unit;
class UnitData;

class Game
{
public:
class Tile {
public:
	int groundTileId = 0; // 0 is no tile
	Prop* prop = nullptr;
};
	void Init();
	void Loop();
	Game::Tile* GetTile(int x, int y);

private:
	void GenerateMap();
	Prop* CreateProp(int id);
	void LoadGameData();
	void CreateTileMaps();

	GameObject* cameraGameObject = new GameObject("cameraGameObject");

	Camera* camera = new Camera();

	GameObject* gameObjectSprite = new GameObject("gameObjectSprite");
	GameObject* gameObjectTileMap = new GameObject("TileMap");
	GameObject* gameObjectCrosshair = new GameObject("Crosshair");

	TileMap* tileMap = nullptr;
	TileMap* tileMapProps = nullptr;
	float cameraZoom = 1.5f; //[1;2.8]
	float cameraArrowMoveSpeed = 3;

	Vector2 cursorPosition = Vector2(0, 0);

	Tile* tiles = nullptr;
	std::vector<UnitData*> unitsData;
	std::vector<Unit*> units;

	//Map settings
	int mapSize = 200;
	int minTreeCount = 1500;
	int maxTreeCount = 2000;
	int minRockCount = 800;
	int maxRockCount = 1200;
	int minRockEmeraldCount = 500;
	int maxRockEmeraldCount = 600;
	int minRockGoldCount = 600;
	int maxRockGoldCount = 800;
	int minCrystalCount = 400;
	int maxCrystalCount = 400;

	//Textures
	std::vector<Texture*> propsTextures;
	std::vector<Texture*> tilesTextures;
	//std::vector<Texture*> unitsTextures;
	Texture* textureShip = nullptr;
	Texture* crosshair = nullptr;

	//Shaders

	//Materials
	Material* material2D = nullptr;
};

