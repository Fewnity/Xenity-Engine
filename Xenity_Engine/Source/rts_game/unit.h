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

	UnitData* unitData = nullptr;

	void Start();
	void Update();
	void SetDestination(Vector2Int position);
	void UpdateLifeBar();
	void Shoot();

	MapManager* mapManager = nullptr;

	bool selected = false;
	SpriteRenderer* selectionSpriteRenderer = nullptr;
	GameObject* lifeBarGO = nullptr;
	SpriteRenderer* lifeBarSprRenderer = nullptr;
	GameObject* gmUnitSprite = nullptr;

	TeamColor color = Blue;
	float currentHealth = 0;

	// Shooting
	Unit* target = nullptr;

	// Pathfinding
	int currentPathNode = 0;
	std::vector<Vector2> path;
	MapManager::Tile* destinationTile = nullptr;
};

