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

	Texture* textureEnv0 = new Texture("rts/Environment/scifiEnvironment_07.png");
	textureEnv0->SetWrapMode(Texture::Clamp);
	textureEnv0->SetPixelPerUnit(128);
	Texture* textureEnv1 = new Texture("rts/Environment/scifiEnvironment_17.png");
	textureEnv1->SetWrapMode(Texture::Clamp);
	textureEnv1->SetPixelPerUnit(128);

	cameraGameObject->AddExistingComponent(camera);
	camera->gameObject->transform.SetPosition(Vector3(0, 0, -10));

	gameObjectSprite->transform.SetPosition(Vector3(0, 0, 0));

	Material* material2D = new Material();
	material2D->shader = shaderStandard2D;

	//gameObjectTileMap->transform.SetRotation(Vector3(0,0,45));

	tileMap = new TileMap();
	gameObjectTileMap->AddExistingComponent(tileMap);
	tileMap->Setup(10, 10);
	tileMap->AddTexture(textureTile0);
	tileMap->AddTexture(textureTile1);
	for (int x = 0; x < 10; x++)
	{
		for (int y = 0; y < 10; y++)
		{
			tileMap->SetTile(x, y, 1);
		}
	}
	tileMap->SetTile(4, 4, 2);
	tileMap->SetTile(1, 0, 2);
	tileMap->SetTile(2, 5, 2);
	tileMap->SetTile(8, 5, 2);
	tileMap->SetTile(4, 8, 2);
	tileMap->material = material2D;

	//Props tilemap
	tileMapProps = new TileMap();
	gameObjectTileMap->AddExistingComponent(tileMapProps);
	tileMapProps->Setup(10, 10);
	tileMapProps->AddTexture(textureEnv0);
	tileMapProps->AddTexture(textureEnv1);
	tileMapProps->SetTile(4, 2, 1);
	tileMapProps->SetTile(8, 9, 2);
	tileMapProps->SetTile(3, 6, 2);
	tileMapProps->SetTile(7, 1, 2);
	tileMapProps->material = material2D;

	SpriteRenderer* spr = new SpriteRenderer(textureShip, material2D);
	gameObjectSprite->AddExistingComponent(spr);

	GameObject* t = new GameObject();
	//t->transform.SetPosition(Vector3(0.32f, 0.32f, 0));
	t->transform.SetPosition(Vector3(1, 0, 0));
	SpriteRenderer* spr2 = new SpriteRenderer(textureShip, material2D);
	t->AddExistingComponent(spr2);

	TextRenderer* textRenderer = new TextRenderer(UiManager::fonts[0], 5, shaderText);
	textRenderer->text = "Salut à tous les amissssss\nzefzefizeifb ezfibzef";
	gameObjectSprite->AddExistingComponent(textRenderer);


	//SDL_SetRelativeMouseMode(SDL_TRUE);
	SDL_SetRelativeMouseMode(SDL_FALSE);
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
	float aspect = static_cast<float>((Window::GetWidth()) / static_cast<float>(Window::GetHeight()));

	if (InputSystem::GetKey(MOUSE_RIGHT)) {
		Vector3 vect = Graphics::usedCamera->gameObject->transform.GetDown();
		//vect *= Time::GetDeltaTime() * InputSystem::mouseSpeed.y * cameraZoom/2.8f * 3;
		//vect *= Time::GetDeltaTime() * InputSystem::mouseSpeed.y / aspect;
		//vect *= InputSystem::mouseSpeed.y * aspect /128.0;
		vect *= InputSystem::mouseSpeed.y * 10 / aspect;
		newCameraPosition += vect;

		vect = Graphics::usedCamera->gameObject->transform.GetLeft();
		//vect *= Time::GetDeltaTime() * InputSystem::mouseSpeed.x * cameraZoom / 2.8f * 3;
		//vect *= Time::GetDeltaTime() * InputSystem::mouseSpeed.x / aspect;
		//vect *= InputSystem::mouseSpeed.x * aspect / 128.0;
		vect *= InputSystem::mouseSpeed.x* 10;
		newCameraPosition += vect;
	}

	if (InputSystem::GetKey(Z))
	{
		Vector3 vect = Graphics::usedCamera->gameObject->transform.GetUp();
		vect *= Time::GetDeltaTime() * 2;
		newCameraPosition += vect;
	}
	if (InputSystem::GetKey(S))
	{
		Vector3 vect = Graphics::usedCamera->gameObject->transform.GetDown();
		vect *= Time::GetDeltaTime() * 2;
		newCameraPosition += vect;
	}
	if (InputSystem::GetKey(D))
	{
		Vector3 vect = Graphics::usedCamera->gameObject->transform.GetRight();
		vect *= Time::GetDeltaTime() * 2;
		newCameraPosition += vect;
	}
	if (InputSystem::GetKey(Q))
	{

		Vector3 vect = Graphics::usedCamera->gameObject->transform.GetLeft();
		vect *= Time::GetDeltaTime() * 2;
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

		camera->SetProjectionSize(2.5f* cameraZoom);
	}

	camera->gameObject->transform.SetPosition(newCameraPosition);

	std::string debugText2 = std::string("Pos x: ") + std::to_string(cameraGameObject->transform.GetPosition().x) + " y: " + std::to_string(cameraGameObject->transform.GetPosition().y) + " z: " + std::to_string(cameraGameObject->transform.GetPosition().z);
	debugText2 += "Size " + std::to_string(camera->GetProjectionSize());
	//Debug::Print(debugText2);
	UiManager::RenderTextCanvas(debugText2, 0.0f, 0.1f, 0, 0.7f,0, Vector3(0.5f, 0.0f, 0.2f), UiManager::fonts[0], H_Right, V_Center, *AssetManager::GetShader(7));

	//gameObjectSprite->transform.SetRotation(Vector3(0, 0, gameObjectSprite->transform.GetRotation().z + Time::GetDeltaTime() * 10));
}