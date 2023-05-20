#pragma once

#include "../engine/monobehaviour.h"

#include <vector>

#include "managers/map_manager.h"
#include "team_enum.h"

class Vector2;
class Vector2Int;

class UnitData;
class SpriteRenderer;

class Unit : public MonoBehaviour
{
public:
	Unit();
	//Unit(UnitData *data, MapManager * mapManager);

	UnitData* unitData = nullptr;

	void Start();
	void Update();
	void SetDestination(Vector2Int position);
	void UpdateLifeBar();
	void Shoot();

	bool selected = false;
	SpriteRenderer* selectionSpriteRenderer = nullptr;
	GameObject* lifeBarGO = nullptr;
	SpriteRenderer* lifeBarSprRenderer = nullptr;
	GameObject* gmUnitSprite = nullptr;

	Unit* target = nullptr;
	std::vector<Vector2> path;
	MapManager::Tile* destinationTile = nullptr;
	MapManager* mapManager = nullptr;
	TeamColor color = Blue;

	int currentPathNode = 0;
	float currentHealth = 0;
};

