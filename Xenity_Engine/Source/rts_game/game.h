#pragma once

#include "../graphics/tile_map.h"
#include "../graphics/camera.h"
#include "../graphics/iDrawable.h"

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
class LineRenderer;
class Astar;
class CameraManager;
class UnitManager;

class Game
{
public:
class Tile {
public:
	int groundTileId = 0; // 0 is no tile
	Prop* prop = nullptr;
	std::vector<Unit*> units;
};

	void Init();
	void Loop();
	Game::Tile* GetTile(int x, int y);
	Vector2 startSelectionPos = Vector2(0, 0);
	bool isDragging = false;
	LineRenderer* lineRendererTop = nullptr;
	LineRenderer* lineRendererBottom = nullptr;
	LineRenderer* lineRendererLeft = nullptr;
	LineRenderer* lineRendererRight = nullptr;
	void SetSelection(bool isSelecting);
	bool isPointInsideAABB(Vector2 point, Vector2 aMin, Vector2 aMax);
	bool intersect(Vector2 aMin, Vector2 aMax, Vector2 bMin, Vector2 bMax);
	Texture* crosshair = nullptr;
	Astar* astar = nullptr;

private:
	int GetDrawPriority();

	void GenerateMap();
	Prop* CreateProp(int id);
	void LoadGameData();
	void CreateTileMaps();
	void MoveCursor();
	void OnMouseUp();


	GameObject* gameObjectSprite = new GameObject("gameObjectSprite");
	GameObject* gameObjectTileMap = new GameObject("TileMap");
	GameObject* gameObjectCrosshair = new GameObject("Crosshair");
	GameObject* gameObjectCrosshair2 = new GameObject("Crosshair2");

	GameObject* gameObjectLineRenderers = new GameObject("lineRenderers");


	CameraManager* cameraManager = nullptr;;
	UnitManager* unitManager = nullptr;;

	TileMap* tileMap = nullptr;
	TileMap* tileMapProps = nullptr;

	Vector2 cursorPosition = Vector2(0, 0);

	Tile* tiles = nullptr;


	Vector2 startMousePosition = Vector2(0, 0);

	Vector4 selectionColor = Vector4(1, 1, 1, 1);

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
	Texture* textureShip = nullptr;
	Texture* gradient = nullptr;

	//Shaders

	//Materials
	Material* material2D = nullptr;
	Material* material2DWithZ = nullptr;
};

