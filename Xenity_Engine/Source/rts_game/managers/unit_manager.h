#pragma once

#include "../../engine/monobehaviour.h"
#include <vector>
#include "../team_enum.h"

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
	
	void Awake();
	void SpawnUnit(Vector2Int position, TeamColor color, int unitId);
	void SpawnUnits();
	void UnselectAllUnits();
	void LoadUnitsData();
	void SelectUnits();
	void OnMouseUp();

	std::vector<UnitData*> unitsData;
	std::vector<UnitPlacement*> unitPlacements;
	CameraManager* cameraManager = nullptr;
	Game* game = nullptr;
	int maxUnitPerTile = 5;
	int selectedUnitData = 9;

private:
};

