#include "unit_manager.h"
#include "unit.h"
#include "unit_data.h"
#include "camera_manager.h"
#include "../xenity.h"
#include "../engine/pathfinding/astar.h"
#include "game.h"
#include "map_manager.h"
#include "team_manager.h"

UnitManager::UnitManager()
{
	reflectedInts.insert(std::pair<std::string, int*>("Max Unit Per Tile", &maxUnitPerTile));
	componentName = "Unit Manager";

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

void UnitManager::SpawnUnit(Vector2Int position, TeamColor color, int unitId)
{
	Unit* newUnit = nullptr;
	GameObject* unitGM = new GameObject("Unit");
	unitGM->transform.SetPosition(position);
	newUnit = unitGM->AddComponent<Unit>();
	newUnit->unitData = unitsData[unitId];
	newUnit->color = color;
	newUnit->mapManager = game->mapManager;
	//units.push_back(newUnit);
	game->teamManager->GetTeamFromColor(color)->units.push_back(newUnit);
	if (game->mapManager->IsValidPosition(position.x, position.y))
	{
		game->mapManager->GetTile(position.x, position.y)->AddUnit(newUnit);
	}
}

void UnitManager::SpawnUnits()
{
	SpawnUnit(Vector2Int(0, 0), Blue, 0);
	SpawnUnit(Vector2Int(0, 0), Blue, 0);

	SpawnUnit(Vector2Int(1, 2), Blue, 4);
	SpawnUnit(Vector2Int(1, 1), Blue, 10);
	SpawnUnit(Vector2Int(3, 1), Blue, 9);
	SpawnUnit(Vector2Int(3, 2), Blue, 8);
	SpawnUnit(Vector2Int(3, 3), Blue, 11);

	SpawnUnit(Vector2Int(0, 1), Orange, 0);

	/*Unit* newUnit = nullptr;
	GameObject* unitGM = new GameObject("Unit");
	unitGM->transform.SetPosition(Vector3(0, 0, 0));
	newUnit = unitGM->AddComponent<Unit>();
	newUnit->unitData = unitsData[0];
	newUnit->mapManager = game->mapManager;
	units.push_back(newUnit);

	unitGM = new GameObject("Unit");
	unitGM->transform.SetPosition(Vector3(1, 2, 0));
	newUnit = unitGM->AddComponent<Unit>();
	newUnit->unitData = unitsData[4];
	newUnit->mapManager = game->mapManager;
	units.push_back(newUnit);

	unitGM = new GameObject("Unit");
	unitGM->transform.SetPosition(Vector3(1, 1, 0));
	newUnit = unitGM->AddComponent<Unit>();
	newUnit->unitData = unitsData[10];
	newUnit->mapManager = game->mapManager;
	units.push_back(newUnit);

	unitGM = new GameObject("Unit");
	unitGM->transform.SetPosition(Vector3(3, 1, 0));
	newUnit = unitGM->AddComponent<Unit>();
	newUnit->unitData = unitsData[9];
	newUnit->mapManager = game->mapManager;
	units.push_back(newUnit);

	unitGM = new GameObject("Unit");
	unitGM->transform.SetPosition(Vector3(3, 2, 0));
	newUnit = unitGM->AddComponent<Unit>();
	newUnit->unitData = unitsData[8];
	newUnit->mapManager = game->mapManager;
	units.push_back(newUnit);

	unitGM = new GameObject("Unit");
	unitGM->transform.SetPosition(Vector3(3, 3, 0));
	newUnit = unitGM->AddComponent<Unit>();
	newUnit->unitData = unitsData[11];
	newUnit->mapManager = game->mapManager;
	units.push_back(newUnit);*/
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
			if (game->isPointInsideAABB(Vector2(unit->GetGameObject()->transform.GetPosition().x, unit->GetGameObject()->transform.GetPosition().y), finalStartPos, finalEndPos))
			{
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
	if (game->manageMode == ManageUnits)
	{
		if (game->isDragging == true)
		{
			game->isDragging = false;
			game->SetSelection(false);
		}
		else
		{
			Vector2 mouseWorldPosition = cameraManager->camera->MouseTo2DWorld();
			int unitSize = units.size();
			std::vector<Unit*> unitFound;

			bool foundNewUnit = false;
			for (int i = 0; i < unitSize; i++)
			{
				Unit* unit = units[i];
				Vector2 unitMin = Vector2(unit->GetGameObject()->transform.GetPosition().x - 0.2, unit->GetGameObject()->transform.GetPosition().y - 0.2);
				Vector2 unitMax = Vector2(unit->GetGameObject()->transform.GetPosition().x + 0.2, unit->GetGameObject()->transform.GetPosition().y + 0.2);

				if (game->isPointInsideAABB(Vector2(mouseWorldPosition.x, mouseWorldPosition.y), unitMin, unitMax))
				{
					unitFound.push_back(unit);
					foundNewUnit = true;
					break;
				}
			}

			if (!foundNewUnit)
			{
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
						Unit* unit = units[i];
						if (unit->selected)
						{
							bool placed = false;
							//std::cout << "unit" << i << std::endl;

							do
							{
								if ((tilesToTest[currentTileToTest]->GetUnitCount() < maxUnitPerTile || unit->destinationTile == tilesToTest[currentTileToTest]) && tilesToTest[currentTileToTest]->prop == nullptr)
								{
									/*if (unit->destinationTile == tilesToTest[currentTileToTest])
										std::cout << "UWU" << std::endl;
									if (tilesToTest[currentTileToTest]->GetUnitCount() < maxUnitPerTile)
										std::cout << "UWU" << tilesToTest[currentTileToTest]->GetUnitCount() << std::endl;*/
									if (unit->destinationTile != tilesToTest[currentTileToTest])
										unit->SetDestination(Game::GetGame()->mapManager->GetTilePosition(tilesToTest[currentTileToTest]));
									placed = true;
								}

								if (!placed)
								{
									//std::cout << "TILE NOT USABLE" << std::endl;
									tilesToTest.erase(tilesToTest.begin() + currentTileToTest);
									offsetSize++;
									if (tilesToTest.size() == 0)
									{
										//std::cout << "FINDING NEW TILE" << std::endl;
										int squareSize = 1 + offsetSize * 2;
										for (int squareX = 0; squareX < squareSize; squareX++)
										{
											for (int squareY = 0; squareY < squareSize; squareY++)
											{
												if (squareX == 0 || squareX == squareSize - 1 || squareY == 0 || squareY == squareSize - 1)
												{
													int off = (int)(squareSize / 2.0f);
													//std::cout << "TRY: " << (tilePos.x - off + squareX) << "; " << (tilePos.y - off + squareY) << std::endl;
													if (Game::GetGame()->mapManager->IsValidPosition(tilePos.x - off + squareX, tilePos.y - off + squareY) &&
														!Game::GetGame()->mapManager->HasPropAtPosition(tilePos.x - off + squareX, tilePos.y - off + squareY))
													{
														tilesToTest.push_back(Game::GetGame()->mapManager->GetTile(tilePos.x - off + squareX, tilePos.y - off + squareY));
														//std::cout << "ADD: " << (tilePos.x - off + squareX) << "; " << (tilePos.y - off + squareY) << std::endl;
													}
												}

											}
										}
									}
									//std::cout << "FINDING NEW TILE ENDED" << std::endl;
									//TODO IF tilesToTest.size() == 0
									if (tilesToTest.size() == 0) {
										stop = true;
										break;
									}
									currentTileToTest = rand() % tilesToTest.size();
									//std::cout << "NEW TILE TO TEST: " << currentTileToTest << std::endl;
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
				int unitFoundCount = unitFound.size();
				for (int i = 0; i < unitFoundCount; i++)
				{
					unitFound[i]->selected = !unitFound[i]->selected;
				}
			}
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
		if (i >= 6)
			newUnitData->rotateWhenMoving = true;

		unitsData.push_back(newUnitData);
	}
}