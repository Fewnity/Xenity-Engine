#include "Unit.h"
#include <iostream>
#include "../rts_game/unit_data.h"
#include "../xenity.h"
#include "../engine/pathfinding/astar.h"

Unit::Unit(UnitData* data, MapManager* mapManager)
{
	reflectedFloats.insert(std::pair<std::string, float*>("Movement Speed", &movementSpeed));
	componentName = "Unit";

	unitData = data;
	this->mapManager = mapManager;
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
		newPos.x += dir.x * Time::GetDeltaTime() * movementSpeed;
		newPos.y += dir.y * Time::GetDeltaTime() * movementSpeed;
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

void Unit::SetDestination(Vector2Int position)
{
	mapManager->astar->SetDestination(Vector2(round(gameObject->transform.GetPosition().x), round(gameObject->transform.GetPosition().y)), Vector2(position.x, position.y));
	path = mapManager->astar->GetPath();
	int pathCount = path.size();
	if(pathCount > 1)
		currentPathNode = 1;
	else
		currentPathNode = 0;

	if (destinationTile) 
	{
		destinationTile->RemoveUnit(this);
	}
	if (path.size() != 0) 
	{
		destinationTile = mapManager->GetTile(position.x, position.y);
		destinationTile->AddUnit(this);
	}
}
