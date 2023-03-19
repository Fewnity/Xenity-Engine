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
#include "../asset_manager.h"
#include "../debug/debug.h"
#include "../graphics/text_renderer.h"
#include "../ui/ui_manager.h"
#include "../tools/shape_spawner.h"
#include "../tools/curve.h"
#include "../tools/benchmark.h"
#include "../time/time.h"
#include "../ui/window.h"
using namespace std::chrono;

/// <summary>
/// Init game
/// </summary>
void Game::Init()
{
	Shader* shader = new Shader("vertexStandard.shader", "fragmentStandard.shader");

	Shader* shader3 = new Shader("3D/vStandard.shader", "3D/fStandard.shader");

	Shader* shader2 = new Shader("vertex2.shader", "fragment2.shader");
	Shader* shaderText = new Shader("vertexText.shader", "fragmentText.shader");
	Shader* shaderTextCanvas = new Shader("vertexTextCanvas.shader", "fragmentTextCanvas.shader");
	Shader* shader2D = new Shader("vertex2D.shader", "fragment2D.shader");
	Shader* shaderStandard2D = new Shader("vertexStandard2D.shader", "fragmentStandard2D.shader");

	SceneGame1* scene = new SceneGame1();
	SceneManager::LoadScene(scene);

	Texture* textureShip = new Texture("ship_0000.png");
	Texture* textureTile0 = new Texture("rts/Tile/scifiTile_42.png");
	textureTile0->SetPixelPerUnit(128);
	Texture* textureTile1 = new Texture("rts/Tile/scifiTile_30.png");
	textureTile1->SetPixelPerUnit(128);

	for (int i = 0; i < 19; i++)
	{
		std::string propTextureName = "rts/Environment/scifiEnvironment_" + std::to_string(i + 1) + ".png";
		Texture* textureEnv = new Texture(propTextureName, Texture::Bilinear, true);
		propsTextures.push_back(textureEnv);
	}

	int propsTexturesCount = propsTextures.size();
	for (int i = 0; i < propsTexturesCount; i++)
	{
		propsTextures[i]->SetWrapMode(Texture::Clamp);
		propsTextures[i]->SetPixelPerUnit(128);
	}

	cameraGameObject->AddExistingComponent(camera);
	camera->gameObject->transform.SetPosition(Vector3(0, 0, -10));

	gameObjectSprite->transform.SetPosition(Vector3(0, 0, 0));

	Material* material2D = new Material();
	material2D->shader = shaderStandard2D;

	//gameObjectTileMap->transform.SetRotation(Vector3(0,0,45));

	GenerateMap();

	tileMap = new TileMap();
	gameObjectTileMap->AddExistingComponent(tileMap);
	tileMap->Setup(mapSize, mapSize);
	tileMap->AddTexture(textureTile0);
	tileMap->AddTexture(textureTile1);
	for (int x = 0; x < mapSize; x++)
	{
		for (int y = 0; y < mapSize; y++)
		{
			tileMap->SetTile(x, y, 1);
		}
	}
	tileMap->material = material2D;

	//Props tilemap
	tileMapProps = new TileMap();
	gameObjectTileMap->AddExistingComponent(tileMapProps);
	tileMapProps->Setup(mapSize, mapSize);
	for (int i = 0; i < propsTexturesCount; i++)
	{
		tileMapProps->AddTexture(propsTextures[i]);
	}
	//tileMapProps->AddTexture(textureEnv0);
	//tileMapProps->AddTexture(textureEnv1);

	int propCount = props.size();
	for (int i = 0; i < propCount; i++)
	{
		tileMapProps->SetTile(props[i]->x, props[i]->y, props[i]->id);
	}

	tileMapProps->material = material2D;

	/*SpriteRenderer* spr = new SpriteRenderer(textureShip, material2D);
	gameObjectSprite->AddExistingComponent(spr);

	GameObject* t = new GameObject();
	t->transform.SetPosition(Vector3(1, 0, 0));
	SpriteRenderer* spr2 = new SpriteRenderer(textureShip, material2D);
	t->AddExistingComponent(spr2);

	TextRenderer* textRenderer = new TextRenderer(UiManager::fonts[0], 5, shaderText);
	textRenderer->text = "Salut à tous les amissssss\nzefzefizeifb ezfibzef";
	gameObjectSprite->AddExistingComponent(textRenderer);*/


	//SDL_SetRelativeMouseMode(SDL_TRUE);
	SDL_SetRelativeMouseMode(SDL_FALSE);
	camera->SetProjectionSize(2.5f * cameraZoom);
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

	if (InputSystem::GetKey(MOUSE_RIGHT)) {
		Vector3 vect = Graphics::usedCamera->gameObject->transform.GetDown();
		//vect *= InputSystem::mouseSpeed.y * 10;
		vect *= InputSystem::mouseSpeed.y * 14.2 * cameraZoom / 2.8f;
		newCameraPosition += vect;

		vect = Graphics::usedCamera->gameObject->transform.GetLeft();
		//vect *= InputSystem::mouseSpeed.x * 10;
		vect *= InputSystem::mouseSpeed.x * 14.2 * cameraZoom / 2.8f;
		newCameraPosition += vect;
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

	std::string debugText2 = std::string("Pos x: ") + std::to_string(cameraGameObject->transform.GetPosition().x) + " y: " + std::to_string(cameraGameObject->transform.GetPosition().y) + " z: " + std::to_string(cameraGameObject->transform.GetPosition().z);
	debugText2 += "Size " + std::to_string(camera->GetProjectionSize());
	//Debug::Print(debugText2);
	UiManager::RenderTextCanvas(debugText2, 0.0f, 0.1f, 0, 0.7f, 0, Vector3(0.5f, 0.0f, 0.2f), UiManager::fonts[0], H_Right, V_Center, *AssetManager::GetShader(7));

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

	//Fill map data

	//Add trees
	int treeCount = minTreeCount + rand() % (maxTreeCount - minTreeCount + 1);
	for (int i = 0; i < treeCount; i++)
	{
		Prop* newProp = new Prop();
		newProp->id = 16 + rand() % 4;
		newProp->SetPosition(rand() % mapSize, rand() % mapSize);
		props.push_back(newProp);
	}

	//Add rocks
	int rockCount = minRockCount + rand() % (maxRockCount - minRockCount + 1);
	for (int i = 0; i < rockCount; i++)
	{
		Prop* newProp = new Prop();
		if (rand() % 2 == 0)
			newProp->id = 2 + rand() % 4;
		else
			newProp->id = 8 + rand() % 4;

		newProp->SetPosition(rand() % mapSize, rand() % mapSize);
		props.push_back(newProp);
	}

	//Add emeralds
	int emeraldCount = minRockEmeraldCount + rand() % (maxRockEmeraldCount - minRockEmeraldCount + 1);
	for (int i = 0; i < emeraldCount; i++)
	{
		Prop* newProp = new Prop();
		if (rand() % 2 == 0)
			newProp->id = 6;
		else
			newProp->id = 12;
		newProp->SetPosition(rand() % mapSize, rand() % mapSize);
		props.push_back(newProp);
	}

	//Add gold
	int goldCount = minRockGoldCount + rand() % (maxRockGoldCount - minRockGoldCount + 1);
	for (int i = 0; i < goldCount; i++)
	{
		Prop* newProp = new Prop();
		if (rand() % 2 == 0)
			newProp->id = 7;
		else
			newProp->id = 13;
		newProp->SetPosition(rand() % mapSize, rand() % mapSize);
		props.push_back(newProp);
	}

	//Add crystal
	int crystalCount = minCrystalCount + rand() % (maxCrystalCount - minCrystalCount + 1);
	for (int i = 0; i < crystalCount; i++)
	{
		Prop* newProp = new Prop();
		newProp->id = 14 + rand() % 2;
		newProp->SetPosition(rand() % mapSize, rand() % mapSize);
		props.push_back(newProp);
	}
}
