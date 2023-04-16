#include "game.h"
#include "../audio/audio_clip.h"
#include "../gameobject.h"
#include "../graphics/graphics.h"
#include "../graphics/mesh.h"
#include "../graphics/material.h"

#include <SDL2/SDL.h>
#include "../scene_manager/SceneManager.h"
#include "../game/sceneGame1.h"
#include "../lighting/lighting.h"
#include "../inputs/input_system.h"
#include "../engine_settings.h"
#include "../vectors/vector3.h"
#include <chrono>
#include <iostream>
#include "../graphics/spriteRenderer.h"
#include "../graphics/sprite_manager.h"
#include "../asset_manager.h"
#include "../debug/debug.h"
#include "../graphics/text_renderer.h"
#include "../ui/ui_manager.h"
#include "../tools/shape_spawner.h"
#include "../tools/curve.h"
#include "../tools/math.h"
#include "../tools/benchmark.h"
#include "../time/time.h"
#include "../ui/window.h"
#include "unit.h"
#include "unit_data.h"

/// <summary>
/// Init game
/// </summary>
void Game::Init()
{
	LoadGameData();

	//gameObjectTileMap->transform.SetLocalScale(Vector3(2, 1, 1));
	//gameObjectTileMap->transform.SetRotation(Vector3(0,0,45));

	GenerateMap();
	CreateTileMaps();

	cameraGameObject->AddExistingComponent(camera);
	camera->gameObject->transform.SetPosition(Vector3(0, 0, -10));

	gameObjectSprite->transform.SetPosition(Vector3(0, 0, 0));


	//SpriteRenderer* spr = new SpriteRenderer(textureShip, material2D);
	//gameObjectSprite->AddExistingComponent(spr);

	/*GameObject* t = new GameObject();
	t->transform.SetPosition(Vector3(1, 1, 0));
	SpriteRenderer* spr2 = new SpriteRenderer(textureShip, material2D);
	t->AddExistingComponent(spr2);

	GameObject* t2 = new GameObject();
	t2->transform.SetPosition(Vector3(2, 2, 0));
	SpriteRenderer* spr3 = new SpriteRenderer(textureShip, material2D);
	t2->AddExistingComponent(spr3);*/

	Unit* newUnit = new Unit(unitsData[0]);
	newUnit->updatePriority = 100;
	GameObject* unitGM = new GameObject("Unit");
	unitGM->transform.SetPosition(Vector3(20, 20, 0));
	unitGM->AddExistingComponent(newUnit);
	units.push_back(newUnit);

	/*GameObject* t3 = new GameObject();
	t3->transform.SetLocalPosition(Vector3(3, 3, 0));
	SpriteRenderer* spr4 = new SpriteRenderer(textureShip, material2D);
	t3->AddExistingComponent(spr4);*/

	gameObjectCrosshair = new GameObject();
	gameObjectCrosshair->transform.SetPosition(Vector3(0, 0, 0));
	SpriteRenderer* spr5 = new SpriteRenderer(crosshair, material2D);
	spr5->color = Vector4(0, 0, 0, 0.2f);
	gameObjectCrosshair->AddExistingComponent(spr5);

	/*TextRenderer* textRenderer = new TextRenderer(UiManager::fonts[0], 5, shaderText);
	textRenderer->text = "Salut à tous les amissssss\nzefzefizeifb ezfibzef";
	gameObjectSprite->AddExistingComponent(textRenderer);*/


	//SDL_SetRelativeMouseMode(SDL_TRUE);
	SDL_SetRelativeMouseMode(SDL_FALSE);
	camera->SetProjectionType(Orthographic);
	camera->SetProjectionSize(2.5f * cameraZoom);
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

	SceneGame1* scene = new SceneGame1();
	SceneManager::LoadScene(scene);

	textureShip = new Texture("ship_0000.png", "Ship");
	Texture* textureTile0 = new Texture("rts/Tile/scifiTile_42.png", "Ground0");
	textureTile0->SetPixelPerUnit(128);
	Texture* textureTile1 = new Texture("rts/Tile/scifiTile_30.png", "Ground1");
	textureTile1->SetPixelPerUnit(128);
	crosshair = new Texture("rts/crosshairs/crosshair.png", "Crosshair");
	crosshair->SetPixelPerUnit(128);

	tilesTextures.push_back(textureTile0);
	tilesTextures.push_back(textureTile1);

	for (int i = 0; i < 19; i++)
	{
		std::string propTextureName = "rts/Environment/scifiEnvironment_" + std::to_string(i + 1) + ".png";
		Texture* textureEnv = new Texture(propTextureName, "", Texture::Bilinear, true);
		propsTextures.push_back(textureEnv);
	}

	UnitData* newUnitData = new UnitData(0);
	unitsData.push_back(newUnitData);

	int propsTexturesCount = propsTextures.size();
	for (int i = 0; i < propsTexturesCount; i++)
	{
		propsTextures[i]->SetWrapMode(Texture::Clamp);
		propsTextures[i]->SetPixelPerUnit(128);
	}

	material2D = new Material("2D Standard");
	material2D->shader = shaderStandard2D;
	//material2D->SetAttribut("color", Vector4(1, 0, 1,1));
}


/// <summary>
/// Game loop
/// </summary>
void Game::Loop()
{
	if (InputSystem::GetKeyDown(ESCAPE))
	{
		//SDL_SetRelativeMouseMode(SDL_FALSE);
	}

	Vector3 newCameraPosition = camera->gameObject->transform.GetPosition();

	if (InputSystem::GetKey(MOUSE_RIGHT))
	{
		Vector3 vect = Graphics::usedCamera->gameObject->transform.GetDown();
		vect *= InputSystem::mouseSpeed.y * 14.2 * cameraZoom / 2.8f;
		newCameraPosition += vect;

		vect = Graphics::usedCamera->gameObject->transform.GetLeft();
		vect *= InputSystem::mouseSpeed.x * 14.2 * cameraZoom / 2.8f;
		newCameraPosition += vect;
	}
	Vector2 mouseWorldPosition = camera->MouseTo2DWorld();

	//Move cursor
	cursorPosition.x = Math::Lerp(cursorPosition.x, round(mouseWorldPosition.x), Time::GetUnscaledDeltaTime() * 20);
	cursorPosition.y = Math::Lerp(cursorPosition.y, round(mouseWorldPosition.y), Time::GetUnscaledDeltaTime() * 20);
	gameObjectCrosshair->transform.SetPosition(Vector3(cursorPosition.x, cursorPosition.y, 0));

	if (InputSystem::GetKeyDown(MOUSE_LEFT))
	{
		tileMap->SetTile(round(mouseWorldPosition.x), round(mouseWorldPosition.y), 2);
	}

	if (InputSystem::GetKey(Z))
	{
		Vector3 vect = Graphics::usedCamera->gameObject->transform.GetUp();
		vect *= Time::GetDeltaTime() * cameraArrowMoveSpeed;
		newCameraPosition += vect;
	}
	if (InputSystem::GetKey(S))
	{
		Vector3 vect = Graphics::usedCamera->gameObject->transform.GetDown();
		vect *= Time::GetDeltaTime() * cameraArrowMoveSpeed;
		newCameraPosition += vect;
	}
	if (InputSystem::GetKey(D))
	{
		Vector3 vect = Graphics::usedCamera->gameObject->transform.GetRight();
		vect *= Time::GetDeltaTime() * cameraArrowMoveSpeed;
		newCameraPosition += vect;
	}
	if (InputSystem::GetKey(Q))
	{
		Vector3 vect = Graphics::usedCamera->gameObject->transform.GetLeft();
		vect *= Time::GetDeltaTime() * cameraArrowMoveSpeed;
		newCameraPosition += vect;
	}

	if (InputSystem::mouseWheel != 0)
	{
		cameraZoom -= InputSystem::mouseWheel / 3.0f;
		if (cameraZoom < 1)
		{
			cameraZoom = 1;
		}
		else if (cameraZoom > 2.8f)
		{
			cameraZoom = 2.8f;
		}

		camera->SetProjectionSize(2.5f * cameraZoom);
	}

	camera->gameObject->transform.SetPosition(newCameraPosition);


	//SpriteManager::Render2DLine(Vector2(0, 0), Vector2(2, -2), 1, lineColor, material2D);

	//std::string debugText2 = std::string("Pos x: ") + std::to_string(cameraGameObject->transform.GetPosition().x) + " y: " + std::to_string(cameraGameObject->transform.GetPosition().y) + " z: " + std::to_string(cameraGameObject->transform.GetPosition().z);
	//debugText2 += "Size " + std::to_string(camera->GetProjectionSize());
	//Debug::Print(debugText2);
	//UiManager::RenderTextCanvas(debugText2, 0.0f, 0.1f, 0, 0.7f, 0, Vector3(0.5f, 0.0f, 0.2f), UiManager::fonts[0], H_Right, V_Center, *AssetManager::GetShader(7));

	//gameObjectSprite->transform.SetRotation(Vector3(0, 0, gameObjectSprite->transform.GetRotation().z + Time::GetDeltaTime() * 10));
}

Game::Tile* Game::GetTile(int x, int y)
{
	if (tiles == nullptr)
		return nullptr;

	return &tiles[x * mapSize + y];
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
}

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

void Game::CreateTileMaps()
{
	//Create ground tile map
	tileMap = new TileMap(material2D);
	gameObjectTileMap->AddExistingComponent(tileMap);
	tileMap->Setup(mapSize, mapSize);
	tileMap->AddTexture(tilesTextures[0]);
	tileMap->AddTexture(tilesTextures[1]);

	//Create props tilemap
	tileMapProps = new TileMap(material2D);
	gameObjectTileMap->AddExistingComponent(tileMapProps);
	tileMapProps->Setup(mapSize, mapSize);

	int propsTexturesCount = propsTextures.size();
	for (int i = 0; i < propsTexturesCount; i++)
	{
		tileMapProps->AddTexture(propsTextures[i]);
	}

	for (int x = 0; x < mapSize; x++)
	{
		for (int y = 0; y < mapSize; y++)
		{
			Tile* tile = GetTile(x, y);
			tileMap->SetTile(x, y, tile->groundTileId);
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