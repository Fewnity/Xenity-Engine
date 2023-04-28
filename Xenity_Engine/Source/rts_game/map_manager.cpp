#include "map_manager.h"
#include "../xenity.h"
#include "../pathfinding/astar.h"
#include "prop.h"
#include "game.h"
#include "unit.h"
#include "unit_manager.h"

MapManager::MapManager()
{
	componentName = "Map Manager";
}

void MapManager::Awake()
{
	//gameObjectTileMap->transform.SetLocalScale(Vector3(2, 1, 1));
//gameObjectTileMap->transform.SetRotation(Vector3(0,0,45));
	gameObjectTileMap->transform.SetPosition(Vector3(0, 0, -1));
}

void MapManager::LoadMapData()
{
	Texture* textureTile0 = new Texture("rts/Tile/scifiTile_42.png", "Ground0");
	textureTile0->SetPixelPerUnit(128);
	Texture* textureTile1 = new Texture("rts/Tile/scifiTile_30.png", "Ground1");
	textureTile1->SetPixelPerUnit(128);

	tilesTextures.push_back(textureTile0);
	tilesTextures.push_back(textureTile1);

	for (int i = 0; i < 19; i++)
	{
		std::string propTextureName = "rts/Environment/scifiEnvironment_" + std::to_string(i + 1) + ".png";
		Texture* textureEnv = new Texture(propTextureName, "", Texture::Bilinear, true);
		propsTextures.push_back(textureEnv);
	}


	int propsTexturesCount = propsTextures.size();
	for (int i = 0; i < propsTexturesCount; i++)
	{
		propsTextures[i]->SetWrapMode(Texture::Clamp);
		propsTextures[i]->SetPixelPerUnit(128);
	}
}

MapManager::Tile* MapManager::GetTile(int x, int y)
{
	if (tiles == nullptr)
		return nullptr;

	//return tiles[x * mapSize + y];
	return &tiles[x][y];
}

bool MapManager::IsValidPosition(int x, int y)
{
	if (x < 0 || y < 0 || x >= mapSize || y >= mapSize)
		return false;

	return true;
}

Vector2Int MapManager::GetTilePosition(MapManager::Tile* tile)
{
	Vector2Int pos = Vector2Int(-1);
	for (int x = 0; x < mapSize; x++)
	{
		for (int y = 0; y < mapSize; y++)
		{
			if (GetTile(x, y) == tile)
			{
				pos.x = x;
				pos.y = y;
				break;
			}
		}
	}
	return pos;
}

void MapManager::GenerateMap()
{
	if (tiles)
	{
		// Libérer la mémoire allouée pour le tableau
		for (int i = 0; i < mapSize; ++i) {
			delete[] tiles[i];
		}
		delete[] tiles;
		//free(tiles);
	}

	tiles = new Tile * [mapSize];
	for (int i = 0; i < mapSize; ++i) { //ymapSize
		tiles[i] = new Tile[mapSize];//ymapSize
	}

	//tiles = (Tile*)malloc(mapSize * mapSize * sizeof(Tile));
	for (int x = 0; x < mapSize; x++)
	{
		for (int y = 0; y < mapSize; y++)
		{
			Tile* tile = GetTile(x, y);
			tile->groundTileId = 1;
			tile->prop = nullptr;
		}
	}

	if (astar)
		delete astar;

	astar = new Astar(mapSize, mapSize);
	astar->canPassCorners = false;
	//Fill map data

	//Add trees
	int treeCount = minTreeCount + rand() % (maxTreeCount - minTreeCount + 1);
	for (int i = 0; i < treeCount; i++)
	{
		int id = 16 + rand() % 4;
		CreateProp(id);
	}

	//Add rocks
	int rockCount = minRockCount + rand() % (maxRockCount - minRockCount + 1);
	for (int i = 0; i < rockCount; i++)
	{
		int id = 0;
		if (rand() % 2 == 0)
			id = 2 + rand() % 4;
		else
			id = 8 + rand() % 4;
		CreateProp(id);
	}

	//Add emeralds
	int emeraldCount = minRockEmeraldCount + rand() % (maxRockEmeraldCount - minRockEmeraldCount + 1);
	for (int i = 0; i < emeraldCount; i++)
	{
		int id = 0;
		if (rand() % 2 == 0)
			id = 6;
		else
			id = 12;
		CreateProp(id);
	}

	//Add gold
	int goldCount = minRockGoldCount + rand() % (maxRockGoldCount - minRockGoldCount + 1);
	for (int i = 0; i < goldCount; i++)
	{
		int id = 0;
		if (rand() % 2 == 0)
			id = 7;
		else
			id = 13;
		CreateProp(id);
	}

	//Add crystal
	int crystalCount = minCrystalCount + rand() % (maxCrystalCount - minCrystalCount + 1);
	for (int i = 0; i < crystalCount; i++)
	{
		int id = 14 + rand() % 2;
		CreateProp(id);
	}

	for (int x = 0; x < mapSize; x++)
	{
		for (int y = 0; y < mapSize; y++)
		{
			Tile* tile = GetTile(x, y);
			if (tile->prop)
			{
				astar->SetTileIsObstacle(x, y, true);
			}
		}
	}
}

/// <summary>
/// Create a new prop in a random position
/// </summary>
/// <param name="id">Prop id</param>
/// <returns>Created prop</returns>
Prop* MapManager::CreateProp(int id)
{
	int x;
	int y;
	do
	{
		x = rand() % mapSize;
		y = rand() % mapSize;
	} while (GetTile(x, y)->prop != nullptr);

	Prop* newProp = new Prop();
	newProp->id = id;

	Tile* tile = GetTile(x, y);
	tile->prop = newProp;

	return newProp;
}

/// <summary>
/// Create tilemaps, 
/// </summary>
void MapManager::CreateTileMaps()
{
	Material* material2DWithZ = AssetManager::GetMaterialByName("2D Standard With Z");

	//Create ground tile map
	tileMap = new TileMap(material2DWithZ);
	gameObjectTileMap->AddExistingComponent(tileMap);
	tileMap->Setup(mapSize, mapSize);
	tileMap->AddTexture(tilesTextures[0]);
	tileMap->AddTexture(tilesTextures[1]);

	//Create props tilemap
	tileMapProps = new TileMap(material2DWithZ);
	tileMapProps->layerOrder = 1;
	gameObjectTileMap->AddExistingComponent(tileMapProps);
	tileMapProps->Setup(mapSize, mapSize);

	int propsTexturesCount = propsTextures.size();
	for (int i = 0; i < propsTexturesCount; i++)
	{
		tileMapProps->AddTexture(propsTextures[i]);
	}

	//Fill tilemaps
	for (int x = 0; x < mapSize; x++)
	{
		for (int y = 0; y < mapSize; y++)
		{
			Tile* tile = GetTile(x, y);
			tileMap->SetTile(x, y, tile->groundTileId);

			//Add prop
			if (tile->prop != nullptr)
			{
				tileMapProps->SetTile(x, y, tile->prop->id);
			}
		}
	}

	tileMap->SetTile(1, 0, 2);
	tileMap->SetTile(4, 2, 2);
	tileMap->SetTile(4, 4, 2);
	tileMap->SetTile(7, 1, 2);
}

void MapManager::Tile::AddUnit(Unit* unit)
{
	units.push_back(unit);
	UpdateUnitsPositions();
}

void MapManager::Tile::RemoveUnit(Unit* unit)
{
	int unitCount = (int)units.size();
	for (int i = 0; i < unitCount; i++)
	{
		if (units[i] == unit)
		{
			units.erase(units.begin() + i);
			break;
		}
	}
	UpdateUnitsPositions();
}

int MapManager::Tile::GetUnitCount()
{
	return units.size();
}

void MapManager::Tile::UpdateUnitsPositions()
{
	Vector2Int pos = Game::GetGame()->mapManager->GetTilePosition(this);
	int unitCount = (int)units.size();
	if (unitCount != 0) 
	{
		UnitPlacement* placement = Game::GetGame()->unitManager->unitPlacements[unitCount-1];
		for (int i = 0; i < unitCount; i++)
		{
			Unit* unit = units[i];
			int pathCount = unit->path.size();

			if(pathCount != 0)
				unit->path.erase(unit->path.end()-1);

			unit->path.push_back(Vector2(pos.x + placement->positions[i].x, pos.y + placement->positions[i].y));
		}
	}
}
