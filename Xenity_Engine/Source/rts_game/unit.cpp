#include "Unit.h"
#include <iostream>
#include "../rts_game/unit_data.h"
#include "../xenity.h"
#include "../engine/pathfinding/astar.h"

Unit::Unit()
{
	reflectedFloats.insert(std::pair<std::string, float*>("Movement Speed", &movementSpeed));
	componentName = "Unit";
}

void Unit::Start()
{
	GameObject* gmUnitSprite = new GameObject();

	SpriteRenderer* unitSpriteRenderer = gmUnitSprite->AddComponent<SpriteRenderer>();
	unitSpriteRenderer->texture = unitData->textures[color];
	unitSpriteRenderer->material = AssetManager::GetMaterialByName("2D Standard");
	unitSpriteRenderer->SetOrderInLayer(3);

	selectionSpriteRenderer = gmUnitSprite->AddComponent<SpriteRenderer>();
	selectionSpriteRenderer->texture = unitData->selectionTexture;
	selectionSpriteRenderer->material = AssetManager::GetMaterialByName("2D Standard");
	selectionSpriteRenderer->SetOrderInLayer(11);

	GetGameObject()->AddChild(gmUnitSprite);
	gmUnitSprite->transform.SetLocalPosition(Vector3(0, 0, 0.0f));
	gmUnitSprite->transform.SetLocalScale(0.5f);
}

void Unit::Update()
{
	selectionSpriteRenderer->SetIsEnabled(selected);
	int pathSize = path.size();
	if (pathSize != 0)
	{
		Vector3 newPos = GetGameObject()->transform.GetPosition();
		Vector2 dir = (path[currentPathNode] - Vector2(GetGameObject()->transform.GetPosition().x, GetGameObject()->transform.GetPosition().y)).normalize();
		newPos.x += dir.x * Time::GetDeltaTime() * movementSpeed;
		newPos.y += dir.y * Time::GetDeltaTime() * movementSpeed;

		if (unitData->rotateWhenMoving)
		{
			float angle = atan2(dir.x, dir.y);
			GetGameObject()->transform.SetRotation(Vector3(0, 0, angle * 180 / M_PI - 90));

			if (dir.x < 0)
			{
				GetGameObject()->transform.SetLocalScale(Vector3(1, -1, 1));
			}
			else
			{
				GetGameObject()->transform.SetLocalScale(Vector3(1, 1, 1));
			}
		}

		GetGameObject()->transform.SetPosition(newPos);

		if (Vector2::Distance(Vector2(GetGameObject()->transform.GetPosition().x, GetGameObject()->transform.GetPosition().y), path[currentPathNode]) <= 0.02f)
		{
			currentPathNode++;
			if (currentPathNode == pathSize)
			{
				currentPathNode = 0;
				path.clear();
			}
		}
	}
	else
	{
		if (unitData->rotateWhenMoving)
		{
			if (GetGameObject()->transform.GetScale().y < 0)
				GetGameObject()->transform.SetRotation(Vector3(0, 0, 180));
			else
				GetGameObject()->transform.SetRotation(Vector3(0, 0, 0));
		}
	}
}

void Unit::SetDestination(Vector2Int position)
{
	mapManager->astar->SetDestination(Vector2(round(GetGameObject()->transform.GetPosition().x), round(GetGameObject()->transform.GetPosition().y)), Vector2(position.x, position.y));
	path = mapManager->astar->GetPath();
	int pathCount = path.size();
	if (pathCount > 1)
		currentPathNode = 1;
	else
		currentPathNode = 0;

	MapManager::Tile* newTile = mapManager->GetTile(position.x, position.y);
	if (newTile != destinationTile)
	{
		if (destinationTile)
		{
			destinationTile->RemoveUnit(this);
		}
		if (path.size() != 0)
		{
			destinationTile = newTile;
			destinationTile->AddUnit(this);
		}
	}
}
