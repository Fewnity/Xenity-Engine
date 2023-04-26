#include "game.h"
#include "../xenity.h"
#include <SDL2/SDL.h>
#include <chrono>
#include <iostream>


#include "unit.h"
#include "unit_data.h"
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_sdl2.h>
#include "../pathfinding/astar.h"
#include "camera_manager.h"
#include "unit_manager.h"

/// <summary>
/// Init game
/// </summary>
void Game::Init()
{
	LoadGameData();

	//gameObjectTileMap->transform.SetLocalScale(Vector3(2, 1, 1));
	//gameObjectTileMap->transform.SetRotation(Vector3(0,0,45));
	gameObjectTileMap->transform.SetPosition(Vector3(0, 0, -1));

	GenerateMap();
	CreateTileMaps();
	GameObject* managersGameObject = new GameObject("Managers");
	cameraManager = new CameraManager();
	cameraManager->Init();
	unitManager = new UnitManager();
	unitManager->cameraManager = cameraManager;
	unitManager->game = this;
	unitManager->LoadUnitData();
	managersGameObject->AddExistingComponent(cameraManager);
	managersGameObject->AddExistingComponent(unitManager);

	unitManager->SpawnUnits();

	gameObjectCrosshair2->transform.SetPosition(Vector3(0, 0, 0));
	gameObjectCrosshair2->SetActive(false);
	SpriteRenderer* sprGrad = new SpriteRenderer(gradient, material2DWithZ);
	sprGrad->color = Vector4(1, 1, 1, 1);
	gameObjectCrosshair2->AddExistingComponent(sprGrad);

	gameObjectCrosshair->transform.SetPosition(Vector3(0, 0, 0));
	SpriteRenderer* spr5 = new SpriteRenderer(crosshair, material2DWithZ);
	spr5->color = Vector4(0, 0, 0, 0.2f);
	gameObjectCrosshair->AddExistingComponent(spr5);

	lineRendererTop = new LineRenderer(0.1f, material2D);
	lineRendererBottom = new LineRenderer(0.1f, material2D);
	lineRendererLeft = new LineRenderer(0.1f, material2D);
	lineRendererRight = new LineRenderer(0.1f, material2D);
	Vector4 selectionLineColor = Vector4(0, 0, 0, 0.2f);
	lineRendererTop->color = selectionLineColor;
	lineRendererBottom->color = selectionLineColor;
	lineRendererLeft->color = selectionLineColor;
	lineRendererRight->color = selectionLineColor;

	gameObjectLineRenderers->AddExistingComponent(lineRendererTop);
	gameObjectLineRenderers->AddExistingComponent(lineRendererBottom);
	gameObjectLineRenderers->AddExistingComponent(lineRendererLeft);
	gameObjectLineRenderers->AddExistingComponent(lineRendererRight);

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
}

void Game::LoadGameData()
{
	Shader* shader = new Shader("vertexStandard.shader", "fragmentStandard.shader");

	Shader* shader3 = new Shader("3D/vStandard.shader", "3D/fStandard.shader");

	Shader* shader2 = new Shader("vertex2.shader", "fragment2.shader");
	Shader* shaderText = new Shader("vertexText.shader", "fragmentText.shader");
	Shader* shaderTextCanvas = new Shader("vertexTextCanvas.shader", "fragmentTextCanvas.shader");
	Shader* shader2D = new Shader("vertex2D.shader", "fragment2D.shader");
	Shader* shaderStandard2D = new Shader("vertexStandard2D.shader", "fragmentStandard2D.shader");
	Shader* shaderStandard2DText = new Shader("vertexStandard2DText.shader", "fragmentStandard2DText.shader");
	Shader* shaderStandard2DWithZ = new Shader("vertexStandard2DZ.shader", "fragmentStandard2DZ.shader");

	//SceneGame1* scene = new SceneGame1();
	//SceneManager::LoadScene(scene);

	textureShip = new Texture("ship_0000.png", "Ship");
	Texture* textureTile0 = new Texture("rts/Tile/scifiTile_42.png", "Ground0");
	textureTile0->SetPixelPerUnit(128);
	Texture* textureTile1 = new Texture("rts/Tile/scifiTile_30.png", "Ground1");
	textureTile1->SetPixelPerUnit(128);
	crosshair = new Texture("rts/crosshairs/crosshair.png", "Crosshair");
	crosshair->SetPixelPerUnit(128);


	gradient = new Texture("gradient.png", "gradient");

	tilesTextures.push_back(textureTile0);
	tilesTextures.push_back(textureTile1);

	for (int i = 0; i < 19; i++)
	{
		std::string propTextureName = "rts/Environment/scifiEnvironment_" + std::to_string(i + 1) + ".png";
		Texture* textureEnv = new Texture(propTextureName, "", Texture::Bilinear, true);
		propsTextures.push_back(textureEnv);
	}


	int propsTexturesCount = propsTextures.size();
	for (int i = 0; i < propsTexturesCount; i++)
	{
		propsTextures[i]->SetWrapMode(Texture::Clamp);
		propsTextures[i]->SetPixelPerUnit(128);
	}

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


void Game::MoveCursor()
{
	Vector2 mouseWorldPosition = cameraManager->camera->MouseTo2DWorld();

	gameObjectCrosshair2->transform.SetPosition(Vector3(mouseWorldPosition.x, mouseWorldPosition.y, 1));

	//Move cursor
	cursorPosition.x = Math::Lerp(cursorPosition.x, round(mouseWorldPosition.x), Time::GetUnscaledDeltaTime() * 20);
	cursorPosition.y = Math::Lerp(cursorPosition.y, round(mouseWorldPosition.y), Time::GetUnscaledDeltaTime() * 20);
	gameObjectCrosshair->transform.SetPosition(Vector3(cursorPosition.x, cursorPosition.y, 0));
}

void Game::OnMouseUp()
{
	Vector2 mouseWorldPosition = cameraManager->camera->MouseTo2DWorld();
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
		unitManager->SelectUnits();
	}

	if (InputSystem::GetKeyUp(MOUSE_RIGHT))
	{
		Vector2 endMousePosition = InputSystem::mousePosition;
		float dis = Vector2::Distance(startMousePosition, endMousePosition);

		if (dis <= 4)
		{
			unitManager->UnselectAllUnits();
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
}

Game::Tile* Game::GetTile(int x, int y)
{
	if (tiles == nullptr)
		return nullptr;

	return &tiles[x * mapSize + y];
}

int Game::GetDrawPriority()
{
	return 0;
}

void Game::GenerateMap()
{
	if (tiles)
		free(tiles);

	tiles = (Tile*)malloc(mapSize * mapSize * sizeof(Tile));
	for (int x = 0; x < mapSize; x++)
	{
		for (int y = 0; y < mapSize; y++)
		{
			Tile* tile = GetTile(x, y);
			tile->groundTileId = 1;
			tile->prop = nullptr;
		}
	}

	if (astar)
		delete astar;

	astar = new Astar(mapSize, mapSize);
	astar->canPassCorners = false;
	//Fill map data

	//Add trees
	int treeCount = minTreeCount + rand() % (maxTreeCount - minTreeCount + 1);
	for (int i = 0; i < treeCount; i++)
	{
		int id = 16 + rand() % 4;
		CreateProp(id);
	}

	//Add rocks
	int rockCount = minRockCount + rand() % (maxRockCount - minRockCount + 1);
	for (int i = 0; i < rockCount; i++)
	{
		int id = 0;
		if (rand() % 2 == 0)
			id = 2 + rand() % 4;
		else
			id = 8 + rand() % 4;
		CreateProp(id);
	}

	//Add emeralds
	int emeraldCount = minRockEmeraldCount + rand() % (maxRockEmeraldCount - minRockEmeraldCount + 1);
	for (int i = 0; i < emeraldCount; i++)
	{
		int id = 0;
		if (rand() % 2 == 0)
			id = 6;
		else
			id = 12;
		CreateProp(id);
	}

	//Add gold
	int goldCount = minRockGoldCount + rand() % (maxRockGoldCount - minRockGoldCount + 1);
	for (int i = 0; i < goldCount; i++)
	{
		int id = 0;
		if (rand() % 2 == 0)
			id = 7;
		else
			id = 13;
		CreateProp(id);
	}

	//Add crystal
	int crystalCount = minCrystalCount + rand() % (maxCrystalCount - minCrystalCount + 1);
	for (int i = 0; i < crystalCount; i++)
	{
		int id = 14 + rand() % 2;
		CreateProp(id);
	}

	for (int x = 0; x < mapSize; x++)
	{
		for (int y = 0; y < mapSize; y++)
		{
			Tile* tile = GetTile(x, y);
			if (tile->prop)
			{
				astar->SetTileIsObstacle(x, y, true);
			}
		}
	}
}

/// <summary>
/// Create a new prop in a random position
/// </summary>
/// <param name="id">Prop id</param>
/// <returns>Created prop</returns>
Prop* Game::CreateProp(int id)
{
	int x;
	int y;
	do
	{
		x = rand() % mapSize;
		y = rand() % mapSize;
	} while (GetTile(x, y)->prop != nullptr);

	Prop* newProp = new Prop();
	newProp->id = id;

	Tile* tile = GetTile(x, y);
	tile->prop = newProp;

	return newProp;
}

/// <summary>
/// Create tilemaps, 
/// </summary>
void Game::CreateTileMaps()
{
	//Create ground tile map
	tileMap = new TileMap(material2DWithZ);
	gameObjectTileMap->AddExistingComponent(tileMap);
	tileMap->Setup(mapSize, mapSize);
	tileMap->AddTexture(tilesTextures[0]);
	tileMap->AddTexture(tilesTextures[1]);

	//Create props tilemap
	tileMapProps = new TileMap(material2DWithZ);
	tileMapProps->layerOrder = 1;
	gameObjectTileMap->AddExistingComponent(tileMapProps);
	tileMapProps->Setup(mapSize, mapSize);

	int propsTexturesCount = propsTextures.size();
	for (int i = 0; i < propsTexturesCount; i++)
	{
		tileMapProps->AddTexture(propsTextures[i]);
	}

	//Fill tilemaps
	for (int x = 0; x < mapSize; x++)
	{
		for (int y = 0; y < mapSize; y++)
		{
			Tile* tile = GetTile(x, y);
			tileMap->SetTile(x, y, tile->groundTileId);

			//Add prop
			if (tile->prop != nullptr)
			{
				tileMapProps->SetTile(x, y, tile->prop->id);
			}
		}
	}

	tileMap->SetTile(1, 0, 2);
	tileMap->SetTile(4, 2, 2);
	tileMap->SetTile(4, 4, 2);
	tileMap->SetTile(7, 1, 2);
}