#pragma once

#include "../engine/monobehaviour.h"
#include "../engine/vectors/vector2.h"

class SpriteRenderer;
class UnitData;

class Bullet : public MonoBehaviour
{
public:
	Bullet();

	void Start();
	void Update();

	SpriteRenderer* spriteRenderer = nullptr;
	Vector2 direction = Vector2(0, 0);
	UnitData* unitData = nullptr;
};

