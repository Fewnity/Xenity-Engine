#pragma once

#include "../monobehaviour.h"
#include <vector>

class Unit;
class UnitData;
class CameraManager;
class Game;
class Vector2;

class UnitPlacement 
{
public:
	std::vector<Vector2> positions;
};

class UnitManager : public MonoBehaviour
{
public:
	UnitManager();
	std::vector<UnitData*> unitsData;
	std::vector<Unit*> units;
	std::vector<UnitPlacement*> unitPlacements;
	
	void SpawnUnits();
	void UnselectAllUnits();
	void LoadUnitData();
	void SelectUnits();
	CameraManager* cameraManager = nullptr;
	Game* game = nullptr;
	void OnMouseUp();
	int maxUnitPerTile = 5;

private:
};

