#include "unit_manager.h"
#include "../unit.h"
#include "../unit_data.h"
#include "camera_manager.h"
#include "../../xenity.h"
#include "../game.h"
#include "map_manager.h"
#include "team_manager.h"

UnitManager::UnitManager()
{
	reflectedInts.insert(std::pair<std::string, int*>("Max Unit Per Tile", &maxUnitPerTile));
	componentName = "Unit Manager";
}

void UnitManager::Awake()
{
	//Create all placement layout

	//1 slot
	UnitPlacement* placement = new UnitPlacement();
	placement->positions.push_back(Vector2(0, 0));
	unitPlacements.push_back(placement);

	//2 slots
	placement = new UnitPlacement();
	placement->positions.push_back(Vector2(0.25f, 0.25f));
	placement->positions.push_back(Vector2(-0.25f, -0.25f));
	unitPlacements.push_back(placement);

	//3 slots
	placement = new UnitPlacement();
	placement->positions.push_back(Vector2(0.25f, 0.25f));
	placement->positions.push_back(Vector2(-0.25f, -0.25f));
	placement->positions.push_back(Vector2(0.0f, 0.0f));
	unitPlacements.push_back(placement);

	//4 slots
	placement = new UnitPlacement();
	placement->positions.push_back(Vector2(0.25f, 0.25f));
	placement->positions.push_back(Vector2(0.25f, -0.25f));
	placement->positions.push_back(Vector2(-0.25f, 0.25f));
	placement->positions.push_back(Vector2(-0.25f, -0.25f));
	unitPlacements.push_back(placement);

	//5 slots
	placement = new UnitPlacement();
	placement->positions.push_back(Vector2(0.25f, 0.25f));
	placement->positions.push_back(Vector2(0.25f, -0.25f));
	placement->positions.push_back(Vector2(-0.25f, 0.25f));
	placement->positions.push_back(Vector2(-0.25f, -0.25f));
	placement->positions.push_back(Vector2(0.0f, 0.0f));
	unitPlacements.push_back(placement);
}

/// <summary>
/// Spawn an unit
/// </summary>
/// <param name="position"></param>
/// <param name="color"></param>
/// <param name="unitId"></param>
void UnitManager::SpawnUnit(Vector2Int position, TeamColor color, int unitId)
{
	Team* team = game->teamManager->GetTeamFromColor(color);
	if (team)
	{
		Unit* newUnit = nullptr;
		GameObject* unitGM = new GameObject("Unit");
		unitGM->GetTransform()->SetPosition(position);
		newUnit = unitGM->AddComponent<Unit>();
		newUnit->unitData = unitsData[unitId];
		newUnit->color = color;
		newUnit->mapManager = game->mapManager;

		team->units.push_back(newUnit);
		if (game->mapManager->IsValidPosition(position.x, position.y))
		{
			game->mapManager->GetTile(position.x, position.y)->AddUnit(newUnit);
		}
	}
}

/// <summary>
/// Spawn units (For testing)
/// </summary>
void UnitManager::SpawnUnits()
{
	SpawnUnit(Vector2Int(0, 0), Blue, 1);
	SpawnUnit(Vector2Int(0, 0), Blue, 0);
	SpawnUnit(Vector2Int(1, 2), Blue, 1);
	SpawnUnit(Vector2Int(1, 1), Blue, 3);
	SpawnUnit(Vector2Int(3, 1), Blue, 9);
	SpawnUnit(Vector2Int(3, 2), Blue, 9);
	SpawnUnit(Vector2Int(3, 3), Blue, 3);

	SpawnUnit(Vector2Int(0, 1), Orange, 0);
	SpawnUnit(Vector2Int(0, 1), Orange, 9);
	SpawnUnit(Vector2Int(0, 1), Orange, 3);
}

/// <summary>
/// Select units from mouse
/// </summary>
void UnitManager::SelectUnits()
{
	Vector2 mouseWorldPosition = cameraManager->camera->MouseTo2DWorld();

	Vector2 endSelectionPos = mouseWorldPosition;

	if (fabs(game->startSelectionPos.x - endSelectionPos.x) >= 0.2f && fabs(game->startSelectionPos.y - endSelectionPos.y) >= 0.2f)
	{
		game->isDragging = true;
		game->SetSelectionUI(true);
		Vector2 finalStartPos = game->startSelectionPos;
		Vector2 finalEndPos = endSelectionPos;

		//Invert End and Start position to avoid weird corners
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

		//Set line renderer positions
		game->lineRendererTop->startPosition = Vector3(finalStartPos.x + game->lineRendererTop->width, finalStartPos.y + (game->lineRendererTop->width / 2.0f), 0);
		game->lineRendererTop->endPosition = Vector3(finalEndPos.x - game->lineRendererTop->width, finalStartPos.y + (game->lineRendererTop->width / 2.0f), 0);

		game->lineRendererBottom->startPosition = Vector3(finalStartPos.x + game->lineRendererBottom->width, finalEndPos.y - (game->lineRendererBottom->width / 2.0f), 0);
		game->lineRendererBottom->endPosition = Vector3(finalEndPos.x - game->lineRendererBottom->width, finalEndPos.y - (game->lineRendererBottom->width / 2.0f), 0);

		game->lineRendererLeft->startPosition = Vector3(finalStartPos.x + (game->lineRendererLeft->width / 2.0f), finalStartPos.y, 0);
		game->lineRendererLeft->endPosition = Vector3(finalStartPos.x + (game->lineRendererLeft->width / 2.0f), finalEndPos.y, 0);

		game->lineRendererRight->startPosition = Vector3(finalEndPos.x - (game->lineRendererRight->width / 2.0f), finalStartPos.y, 0);
		game->lineRendererRight->endPosition = Vector3(finalEndPos.x - (game->lineRendererRight->width / 2.0f), finalEndPos.y, 0);

		int unitSize = game->teamManager->localPlayerTeam->units.size();
		//Check if units are in the selection zone
		for (int i = 0; i < unitSize; i++)
		{
			Unit* unit = game->teamManager->localPlayerTeam->units[i];
			unit->selected = false;
			if (game->isPointInsideAABB(Vector2(unit->GetTransform()->GetPosition().x, unit->GetTransform()->GetPosition().y), finalStartPos, finalEndPos))
			{
				unit->selected = true;
			}
		}
	}
	else
	{
		game->SetSelectionUI(false);
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
	if (game->manageMode == ManageUnits)
	{
		if (game->isDragging == true) //If the player was dragging the mouse to select unit
		{
			//Disable dragging UI
			game->isDragging = false;
			game->SetSelectionUI(false);
		}
		else //If the player only clicks, check if he wants to select an unit or to move selected units
		{
			//Check if the player clicks on an unit
			int unitSize = game->teamManager->localPlayerTeam->units.size();
			Unit* unitFound = nullptr;
			for (int i = 0; i < unitSize; i++)
			{
				Unit* unit = game->teamManager->localPlayerTeam->units[i];
				Vector2 unitMin = Vector2(unit->GetTransform()->GetPosition().x - 0.2, unit->GetTransform()->GetPosition().y - 0.2);
				Vector2 unitMax = Vector2(unit->GetTransform()->GetPosition().x + 0.2, unit->GetTransform()->GetPosition().y + 0.2);

				if (game->isPointInsideAABB(Vector2(mouseWorldPosition.x, mouseWorldPosition.y), unitMin, unitMax))
				{
					unitFound = unit;
					break;
				}
			}

			if (!unitFound) //If no unit has been clicked
			{
				//Try to place units at the cursor position
				Vector2Int tilePos = Vector2Int(round(mouseWorldPosition.x), round(mouseWorldPosition.y));
				if (Game::GetGame()->mapManager->IsValidPosition(tilePos.x, tilePos.y))
				{
					std::vector<MapManager::Tile*> tilesToTest;
					int offsetSize = 0;
					tilesToTest.push_back(Game::GetGame()->mapManager->GetTile(tilePos.x, tilePos.y));
					int currentTileToTest = 0;
					bool stop = false;
					for (int i = 0; i < unitSize; i++)
					{
						Unit* unit = game->teamManager->localPlayerTeam->units[i];
						if (unit->selected)
						{
							bool placed = false;

							do
							{
								//If the tile is not fully filled
								if ((tilesToTest[currentTileToTest]->GetUnitCount() < maxUnitPerTile || unit->destinationTile == tilesToTest[currentTileToTest]) && tilesToTest[currentTileToTest]->prop == nullptr)
								{
									//Set the current unit new destination
									if (unit->destinationTile != tilesToTest[currentTileToTest])
										unit->SetDestination(Game::GetGame()->mapManager->GetTilePosition(tilesToTest[currentTileToTest]));
									placed = true;
								}

								//If the unit wasn't able to be placed
								if (!placed)
								{
									//Remove the fully filled tile
									tilesToTest.erase(tilesToTest.begin() + currentTileToTest);
									offsetSize++;
									//Try to take another tile
									if (tilesToTest.size() == 0)
									{
										int squareSize = 1 + offsetSize * 2;
										for (int squareX = 0; squareX < squareSize; squareX++)
										{
											for (int squareY = 0; squareY < squareSize; squareY++)
											{
												if (squareX == 0 || squareX == squareSize - 1 || squareY == 0 || squareY == squareSize - 1)
												{
													int off = (int)(squareSize / 2.0f);
													if (Game::GetGame()->mapManager->IsValidPosition(tilePos.x - off + squareX, tilePos.y - off + squareY) &&
														!Game::GetGame()->mapManager->HasPropAtPosition(tilePos.x - off + squareX, tilePos.y - off + squareY))
													{
														tilesToTest.push_back(Game::GetGame()->mapManager->GetTile(tilePos.x - off + squareX, tilePos.y - off + squareY));
													}
												}

											}
										}
									}
									if (tilesToTest.size() == 0)
									{
										stop = true;
										break;
									}
									currentTileToTest = rand() % tilesToTest.size();
								}
							} while (!placed);
						}
						if (stop)
							break;
					}
				}
			}
			else
			{
				if (!InputSystem::GetKey(LEFT_CONTROL))
				{
					UnselectAllUnits();
				}
				unitFound->selected = !unitFound->selected;
			}
		}
	}
	else if (game->manageMode == ManageMode::SpawnUnit)
	{
		//Spawn a new unit at cursor position
		Vector2Int tilePos = Vector2Int(round(mouseWorldPosition.x), round(mouseWorldPosition.y));
		SpawnUnit(tilePos, game->teamManager->localPlayerTeam->color, 0);
	}
}

/// <summary>
/// Unselect all unitys
/// </summary>
void UnitManager::UnselectAllUnits()
{
	int unitSize = game->teamManager->localPlayerTeam->units.size();
	for (int i = 0; i < unitSize; i++)
	{
		Unit* unit = game->teamManager->localPlayerTeam->units[i];
		unit->selected = false;
	}
}

/// <summary>
/// Load units data
/// </summary>
void UnitManager::LoadUnitsData()
{
	Texture* bulletTexture = new Texture("rts/bullet.png", "Bullet");
	for (int i = 0; i < 12; i++)
	{
		UnitData* newUnitData = new UnitData(i, game->crosshair, bulletTexture);
		if (i >= 6)
			newUnitData->rotateWhenMoving = true;

		unitsData.push_back(newUnitData);
	}

	UnitData* unit = unitsData[0];
	unit->name = "Worker";
	unit->health = 10;
	unit->bulletSpeed = 1;
	unit->fireRate = 1;
	unit->movementSpeed = 1;
	unit->used = true;

	unit = unitsData[1];
	unit->name = "Soldier";
	unit->bulletSpeed = 1;
	unit->health = 20;
	unit->fireRate = 1;
	unit->movementSpeed = 1;
	unit->used = true;

	unit = unitsData[3];
	unit->name = "Bazooka";
	unit->bulletSpeed = 1;
	unit->health = 20;
	unit->fireRate = 1;
	unit->movementSpeed = 0.6f;
	unit->used = true;

	unit = unitsData[9];
	unit->name = "Tank";
	unit->bulletSpeed = 1;
	unit->health = 200;
	unit->fireRate = 1;
	unit->movementSpeed = 0.8f;
	unit->used = true;
}