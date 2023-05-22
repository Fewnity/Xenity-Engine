#pragma once

#include "../../engine/monobehaviour.h"
#include <vector>

class CameraManager;
class MapManager;
class Game;
class Vector2Int;
class BuildingData;

class BuildManager : public MonoBehaviour
{
public:
	BuildManager();
	
	void LoadBuildingsData();
	void OnMouseUp();
	void PlaceBuilding(Vector2Int position);

	Game* game = nullptr;
	CameraManager* cameraManager = nullptr;
	MapManager* mapManager = nullptr;
	std::vector<BuildingData*> buildingsData;

private:
};

