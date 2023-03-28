#include "Unit.h"
#include <iostream>
#include "../graphics/spriteRenderer.h"
#include "../rts_game/unit_data.h"
#include "../graphics/texture.h"
#include "../graphics/material.h"

Unit::Unit(UnitData* data)
{
	unitData = data;
}

void Unit::Start()
{
	GameObject* gmUnitSprite = new GameObject();
	SpriteRenderer* unitSpriteRenderer = new SpriteRenderer(unitData->textures[0], AssetManager::GetMaterialByName("2D Standard"));
	gmUnitSprite->AddExistingComponent(unitSpriteRenderer);
	gameObject->AddChild(gmUnitSprite);
	gmUnitSprite->transform.SetLocalPosition(Vector3(0, 0, 0));
}

void Unit::Update()
{
}