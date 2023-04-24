#pragma once

#include "../monobehaviour.h"

#include <vector>

class Vector2;

class UnitData;
class SpriteRenderer;

class Unit : public MonoBehaviour
{
public:
	Unit() = delete;
	Unit(UnitData *data);

	UnitData* unitData = nullptr;

	void Start();
	void Update();
	bool selected = false;
	SpriteRenderer* selectionSpriteRenderer = nullptr;
	std::vector<Vector2> path;
	int currentPathNode = 0;
};

