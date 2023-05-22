#include "game.h"
#include "../xenity.h"
#include <chrono>
#include <iostream>
#include "unit.h"
#include "unit_data.h"
#include "../engine/pathfinding/astar.h"
#include "managers/camera_manager.h"
#include "managers/unit_manager.h"
#include "managers/map_manager.h"
#include "managers/build_manager.h"
#include "managers/team_manager.h"

Game* Game::game;

/// <summary>
/// Init game
/// </summary>
void Game::Init()
{
	game = this;
	LoadGameData();

	GameObject* managersGameObject = new GameObject("Managers");

	//Create Team Manager
	teamManager = managersGameObject->AddComponent<TeamManager>();
	teamManager->CreateTeam(Blue, true);
	teamManager->CreateTeam(Orange, false);

	//Create Map Manager
	mapManager = managersGameObject->AddComponent<MapManager>();
	mapManager->LoadMapData();
	mapManager->GenerateMap();
	mapManager->CreateTileMaps();

	//Create Camera Manager
	cameraManager = managersGameObject->AddComponent<CameraManager>();
	cameraManager->Init();

	//Create Build Manager
	buildManager = managersGameObject->AddComponent<BuildManager>();
	buildManager->game = this;
	buildManager->mapManager = mapManager;
	buildManager->cameraManager = cameraManager;
	buildManager->LoadBuildingsData();

	//Create Unit Manager
	unitManager = managersGameObject->AddComponent<UnitManager>();
	unitManager->cameraManager = cameraManager;
	unitManager->game = this;
	unitManager->LoadUnitsData();
	unitManager->SpawnUnits();

	//Create crosshair
	gameObjectCrosshair->GetTransform()->SetPosition(Vector3(0, 0, 0));
	SpriteRenderer* crosshairSprRenderer = gameObjectCrosshair->AddComponent<SpriteRenderer>();
	crosshairSprRenderer->texture = crosshair;
	crosshairSprRenderer->material = material2DWithZ;
	crosshairSprRenderer->color = Color::CreateFromRGBAFloat(0, 0, 0, 0.2f);
	crosshairSprRenderer->SetOrderInLayer(10);

	//Create lines for selection
	Color selectionLineColor = Color::CreateFromRGBAFloat(0, 0, 0, 0.2f);
	lineRendererTop = gameObjectLineRenderers->AddComponent<LineRenderer>();
	lineRendererBottom = gameObjectLineRenderers->AddComponent<LineRenderer>();
	lineRendererLeft = gameObjectLineRenderers->AddComponent<LineRenderer>();
	lineRendererRight = gameObjectLineRenderers->AddComponent<LineRenderer>();

	lineRendererTop->width = lineRendererBottom->width = lineRendererLeft->width = lineRendererRight->width = 0.1f;
	lineRendererTop->material = lineRendererBottom->material = lineRendererLeft->material = lineRendererRight->material = material2D;
	lineRendererTop->color = lineRendererBottom->color = lineRendererLeft->color = lineRendererRight->color = selectionLineColor;
	lineRendererTop->SetOrderInLayer(10);
	lineRendererBottom->SetOrderInLayer(10);
	lineRendererLeft->SetOrderInLayer(10);
	lineRendererRight->SetOrderInLayer(10);

	//Create canvas
	GameObject* canvasGO = new GameObject("Canvas");
	//Add the ressources text to the canvas
	ressourcesTextRenderer = canvasGO->AddComponent<TextRendererCanvas>();
	ressourcesTextRenderer->font = UiManager::fonts[0];
	ressourcesTextRenderer->size = 0.8f;
	ressourcesTextRenderer->shader = shaderTextCanvas;
	ressourcesTextRenderer->horizontalAligment = H_Right;
	ressourcesTextRenderer->verticalAlignment = V_Bottom;
	ressourcesTextRenderer->position = Vector3(0, 0, 0);

	//Add the game mode text to the canvas
	modeTextRenderer = canvasGO->AddComponent<TextRendererCanvas>();
	modeTextRenderer->font = UiManager::fonts[0];
	modeTextRenderer->size = 0.8f;
	modeTextRenderer->shader = shaderTextCanvas;
	modeTextRenderer->horizontalAligment = H_Center;
	modeTextRenderer->verticalAlignment = V_Top;
	modeTextRenderer->position = Vector3(0.5f, 1, 0);
}

void Game::LoadGameData()
{
	//Load shaders
	Shader* shader = new Shader("vertexStandard.shader", "fragmentStandard.shader");
	Shader* shader3 = new Shader("3D/vStandard.shader", "3D/fStandard.shader");
	Shader* shader2 = new Shader("vertex2.shader", "fragment2.shader");
	Shader* shaderText = new Shader("vertexText.shader", "fragmentText.shader");
	shaderTextCanvas = new Shader("vertexTextCanvas.shader", "fragmentTextCanvas.shader");
	Shader* shader2D = new Shader("vertex2D.shader", "fragment2D.shader");
	Shader* shaderStandard2D = new Shader("vertexStandard2D.shader", "fragmentStandard2D.shader");
	Shader* shaderStandard2DText = new Shader("vertexStandard2DText.shader", "fragmentStandard2DText.shader");
	Shader* shaderStandard2DWithZ = new Shader("vertexStandard2DZ.shader", "fragmentStandard2DZ.shader");

	//load textures
	textureShip = new Texture("ship_0000.png", "Ship");
	crosshair = new Texture("rts/crosshairs/crosshair.png", "Crosshair");
	crosshair->SetPixelPerUnit(128);
	gradient = new Texture("gradient.png", "gradient");

	//Create materials
	material2D = new Material("2D Standard");
	material2D->shader = shaderStandard2D;
	material2DWithZ = new Material("2D Standard With Z");
	material2DWithZ->shader = shaderStandard2DWithZ;
}

bool Game::isPointInsideAABB(Vector2 point, Vector2 aMin, Vector2 aMax)
{
	return (
		point.x >= aMin.x &&
		point.x <= aMax.x &&
		point.y >= aMin.y &&
		point.y <= aMax.y
		);
}

bool Game::intersect(Vector2 aMin, Vector2 aMax, Vector2 bMin, Vector2 bMax)
{
	return (
		aMin.x <= bMax.x &&
		aMax.x >= bMin.x &&
		aMin.y <= bMax.y &&
		aMax.y >= bMin.y
		);
}

/// <summary>
/// Disable / Enable selection UI
/// </summary>
/// <param name="isSelecting"></param>
void Game::SetSelectionUI(bool isSelecting)
{
	lineRendererTop->SetIsEnabled(isSelecting);
	lineRendererBottom->SetIsEnabled(isSelecting);
	lineRendererLeft->SetIsEnabled(isSelecting);
	lineRendererRight->SetIsEnabled(isSelecting);
	gameObjectCrosshair->SetActive(!isSelecting);
}

/// <summary>
/// Update ressources text
/// </summary>
void Game::UpdateRessourcesText()
{
	std::string ressourceText = "";
	ressourceText += "Wood: " + std::to_string(wood) + "\n";
	ressourceText += "Stone: " + std::to_string(rock) + "\n";
	ressourceText += "Gold: " + std::to_string(gold) + "\n";
	ressourceText += "Crystal: " + std::to_string(crystal) + "\n";

	ressourcesTextRenderer->text = ressourceText;
}

/// <summary>
/// Update game mode text
/// </summary>
void Game::UpdateModeText()
{
	std::string modeText = "Mode: ";

	if (manageMode == ManageUnits)
		modeText += "Manage units";
	else if (manageMode == ManageBuildings)
		modeText += "Manage buildings";
	else if (manageMode == SpawnUnit)
		modeText += "Spawn unit";

	modeTextRenderer->text = modeText;
}

/// <summary>
/// Move cursor
/// </summary>
void Game::MoveCursor()
{
	Vector2 mouseWorldPosition = cameraManager->camera->MouseTo2DWorld();
	cursorPosition.x = Math::Lerp(cursorPosition.x, round(mouseWorldPosition.x), Time::GetUnscaledDeltaTime() * 20);
	cursorPosition.y = Math::Lerp(cursorPosition.y, round(mouseWorldPosition.y), Time::GetUnscaledDeltaTime() * 20);
	gameObjectCrosshair->GetTransform()->SetPosition(Vector3(cursorPosition.x, cursorPosition.y, 0));
}

void Game::OnMouseUp()
{
	unitManager->OnMouseUp();
	buildManager->OnMouseUp();
}

/// <summary>
/// Check if the player is doing something with the keyboard
/// </summary>
void Game::CheckKeyboardInput()
{
	if (InputSystem::GetKeyDown(ESCAPE))
	{
	}

	// Change mode
	if (InputSystem::GetKeyDown(NUM_1))
	{
		manageMode = ManageUnits;
	}
	else if (InputSystem::GetKeyDown(NUM_2))
	{
		manageMode = ManageBuildings;
	}
	else if (InputSystem::GetKeyDown(NUM_3))
	{
		manageMode = SpawnUnit;
	}
}

/// <summary>
/// Check if the player is doing something with the mouse
/// </summary>
void Game::CheckMouseInput() 
{
	Vector2 mouseWorldPosition = cameraManager->camera->MouseTo2DWorld();

	if (InputSystem::GetKeyDown(MOUSE_LEFT))
	{
		startSelectionPos = mouseWorldPosition;
	}
	if (InputSystem::GetKey(MOUSE_LEFT))
	{
		if (manageMode == ManageUnits)
			unitManager->SelectUnits();
	}
	if (InputSystem::GetKeyDown(MOUSE_RIGHT))
	{
		startMousePosition = InputSystem::mousePosition;
	}
	if (InputSystem::GetKeyUp(MOUSE_RIGHT))
	{
		//Unselect all units if the player don't want to move the camera
		Vector2 endMousePosition = InputSystem::mousePosition;
		if (manageMode == ManageUnits)
		{
			float dis = Vector2::Distance(startMousePosition, endMousePosition);

			if (dis <= 4)
			{
				unitManager->UnselectAllUnits();
			}
		}
	}
	if (InputSystem::GetKeyUp(MOUSE_LEFT))
	{
		OnMouseUp();
	}
}

/// <summary>
/// Game loop
/// </summary>
void Game::Loop()
{
	CheckKeyboardInput();
	CheckMouseInput();
	MoveCursor();
	UpdateRessourcesText();
	UpdateModeText();
}