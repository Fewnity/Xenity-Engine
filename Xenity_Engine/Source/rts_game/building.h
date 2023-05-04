#pragma once

#include "../engine/monobehaviour.h"

#include <vector>

#include "build_manager.h"

class Vector2;
class Vector2Int;

class BuildingData;
class SpriteRenderer;

class Building : public MonoBehaviour
{
public:
	Building() = delete;
	Building(BuildingData *data, BuildManager * buildManager);

	BuildingData* buildingData = nullptr;

	void Start();
	void Update();

	bool selected = false;
	SpriteRenderer* selectionSpriteRenderer = nullptr;
	std::vector<Vector2> path;

	int currentPathNode = 0;
	float movementSpeed = 1;
};

