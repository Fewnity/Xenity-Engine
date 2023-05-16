#pragma once

#include "../engine/monobehaviour.h"
#include <vector>
#include "team_enum.h"

class Unit;
class UnitData;
class CameraManager;
class Game;
class Vector2;
class Vector2Int;

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
	
	void SpawnUnit(Vector2Int position, TeamColor color, int unitId);
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

