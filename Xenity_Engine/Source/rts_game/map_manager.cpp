#include "map_manager.h"
#include "../xenity.h"
#include "../engine/pathfinding/astar.h"
#include "prop.h"
#include "game.h"
#include "unit.h"
#include "unit_manager.h"
#include "prop_data.h"

#include "../engine/noise/noise.h"

MapManager::MapManager()
{
	componentName = "Map Manager";
}

void MapManager::Awake()
{
	//gameObjectTileMap->transform.SetLocalScale(Vector3(2, 1, 1));
	//gameObjectTileMap->transform.SetRotation(Vector3(0,0,45));
	gameObjectTileMap->transform.SetPosition(Vector3(0, 0, 0));
}

void MapManager::LoadMapData()
{
	Texture* textureTile0 = new Texture("rts/Tile/scifiTile_42.png", "Ground0");
	textureTile0->SetPixelPerUnit(128);
	Texture* textureTile1 = new Texture("rts/Tile/scifiTile_30.png", "Ground1");
	textureTile1->SetPixelPerUnit(128);

	tilesTextures.push_back(textureTile0);
	tilesTextures.push_back(textureTile1);

	PropData* treeData = new PropData();
	treeData->type = Wood;
	//treeData->AddPropData(1, "rts/Environment/scifiEnvironment_16.png");
	//treeData->AddPropData(1, "rts/Environment/scifiEnvironment_17.png");
	treeData->AddPropData(1, "rts/Environment/scifiEnvironment_18.png");
	treeData->AddPropData(1, "rts/Environment/scifiEnvironment_19.png");
	treeData->AddPropData(1, "rts/Environment/scifiEnvironment_21.png");
	treeData->AddPropData(1, "rts/Environment/scifiEnvironment_22.png");
	treeData->AddPropData(1, "rts/Environment/scifiEnvironment_23.png");

	PropData* rockData = new PropData();
	rockData->type = Rock;
	rockData->AddPropData(1, "rts/Environment/scifiEnvironment_2.png");
	rockData->AddPropData(1, "rts/Environment/scifiEnvironment_3.png");
	rockData->AddPropData(1, "rts/Environment/scifiEnvironment_4.png");
	rockData->AddPropData(1, "rts/Environment/scifiEnvironment_5.png");
	/*rockData->AddPropData(1, "rts/Environment/scifiEnvironment_8.png");
	rockData->AddPropData(1, "rts/Environment/scifiEnvironment_9.png");
	rockData->AddPropData(1, "rts/Environment/scifiEnvironment_10.png");
	rockData->AddPropData(1, "rts/Environment/scifiEnvironment_11.png");*/

	PropData* goldData = new PropData();
	goldData->type = Gold;
	goldData->AddPropData(1, "rts/Environment/scifiEnvironment_7.png");
	//goldData->AddPropData(1, "rts/Environment/scifiEnvironment_13.png");

	PropData* crystalData = new PropData();
	crystalData->type = Crystal;
	crystalData->AddPropData(1, "rts/Environment/scifiEnvironment_14.png");
	crystalData->AddPropData(1, "rts/Environment/scifiEnvironment_15.png");

	propsData.push_back(treeData);
	propsData.push_back(rockData);
	propsData.push_back(goldData);
	propsData.push_back(crystalData);
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

bool MapManager::HasPropAtPosition(int x, int y)
{
	return GetTile(x, y)->prop != nullptr;
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
	}

	tiles = new Tile * [mapSize];
	for (int i = 0; i < mapSize; ++i) { //ymapSize
		tiles[i] = new Tile[mapSize];//ymapSize
	}

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
	int treeDataCount = propsData[0]->data.size();
	for (int i = 0; i < treeCount; i++)
	{
		//CreateProp(0, rand() % treeDataCount);
	}

	//Add rocks
	int rockCount = minRockCount + rand() % (maxRockCount - minRockCount + 1);
	int rockDataCount = propsData[1]->data.size();
	for (int i = 0; i < rockCount; i++)
	{
		//CreateProp(1, rand() % rockDataCount);
	}

	//Add gold
	int goldCount = minRockGoldCount + rand() % (maxRockGoldCount - minRockGoldCount + 1);
	int goldDataCount = propsData[2]->data.size();
	for (int i = 0; i < goldCount; i++)
	{
		//CreateProp(2, rand() % goldDataCount);
	}

	//Add crystal
	int crystalCount = minCrystalCount + rand() % (maxCrystalCount - minCrystalCount + 1);
	int crystalDataCount = propsData[3]->data.size();
	for (int i = 0; i < crystalCount; i++)
	{
		//CreateProp(3, rand() % crystalDataCount);
	}

	Vector2 treeOffset1 = Vector2(rand(), rand());
	Vector2 treeOffset2 = Vector2(rand(), rand());
	Vector2 rockOffset = Vector2(rand(), rand());
	Vector2 goldOffset = Vector2(rand(), rand());
	Vector2 crystalOffset = Vector2(rand(), rand());

	float noiseSize = 10;
	float levelOffset = 1.7;

	float noiseSize2 = 18;
	float levelOffset2 = 2;

	float noiseSizeRock = 18;
	float levelOffsetRock = 1;

	float noiseSizeGold = 18;
	float levelOffsetGold = 1;

	float noiseSizeCrystal = 18;
	float levelOffsetCrystal = 1;
	for (int x = 0; x < mapSize; x++)
	{
		for (int y = 0; y < mapSize; y++)
		{
			float forest1Noise = Noise::noise2(x / (float)mapSize * noiseSize, y / (float)mapSize * noiseSize);
			float forest2Noise = Noise::noise2(x / (float)mapSize * noiseSize2, y / (float)mapSize * noiseSize2);
			float rockNoise = Noise::noise2(x / (float)mapSize * noiseSizeCrystal + rockOffset.x, y / (float)mapSize * noiseSizeCrystal + rockOffset.y);
			float goldNoise = Noise::noise2(x / (float)mapSize * noiseSizeCrystal + goldOffset.x, y / (float)mapSize * noiseSizeCrystal + goldOffset.y);
			float crystalNoise = Noise::noise2(x / (float)mapSize * noiseSizeCrystal + crystalOffset.x, y / (float)mapSize * noiseSizeCrystal + crystalOffset.y);
			if (forest1Noise >= 0.7)
			{
				Prop* newProp = new Prop();
				newProp->data = propsData[0];
				int dataSize = propsData[0]->data.size();
				Tile* tile = GetTile(x, y);
				tile->prop = newProp;
				tile->prop->id = (int)((forest1Noise - 0.7f) / 0.3f * dataSize * levelOffset);
				if (tile->prop->id >= dataSize)
				{
					tile->prop->id = dataSize - 1;
				}
				else if (tile->prop->id < 0)
				{
					tile->prop->id = 0;
				}
			}
			else if (forest2Noise >= 0.75)
			{
					Prop* newProp = new Prop();
					newProp->data = propsData[0];
					int dataSize = propsData[0]->data.size();
					Tile* tile = GetTile(x, y);
					tile->prop = newProp;
					tile->prop->id = (int)((forest2Noise - 0.75f) / 0.25f * dataSize * levelOffset2);
					if (tile->prop->id >= dataSize)
					{
						tile->prop->id = dataSize - 1;
					}
					else if (tile->prop->id < 0)
					{
						tile->prop->id = 0;
					}
			}
			else if (rockNoise >= 0.8)
			{
				Prop* newProp = new Prop();
				newProp->data = propsData[1];
				int dataSize = propsData[1]->data.size();
				Tile* tile = GetTile(x, y);
				tile->prop = newProp;
				tile->prop->id = (int)((rockNoise - 0.8f) / 0.2f * dataSize * levelOffsetRock);
				if (tile->prop->id >= dataSize)
				{
					tile->prop->id = dataSize - 1;
				}
				else if (tile->prop->id < 0)
				{
					tile->prop->id = 0;
				}
			}
			else if (goldNoise >= 0.8)
			{
				Prop* newProp = new Prop();
				newProp->data = propsData[2];
				int dataSize = propsData[2]->data.size();
				Tile* tile = GetTile(x, y);
				tile->prop = newProp;
				tile->prop->id = (int)((goldNoise - 0.8f) / 0.2f * dataSize * levelOffsetGold);
				if (tile->prop->id >= dataSize)
				{
					tile->prop->id = dataSize - 1;
				}
				else if (tile->prop->id < 0)
				{
					tile->prop->id = 0;
				}
			}
			else if (crystalNoise >= 0.85)
			{
				Prop* newProp = new Prop();
				newProp->data = propsData[3];
				int dataSize = propsData[3]->data.size();
				Tile* tile = GetTile(x, y);
				tile->prop = newProp;
				tile->prop->id = (int)((crystalNoise - 0.85f) / 0.15f * dataSize * levelOffsetCrystal);
				if (tile->prop->id >= dataSize)
				{
					tile->prop->id = dataSize - 1;
				}
				else if (tile->prop->id < 0)
				{
					tile->prop->id = 0;
				}
			}
		}
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
/// Create a new prop in a random position
/// </summary>
/// <param name="id">Prop id</param>
/// <returns>Created prop</returns>
Prop* MapManager::CreateProp(int propType, int index)
{
	int x;
	int y;
	do
	{
		x = rand() % mapSize;
		y = rand() % mapSize;
	} while (GetTile(x, y)->prop != nullptr);

	Prop* newProp = new Prop();
	newProp->data = propsData[propType];
	newProp->id = index;

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
	tileMap = gameObjectTileMap->AddComponent<Tilemap>();
	tileMap->material = material2DWithZ;

	tileMap->Setup(mapSize, mapSize);
	tileMap->AddTexture(tilesTextures[0]);
	tileMap->AddTexture(tilesTextures[1]);

	//Create props tilemap
	tileMapProps = gameObjectTileMap->AddComponent<Tilemap>();
	tileMapProps->material = material2DWithZ;
	tileMapProps->SetOrderInLayer(1);
	tileMapProps->Setup(mapSize, mapSize);

	int propDataSize = propsData.size();
	for (int i = 0; i < propDataSize; i++)
	{
		int count = propsData[i]->data.size();
		for (int j = 0; j < count; j++)
		{
			tileMapProps->AddTexture(propsData[i]->data[j]->texture);
		}
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
				tileMapProps->SetTile(x, y, tile->prop->data->data[tile->prop->id]->texture);
			}
		}
	}

	/*tileMap->SetTile(1, 0, 2);
	tileMap->SetTile(4, 2, 2);
	tileMap->SetTile(4, 4, 2);
	tileMap->SetTile(7, 1, 2);*/
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
		UnitPlacement* placement = Game::GetGame()->unitManager->unitPlacements[unitCount - 1];
		for (int i = 0; i < unitCount; i++)
		{
			Unit* unit = units[i];
			int pathCount = unit->path.size();

			if (pathCount != 0)
				unit->path.erase(unit->path.end() - 1);

			unit->path.push_back(Vector2(pos.x + placement->positions[i].x, pos.y + placement->positions[i].y));
		}
	}
}
