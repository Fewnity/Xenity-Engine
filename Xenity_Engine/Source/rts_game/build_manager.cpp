#include "build_manager.h"
#include "building_data.h"
#include "camera_manager.h"
#include "../xenity.h"
#include "game.h"
#include "map_manager.h"
#include "building.h"

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
}


void BuildManager::PlaceBuilding(Vector2Int position)
{
	GameObject * buildingGO = new GameObject("Building");
	Building *building = new Building(buildingsData[0], this);
	buildingGO->AddExistingComponent(building);
	buildingGO->transform.SetLocalPosition(position);
	mapManager->GetTile(position.x, position.y)->building = building;
}