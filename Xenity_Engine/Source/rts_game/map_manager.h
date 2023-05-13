#pragma once

#include <vector>
#include "../engine/monobehaviour.h"

class Prop;
class Unit;
class Astar;
class Texture;
class GameObject;
class Tilemap;
class Vector2Int;
class Building;
class PropData;

class MapManager : public MonoBehaviour
{
public:
	class Tile 
	{
	public:
		int groundTileId = 0; // 0 is no tile
		Prop* prop = nullptr;
		Building* building = nullptr;
		void AddUnit(Unit* unit);
		void RemoveUnit(Unit* unit);
		int GetUnitCount();

	private:
		void UpdateUnitsPositions();
		std::vector<Unit*> units;
	};

	MapManager();
	void Awake();
	Tile** tiles = nullptr;
	Tilemap* tileMap = nullptr;
	Tilemap* tileMapProps = nullptr;

	void LoadMapData();
	MapManager::Tile* GetTile(int x, int y);
	Vector2Int GetTilePosition(MapManager::Tile* tile);
	bool HasPropAtPosition(int x, int y);
	bool IsValidPosition(int x, int y);
	void GenerateMap();
	void CreateTileMaps();
	Prop* CreateProp(int id);
	Prop* CreateProp(int propType, int index);
	GameObject* gameObjectTileMap = new GameObject("TileMap");
	Astar* astar = nullptr;
	std::vector<PropData*> propsData;

private:
	//Map settings
	int mapSize = 60;
	int minTreeCount = 400;
	int maxTreeCount = 400;
	int minRockCount = 100;
	int maxRockCount = 100;
	int minRockGoldCount = 50;
	int maxRockGoldCount = 50;
	int minCrystalCount = 30;
	int maxCrystalCount = 30;
	std::vector<Texture*> tilesTextures;
};

