#include "Unit.h"
#include <iostream>
#include "../rts_game/unit_data.h"
#include "../xenity.h"

Unit::Unit(UnitData* data)
{
	unitData = data;
}

void Unit::Start()
{
	GameObject* gmUnitSprite = new GameObject();
	SpriteRenderer* unitSpriteRenderer = new SpriteRenderer(unitData->textures[0], AssetManager::GetMaterialByName("2D Standard"));
	gmUnitSprite->AddExistingComponent(unitSpriteRenderer);
	selectionSpriteRenderer = new SpriteRenderer(unitData->selectionTexture, AssetManager::GetMaterialByName("2D Standard"));
	gmUnitSprite->AddExistingComponent(selectionSpriteRenderer);
	gameObject->AddChild(gmUnitSprite);
	gmUnitSprite->transform.SetLocalPosition(Vector3(0, 0, 0));
	gmUnitSprite->transform.SetLocalScale(0.5f);
}

void Unit::Update()
{
	selectionSpriteRenderer->SetIsEnabled(selected);

	int pathSize = path.size();
	if (pathSize != 0) 
	{
		Vector3 newPos = gameObject->transform.GetPosition();
		Vector2 dir = (path[currentPathNode] - Vector2(gameObject->transform.GetPosition().x, gameObject->transform.GetPosition().y)).normalize();
		newPos.x += dir.x * Time::GetDeltaTime();
		newPos.y += dir.y * Time::GetDeltaTime();
		gameObject->transform.SetPosition(newPos);

		if (Vector2::Distance(Vector2(gameObject->transform.GetPosition().x, gameObject->transform.GetPosition().y), path[currentPathNode]) <= 0.02f)
		{
			currentPathNode++;
			if (currentPathNode == pathSize)
			{
				currentPathNode = 0;
				path.clear();
			}
		}
	}
}