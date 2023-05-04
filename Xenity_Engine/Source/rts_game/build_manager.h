#pragma once

#include "../engine/monobehaviour.h"
#include <vector>

class Unit;
class UnitData;
class CameraManager;
class MapManager;
class Game;
class Vector2;
class Vector2Int;
class BuildingData;

class BuildManager : public MonoBehaviour
{
public:
	BuildManager();
	std::vector<BuildingData*> buildingsData;
	
	void LoadBuildingsData();
	CameraManager* cameraManager = nullptr;
	MapManager* mapManager = nullptr;
	Game* game = nullptr;
	void OnMouseUp();
	void PlaceBuilding(Vector2Int position);

private:
};

