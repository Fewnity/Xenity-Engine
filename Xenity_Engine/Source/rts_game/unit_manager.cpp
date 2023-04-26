#include "unit_manager.h"
#include "unit.h"
#include "unit_data.h"
#include "camera_manager.h"
#include "../xenity.h"
#include "../pathfinding/astar.h"
#include "game.h"

UnitManager::UnitManager()
{
}

void UnitManager::SpawnUnits()
{
	Unit* newUnit = new Unit(unitsData[0]);
	GameObject* unitGM = new GameObject("Unit");
	unitGM->transform.SetPosition(Vector3(0, 0, 0));
	unitGM->AddExistingComponent(newUnit);
	units.push_back(newUnit);

	newUnit = new Unit(unitsData[4]);
	unitGM = new GameObject("Unit");
	unitGM->transform.SetPosition(Vector3(1, 2, 0));
	unitGM->AddExistingComponent(newUnit);
	units.push_back(newUnit);

	newUnit = new Unit(unitsData[10]);
	unitGM = new GameObject("Unit");
	unitGM->transform.SetPosition(Vector3(1, 1, 0));
	unitGM->AddExistingComponent(newUnit);
	units.push_back(newUnit);
}

void UnitManager::SelectUnits()
{
	Vector2 mouseWorldPosition = cameraManager->camera->MouseTo2DWorld();

	Vector2 endSelectionPos = mouseWorldPosition;

	int unitSize = units.size();
	if (fabs(game->startSelectionPos.x - endSelectionPos.x) >= 0.2f && fabs(game->startSelectionPos.y - endSelectionPos.y) >= 0.2f)
	{
		game->isDragging = true;
		game->SetSelection(true);
		Vector2 finalStartPos = game->startSelectionPos;
		Vector2 finalEndPos = endSelectionPos;

		if (game->startSelectionPos.x > endSelectionPos.x)
		{
			finalEndPos.x = game->startSelectionPos.x;
			finalStartPos.x = endSelectionPos.x;
		}
		if (game->startSelectionPos.y > endSelectionPos.y)
		{
			finalEndPos.y = game->startSelectionPos.y;
			finalStartPos.y = endSelectionPos.y;
		}

		game->lineRendererTop->startPosition = Vector3(finalStartPos.x + game->lineRendererTop->width, finalStartPos.y + (game->lineRendererTop->width / 2.0f), 0);
		game->lineRendererTop->endPosition = Vector3(finalEndPos.x - game->lineRendererTop->width, finalStartPos.y + (game->lineRendererTop->width / 2.0f), 0);

		game->lineRendererBottom->startPosition = Vector3(finalStartPos.x + game->lineRendererBottom->width, finalEndPos.y - (game->lineRendererBottom->width / 2.0f), 0);
		game->lineRendererBottom->endPosition = Vector3(finalEndPos.x - game->lineRendererBottom->width, finalEndPos.y - (game->lineRendererBottom->width / 2.0f), 0);

		game->lineRendererLeft->startPosition = Vector3(finalStartPos.x + (game->lineRendererLeft->width / 2.0f), finalStartPos.y, 0);
		game->lineRendererLeft->endPosition = Vector3(finalStartPos.x + (game->lineRendererLeft->width / 2.0f), finalEndPos.y, 0);

		game->lineRendererRight->startPosition = Vector3(finalEndPos.x - (game->lineRendererRight->width / 2.0f), finalStartPos.y, 0);
		game->lineRendererRight->endPosition = Vector3(finalEndPos.x - (game->lineRendererRight->width / 2.0f), finalEndPos.y, 0);

		for (int i = 0; i < unitSize; i++)
		{
			Unit* unit = units[i];
			unit->selected = false;
			if (game->isPointInsideAABB(Vector2(unit->gameObject->transform.GetPosition().x, unit->gameObject->transform.GetPosition().y), finalStartPos, finalEndPos)) {
				unit->selected = true;
			}
		}
	}
	else
	{
		game->SetSelection(false);
		if (game->isDragging)
		{
			game->isDragging = false;
			UnselectAllUnits();
		}
	}
}

void UnitManager::OnMouseUp()
{
	Vector2 mouseWorldPosition = cameraManager->camera->MouseTo2DWorld();
	int unitSize = units.size();
	std::vector<Unit*> unitFound;

	bool foundNewUnit = false;
	for (int i = 0; i < unitSize; i++)
	{
		Unit* unit = units[i];
		Vector2 unitMin = Vector2(unit->gameObject->transform.GetPosition().x - 0.2, unit->gameObject->transform.GetPosition().y - 0.2);
		Vector2 unitMax = Vector2(unit->gameObject->transform.GetPosition().x + 0.2, unit->gameObject->transform.GetPosition().y + 0.2);

		if (game->isPointInsideAABB(Vector2(mouseWorldPosition.x, mouseWorldPosition.y), unitMin, unitMax))
		{
			unitFound.push_back(unit);
			foundNewUnit = true;
			break;
		}
	}

	if (!foundNewUnit)
	{
		for (int i = 0; i < unitSize; i++)
		{
			Unit* unit = units[i];
			if (unit->selected)
			{
				game->astar->SetDestination(Vector2(round(unit->gameObject->transform.GetPosition().x), round(unit->gameObject->transform.GetPosition().y)), Vector2(round(mouseWorldPosition.x), round(mouseWorldPosition.y)));
				unit->path = game->astar->GetPath();
			}
		}
	}
	else
	{
		if (!InputSystem::GetKey(LEFT_CONTROL))
		{
			UnselectAllUnits();
		}
		int unitFoundCount = unitFound.size();
		for (int i = 0; i < unitFoundCount; i++)
		{
			unitFound[i]->selected = !unitFound[i]->selected;
		}
	}
}

void UnitManager::UnselectAllUnits()
{
	int unitSize = units.size();
	for (int i = 0; i < unitSize; i++)
	{
		Unit* unit = units[i];
		unit->selected = false;
	}
}

void UnitManager::LoadUnitData()
{
	for (int i = 0; i < 12; i++)
	{
		UnitData* newUnitData = new UnitData(i, game->crosshair);
		unitsData.push_back(newUnitData);
	}
}
