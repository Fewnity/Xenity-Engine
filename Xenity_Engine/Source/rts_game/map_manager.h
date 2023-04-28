#pragma once

#include <vector>
#include "../monobehaviour.h"

class Prop;
class Unit;
class Astar;
class Texture;
class GameObject;
class TileMap;
class Vector2Int;

class MapManager : public MonoBehaviour
{
public:
	MapManager();
	class Tile {
	public:
		int groundTileId = 0; // 0 is no tile
		Prop* prop = nullptr;
		void AddUnit(Unit* unit);
		void RemoveUnit(Unit* unit);
		int GetUnitCount();

	private:
		void UpdateUnitsPositions();
		std::vector<Unit*> units;
	};
	void Awake();
	Tile** tiles = nullptr;
	TileMap* tileMap = nullptr;
	TileMap* tileMapProps = nullptr;

	void LoadMapData();
	MapManager::Tile* GetTile(int x, int y);
	Vector2Int GetTilePosition(MapManager::Tile* tile);
	bool IsValidPosition(int x, int y);
	void GenerateMap();
	void CreateTileMaps();
	Prop* CreateProp(int id);
	GameObject* gameObjectTileMap = new GameObject("TileMap");
	Astar* astar = nullptr;

private:
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
	std::vector<Texture*> propsTextures;
	std::vector<Texture*> tilesTextures;
};

