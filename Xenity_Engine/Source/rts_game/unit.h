#pragma once

#include "../monobehaviour.h"

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
};

