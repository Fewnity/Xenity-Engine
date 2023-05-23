#include "map_manager.h"
#include "../../xenity.h"
#include "../../engine/pathfinding/astar.h"
#include "../prop.h"
#include "../game.h"
#include "../unit.h"
#include "unit_manager.h"
#include "../prop_data.h"

#include "../../engine/noise/noise.h"

MapManager::MapManager()
{
	componentName = "Map Manager";
}

void MapManager::Awake()
{
	//gameObjectTileMap->GetTransform()->SetLocalScale(Vector3(2, 1, 1));
	//gameObjectTileMap->GetTransform()->SetRotation(Vector3(0,0,45));
	gameObjectTileMap->GetTransform()->SetPosition(Vector3(0, 0, 0));
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

/// <summary>
/// Get tile
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <returns></returns>
MapManager::Tile* MapManager::GetTile(int x, int y)
{
	if (tiles == nullptr)
		return nullptr;

	return &tiles[x][y];
}

/// <summary>
/// Check if the position is out of the map
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <returns></returns>
bool MapManager::IsValidPosition(int x, int y)
{
	if (x >= mapSize || y >= mapSize || x < 0 || y < 0)
		return false;

	return true;
}

/// <summary>
/// Check if a tile has a prop at position
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <returns></returns>
bool MapManager::HasPropAtPosition(int x, int y)
{
	return GetTile(x, y)->prop != nullptr;
}

/// <summary>
/// Get the position of a tile
/// </summary>
/// <param name="tile"></param>
/// <returns></returns>
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

void MapManager::PlaceProp(int dataIndex, Vector2Int position, float noise, float noiseStartLevel, float noiseMultiplier) 
{
	Prop* newProp = new Prop();
	newProp->data = propsData[dataIndex];
	int dataSize = propsData[dataIndex]->data.size();
	Tile* tile = GetTile(position.x, position.y);
	tile->prop = newProp;
	tile->prop->id = (int)((noise - noiseStartLevel) / (1- noiseStartLevel) * dataSize * noiseMultiplier);
	if (tile->prop->id >= dataSize)
	{
		tile->prop->id = dataSize - 1;
	}
	else if (tile->prop->id < 0)
	{
		tile->prop->id = 0;
	}
}

/// <summary>
/// Generate map
/// </summary>
void MapManager::GenerateMap()
{
	//Empty tiles if tiles already exist
	if (tiles)
	{
		for (int i = 0; i < mapSize; ++i)
		{
			delete[] tiles[i];
		}
		delete[] tiles;
	}

	//Create tiles in memory
	tiles = new Tile * [mapSize];
	for (int i = 0; i < mapSize; ++i)
	{
		tiles[i] = new Tile[mapSize];
	}

	//Fill tiles
	for (int x = 0; x < mapSize; x++)
	{
		for (int y = 0; y < mapSize; y++)
		{
			Tile* tile = GetTile(x, y);
			tile->groundTileId = 1;
			tile->prop = nullptr;
		}
	}

	//Delete astar if astar already exists
	if (astar)
		delete astar;

	//Create astar
	astar = new Astar(mapSize, mapSize);
	astar->canPassCorners = false;

	//Fill map data

	//Add trees
	/*int treeCount = minTreeCount + rand() % (maxTreeCount - minTreeCount + 1);
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
	}*/

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
			if (forest1Noise >= 0.7f) //Place Big forest
			{
				PlaceProp(0, Vector2Int(x, y), forest1Noise, 0.7f, levelOffset);
			}
			else
			{
				float forest2Noise = Noise::noise2(x / (float)mapSize * noiseSize2, y / (float)mapSize * noiseSize2);
				if (forest2Noise >= 0.75f) //Place little forest
				{
					PlaceProp(0, Vector2Int(x, y), forest2Noise, 0.75f, levelOffset2);
				}
				else
				{
					float rockNoise = Noise::noise2(x / (float)mapSize * noiseSizeRock + rockOffset.x, y / (float)mapSize * noiseSizeRock + rockOffset.y);
					if (rockNoise >= 0.8f) //Place rocks
					{
						PlaceProp(1, Vector2Int(x, y), rockNoise, 0.8f, levelOffsetRock);
					}
					else
					{
						float goldNoise = Noise::noise2(x / (float)mapSize * noiseSizeGold + goldOffset.x, y / (float)mapSize * noiseSizeGold + goldOffset.y);
						if (goldNoise >= 0.8f) //Place gold
						{
							PlaceProp(2, Vector2Int(x, y), goldNoise, 0.8f, levelOffsetGold);
						}
						else
						{
							float crystalNoise = Noise::noise2(x / (float)mapSize * noiseSizeCrystal + crystalOffset.x, y / (float)mapSize * noiseSizeCrystal + crystalOffset.y);
							if (crystalNoise >= 0.85f) //Place crystal
							{
								PlaceProp(3, Vector2Int(x, y), crystalNoise, 0.85f, levelOffsetCrystal);
							}
						}
					}
				}
			}
		}
	}


	//Place obstacles in astar
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
}

/// <summary>
/// Add unit in a tile
/// </summary>
/// <param name="unit"></param>
void MapManager::Tile::AddUnit(Unit* unit)
{
	units.push_back(unit);
	UpdateUnitsPositions();
}

/// <summary>
/// Remove unit from the tile
/// </summary>
/// <param name="unit"></param>
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

/// <summary>
/// Update units position in the tile
/// </summary>
void MapManager::Tile::UpdateUnitsPositions()
{
	int unitCount = (int)units.size();
	if (unitCount != 0)
	{
		Vector2Int pos = Game::GetGame()->mapManager->GetTilePosition(this);
		UnitPlacement* placement = Game::GetGame()->unitManager->unitPlacements[unitCount - 1];
		for (int i = 0; i < unitCount; i++)
		{
			Unit* unit = units[i];
			int pathCount = unit->path.size();

			//Remove old last path node (old tile)
			if (pathCount != 0)
				unit->path.erase(unit->path.end() - 1);

			unit->path.push_back(Vector2(pos.x + placement->positions[i].x, pos.y + placement->positions[i].y));
		}
	}
}
