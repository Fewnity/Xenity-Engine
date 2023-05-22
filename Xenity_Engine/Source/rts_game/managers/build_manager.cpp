#include "build_manager.h"
#include "../building_data.h"
#include "camera_manager.h"
#include "../../xenity.h"
#include "../game.h"
#include "map_manager.h"
#include "../building.h"
#include "../building_type_enum.h"

BuildManager::BuildManager()
{
	componentName = "Build Manager";
}

void BuildManager::OnMouseUp()
{
	//Place a building
	if (game->manageMode == ManageBuildings) 
	{
		Vector2 mouseWorldPosition = cameraManager->camera->MouseTo2DWorld();
		PlaceBuilding(Vector2Int(round(mouseWorldPosition.x), round(mouseWorldPosition.y)));
	}
}

/// <summary>
/// Load buildings data
/// </summary>
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

	/*for (int i = 0; i < 3000; i++)
	{
		PlaceBuilding(Vector2Int(5, 0));
	}*/
}

/// <summary>
/// Place a building at position
/// </summary>
/// <param name="position"></param>
void BuildManager::PlaceBuilding(Vector2Int position)
{
	if (mapManager->IsValidPosition(position.x, position.y))
	{
		//Check if the building can be placed
		bool hasProp = mapManager->HasPropAtPosition(position.x, position.y);
		BuildingType type = BuildingType::Miner;
		if (!hasProp && type != BuildingType::Miner || hasProp && type == BuildingType::Miner)
		{
			MapManager::Tile* tile = mapManager->GetTile(position.x, position.y);

			GameObject* buildingGO = new GameObject("Building");
			Building* building = buildingGO->AddComponent<Building>();
			building->buildingData = buildingsData[type];
			building->tile = tile;
			tile->building = building;
			buildingGO->GetTransform()->SetLocalPosition(position);
		}
	}
}