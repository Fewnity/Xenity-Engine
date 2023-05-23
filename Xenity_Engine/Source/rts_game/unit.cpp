#include "Unit.h"
#include <iostream>
#include "../rts_game/unit_data.h"
#include "../xenity.h"
#include "../engine/pathfinding/astar.h"
#include "game.h"
#include "managers/team_manager.h"
#include "bullet.h"

Unit::Unit()
{
	// Temporary code
	reflectedFloats.insert(std::pair<std::string, float*>("Current Health", &currentHealth));
	componentName = "Unit";
}

void Unit::Start()
{
	gmUnitSprite = new GameObject("Unit Sprite");
	GetGameObject()->AddChild(gmUnitSprite);
	gmUnitSprite->GetTransform()->SetLocalPosition(Vector3(0, 0, 0));

	// Create unit Sprite Renderer
	SpriteRenderer* unitSpriteRenderer = gmUnitSprite->AddComponent<SpriteRenderer>();
	unitSpriteRenderer->texture = unitData->textures[color];
	unitSpriteRenderer->material = AssetManager::GetMaterialByName("2D Standard");
	unitSpriteRenderer->SetOrderInLayer(3);

	// Create selection Sprite Renderer
	selectionSpriteRenderer = gmUnitSprite->AddComponent<SpriteRenderer>();
	selectionSpriteRenderer->texture = unitData->selectionTexture;
	selectionSpriteRenderer->material = AssetManager::GetMaterialByName("2D Standard");
	selectionSpriteRenderer->SetOrderInLayer(11);

	lifeBarGO = new GameObject("Unit Life Bar");
	GetGameObject()->AddChild(lifeBarGO);
	lifeBarGO->GetTransform()->SetLocalPosition(Vector3(0, 0.2f, 0));

	// Create life bar Sprite renderer
	lifeBarSprRenderer = lifeBarGO->AddComponent<SpriteRenderer>();
	lifeBarSprRenderer->texture = AssetManager::defaultTexture;
	lifeBarSprRenderer->material = AssetManager::GetMaterialByName("2D Standard");
	if (color == Game::GetGame()->teamManager->localPlayerTeam->color)
		lifeBarSprRenderer->color = Color::CreateFromRGBAFloat(0, 1, 0, 1);
	else
		lifeBarSprRenderer->color = Color::CreateFromRGBAFloat(1, 0, 0, 1);
	lifeBarSprRenderer->SetOrderInLayer(4);

	currentHealth = unitData->health;
	UpdateLifeBar();
	Shoot();
}

void Unit::Update()
{
	selectionSpriteRenderer->SetIsEnabled(selected);
	int pathSize = path.size();
	if (pathSize != 0)
	{
		// Get new unit's position
		Vector3 newPos = GetTransform()->GetPosition();
		Vector2 dir = (path[currentPathNode] - Vector2(GetTransform()->GetPosition().x, GetTransform()->GetPosition().y)).normalize();
		newPos.x += dir.x * Time::GetDeltaTime() * unitData->movementSpeed;
		newPos.y += dir.y * Time::GetDeltaTime() * unitData->movementSpeed;

		// Set unit orientation
		float scale = 0.5f;
		float xScale = scale;
		float yScale = scale;
		if (unitData->rotateWhenMoving)
		{
			float angle = atan2(dir.x, dir.y);
			gmUnitSprite->GetTransform()->SetRotation(Vector3(0, 0, angle * 180 / M_PI - 90));

			if (dir.x < 0)
				yScale = -yScale;
		}
		else
		{
			if (dir.x < 0)
				xScale = -xScale;
		}
		gmUnitSprite->GetTransform()->SetLocalScale(Vector3(xScale, yScale, scale));

		GetTransform()->SetPosition(newPos);

		// Check if the unit needs to go to the next path node
		if (Vector2::Distance(Vector2(GetTransform()->GetPosition().x, GetTransform()->GetPosition().y), path[currentPathNode]) <= 0.01f)
		{
			currentPathNode++;
			// If the path is ended
			if (currentPathNode == pathSize)
			{
				// Reset pathfinding variables
				currentPathNode = 0;
				path.clear();
			}
		}
	}
	else
	{
		// Set rotation when stationary
		if (unitData->rotateWhenMoving)
		{
			float zAngle = 0;
			if (gmUnitSprite->GetTransform()->GetScale().y < 0)
				zAngle = 180;

			gmUnitSprite->GetTransform()->SetRotation(Vector3(0, 0, zAngle));
		}
	}
}

/// <summary>
/// 
/// </summary>
/// <param name="position"></param>
void Unit::SetDestination(Vector2Int position)
{
	//Set astar destination
	mapManager->astar->SetDestination(Vector2(round(GetTransform()->GetPosition().x), round(GetTransform()->GetPosition().y)), Vector2(position.x, position.y));
	path = mapManager->astar->GetPath();
	int pathCount = path.size();
	if (pathCount > 1)
		currentPathNode = 1;
	else
		currentPathNode = 0;

	// Add unit to the tile
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

/// <summary>
/// Shoot a bullet
/// </summary>
void Unit::Shoot()
{
	GameObject* bulletGO = new GameObject("Bullet");
	Bullet* bullet = bulletGO->AddComponent<Bullet>();
	bulletGO->GetTransform()->SetPosition(GetTransform()->GetPosition());
	bullet->unitData = unitData;
	Destroy(bullet);
}

/// <summary>
/// Update life bar
/// </summary>
void Unit::UpdateLifeBar()
{
	lifeBarGO->GetTransform()->SetLocalScale(Vector3(60 * (currentHealth / unitData->health), 6, 1));
}
