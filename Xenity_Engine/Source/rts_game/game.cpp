#include "game.h"
#include "../xenity.h"
#include <chrono>
#include <iostream>
#include "unit.h"
#include "unit_data.h"
#include "../engine/pathfinding/astar.h"
#include "camera_manager.h"
#include "unit_manager.h"
#include "map_manager.h"
#include "build_manager.h"
#include "team_manager.h"

Game* Game::game;

Game* Game::GetGame()
{
	return game;
}

/// <summary>
/// Init game
/// </summary>
void Game::Init()
{
	game = this;
	LoadGameData();

	GameObject* managersGameObject = new GameObject("Managers");

	teamManager = managersGameObject->AddComponent<TeamManager>();
	teamManager->CreateTeam(Blue, true);
	teamManager->CreateTeam(Orange, false);

	mapManager = managersGameObject->AddComponent<MapManager>();
	mapManager->LoadMapData();
	mapManager->GenerateMap();
	mapManager->CreateTileMaps();

	cameraManager = managersGameObject->AddComponent<CameraManager>();
	cameraManager->Init();

	buildManager = managersGameObject->AddComponent<BuildManager>();
	buildManager->game = this;
	buildManager->mapManager = mapManager;
	buildManager->cameraManager = cameraManager;
	buildManager->LoadBuildingsData();

	unitManager = managersGameObject->AddComponent<UnitManager>();
	unitManager->cameraManager = cameraManager;
	unitManager->game = this;
	unitManager->LoadUnitData();
	unitManager->SpawnUnits();

	gameObjectCrosshair2->transform.SetPosition(Vector3(0, 0, 0));
	gameObjectCrosshair2->SetActive(false);
	SpriteRenderer* sprGrad = gameObjectCrosshair2->AddComponent<SpriteRenderer>();
	sprGrad->texture = gradient;
	sprGrad->material = material2DWithZ;
	sprGrad->color = Color::CreateFromRGBAFloat(1, 1, 1, 1);

	gameObjectCrosshair->transform.SetPosition(Vector3(0, 0, 0));
	SpriteRenderer* spr5 = gameObjectCrosshair->AddComponent<SpriteRenderer>();
	spr5->texture = crosshair;
	spr5->material = material2DWithZ;

	spr5->color = Color::CreateFromRGBAFloat(0, 0, 0, 0.2f);
	spr5->SetOrderInLayer(10);

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

	/*TextRenderer* textRenderer = new TextRenderer(UiManager::fonts[0], 5, shaderText);
	textRenderer->text = "Salut à tous les amissssss\nzefzefizeifb ezfibzef";
	gameObjectSprite->AddExistingComponent(textRenderer);*/


	//SDL_SetRelativeMouseMode(SDL_TRUE);
	//SDL_SetRelativeMouseMode(SDL_FALSE);
	//camera->SetProjectionType(Orthographic);
	//camera->SetProjectionSize(2.5f * cameraZoom);

	GameObject* ParentTest = new GameObject("Parent0");
	GameObject* childTest1 = new GameObject("child0");
	GameObject* childTest2 = new GameObject("child1");
	GameObject* childTest3 = new GameObject("child2");

	ParentTest->AddChild(childTest1);
	ParentTest->AddChild(childTest2);
	childTest1->AddChild(childTest3);

	GameObject* canvasGO = new GameObject("Canvas");
	ressourcesTextRenderer = canvasGO->AddComponent< TextRendererCanvas>();
	ressourcesTextRenderer->font = UiManager::fonts[0];
	ressourcesTextRenderer->size = 0.8f;
	ressourcesTextRenderer->shader = shaderTextCanvas;
	ressourcesTextRenderer->horizontalAligment = H_Right;
	ressourcesTextRenderer->verticalAlignment = V_Bottom;
	ressourcesTextRenderer->position = Vector3(0, 0, 0);

	modeTextRenderer = canvasGO->AddComponent< TextRendererCanvas>();
	modeTextRenderer->font = UiManager::fonts[0];
	modeTextRenderer->size = 0.8f;
	modeTextRenderer->shader = shaderTextCanvas;
	modeTextRenderer->horizontalAligment = H_Center;
	modeTextRenderer->verticalAlignment = V_Top;
	modeTextRenderer->position = Vector3(0.5, 1, 0);
}

void Game::LoadGameData()
{
	Shader* shader = new Shader("vertexStandard.shader", "fragmentStandard.shader");

	Shader* shader3 = new Shader("3D/vStandard.shader", "3D/fStandard.shader");

	Shader* shader2 = new Shader("vertex2.shader", "fragment2.shader");
	Shader* shaderText = new Shader("vertexText.shader", "fragmentText.shader");
	shaderTextCanvas = new Shader("vertexTextCanvas.shader", "fragmentTextCanvas.shader");
	Shader* shader2D = new Shader("vertex2D.shader", "fragment2D.shader");
	Shader* shaderStandard2D = new Shader("vertexStandard2D.shader", "fragmentStandard2D.shader");
	Shader* shaderStandard2DText = new Shader("vertexStandard2DText.shader", "fragmentStandard2DText.shader");
	Shader* shaderStandard2DWithZ = new Shader("vertexStandard2DZ.shader", "fragmentStandard2DZ.shader");

	//SceneGame1* scene = new SceneGame1();
	//SceneManager::LoadScene(scene);

	textureShip = new Texture("ship_0000.png", "Ship");

	crosshair = new Texture("rts/crosshairs/crosshair.png", "Crosshair");
	crosshair->SetPixelPerUnit(128);


	gradient = new Texture("gradient.png", "gradient");



	material2D = new Material("2D Standard");
	material2D->shader = shaderStandard2D;

	material2DWithZ = new Material("2D Standard With Z");
	material2DWithZ->shader = shaderStandard2DWithZ;

	//material2D->SetAttribut("color", Vector4(1, 0, 1,1));
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

void Game::SetSelection(bool isSelecting)
{
	lineRendererTop->SetIsEnabled(isSelecting);
	lineRendererBottom->SetIsEnabled(isSelecting);
	lineRendererLeft->SetIsEnabled(isSelecting);
	lineRendererRight->SetIsEnabled(isSelecting);
	gameObjectCrosshair->SetActive(!isSelecting);
}

void Game::UpdateRessourcesText()
{
	std::string ressourceText = "";
	ressourceText += "Wood: " + std::to_string(wood) + "\n";
	ressourceText += "Stone: " + std::to_string(rock) + "\n";
	ressourceText += "Gold: " + std::to_string(gold) + "\n";
	ressourceText += "Crystal: " + std::to_string(crystal) + "\n";

	ressourcesTextRenderer->text = ressourceText;
}

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

void Game::MoveCursor()
{
	Vector2 mouseWorldPosition = cameraManager->camera->MouseTo2DWorld();

	//gameObjectCrosshair2->transform.SetPosition(Vector3(mouseWorldPosition.x, mouseWorldPosition.y, 1));

	//Move cursor
	cursorPosition.x = Math::Lerp(cursorPosition.x, round(mouseWorldPosition.x), Time::GetUnscaledDeltaTime() * 20);
	cursorPosition.y = Math::Lerp(cursorPosition.y, round(mouseWorldPosition.y), Time::GetUnscaledDeltaTime() * 20);
	gameObjectCrosshair->transform.SetPosition(Vector3(cursorPosition.x, cursorPosition.y, 0));
}

void Game::OnMouseUp()
{
	/*Vector2 mouseWorldPosition = cameraManager->camera->MouseTo2DWorld();
	if (manageMode == ManageUnits)
	{
		if (isDragging == true)
		{
			isDragging = false;
			SetSelection(false);
		}
		else
		{
			unitManager->OnMouseUp();
		}
	}
	else if (manageMode == ManageBuildings)
	{
		buildManager->OnMouseUp();
	}
	else if (manageMode == SpawnUnit)
	{
		buildManager->OnMouseUp();
	}*/

	unitManager->OnMouseUp();
	buildManager->OnMouseUp();
}

/// <summary>
/// Game loop
/// </summary>
void Game::Loop()
{
	Vector2 mouseWorldPosition = cameraManager->camera->MouseTo2DWorld();

	if (InputSystem::GetKeyDown(ESCAPE))
	{
		//SDL_SetRelativeMouseMode(SDL_FALSE);
	}

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

	if (InputSystem::GetKeyDown(MOUSE_LEFT))
	{
		startSelectionPos = mouseWorldPosition;
	}

	if (InputSystem::GetKeyDown(MOUSE_RIGHT))
	{
		startMousePosition = InputSystem::mousePosition;
	}

	if (InputSystem::GetKey(MOUSE_LEFT))
	{
		if (manageMode == ManageUnits)
			unitManager->SelectUnits();
	}

	if (InputSystem::GetKeyUp(MOUSE_RIGHT))
	{
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

	if (InputSystem::GetKeyDown(MOUSE_LEFT))
	{
		//tileMap->SetTile(round(mouseWorldPosition.x), round(mouseWorldPosition.y), 2);
	}

	MoveCursor();
	UpdateRessourcesText();
	UpdateModeText();
}