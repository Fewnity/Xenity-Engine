#pragma once

#include "../monobehaviour.h"

class Unit;
class UnitData;
class CameraManager;
class Game;

class UnitManager : public MonoBehaviour
{
public:
	UnitManager();
	std::vector<UnitData*> unitsData;
	std::vector<Unit*> units;
	void SpawnUnits();
	void UnselectAllUnits();
	void LoadUnitData();
	void SelectUnits();
	CameraManager* cameraManager = nullptr;
	Game* game = nullptr;
	void OnMouseUp();

private:
};

