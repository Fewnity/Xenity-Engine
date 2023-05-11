#include "build_manager.h"
#include "building_data.h"
#include "camera_manager.h"
#include "../xenity.h"
#include "game.h"
#include "map_manager.h"
#include "building.h"
#include "building_type.h"

BuildManager::BuildManager()
{
}

void BuildManager::OnMouseUp()
{
	Vector2 mouseWorldPosition = cameraManager->camera->MouseTo2DWorld();
	PlaceBuilding(Vector2Int(round(mouseWorldPosition.x), round(mouseWorldPosition.y)));
}

void BuildManager::LoadBuildingsData()
{
	for (int i = 0; i < 16; i++)
	{
		BuildingData* newBuildingData = new BuildingData(i, game->crosshair);
		buildingsData.push_back(newBuildingData);
	}
	buildingsData[BuildingType::Telsa]->type = BuildingType::Telsa;
	buildingsData[BuildingType::Generator]->type = BuildingType::Generator;
	buildingsData[BuildingType::Base]->type = BuildingType::Base;
	buildingsData[BuildingType::Miner]->type = BuildingType::Miner;
	buildingsData[BuildingType::Storage]->type = BuildingType::Storage;

	for (int i = 0; i < 3000; i++)
	{
		PlaceBuilding(Vector2Int(5, 0));
	}
}

void BuildManager::PlaceBuilding(Vector2Int position)
{
	if (mapManager->IsValidPosition(position.x, position.y))
	{
		BuildingType type = BuildingType::Miner;
		bool hasProp = mapManager->HasPropAtPosition(position.x, position.y);

		if (!hasProp && type != BuildingType::Miner || hasProp && type == BuildingType::Miner)
		{
			MapManager::Tile* tile = mapManager->GetTile(position.x, position.y);

			GameObject* buildingGO = new GameObject("Building");
			Building* building = new Building(buildingsData[type], this);
			building->tile = tile;
			buildingGO->AddExistingComponent(building);
			buildingGO->transform.SetLocalPosition(position);
			tile->building = building;
		}
	}
}