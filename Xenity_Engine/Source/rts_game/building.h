#pragma once

#include "../engine/monobehaviour.h"
#include "managers/build_manager.h"
#include "managers/map_manager.h"
#include <vector>

class Vector2;
class Vector2Int;
class BuildingData;
class SpriteRenderer;

class Building : public MonoBehaviour
{
public:
	Building();

	void Start();
	void Update();

	BuildingData* buildingData = nullptr;

	bool selected = false;
	SpriteRenderer* selectionSpriteRenderer = nullptr;
	MapManager::Tile* tile = nullptr;
	float miningTimer = 0;
};

