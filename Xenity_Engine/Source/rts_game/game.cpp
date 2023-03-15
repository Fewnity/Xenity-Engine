#include "game.h"
#include "../audio/audio_clip.h"
#include "../gameobject.h"
#include "../graphics/graphics.h"
#include "../graphics/mesh.h"
#include "../graphics/material.h"
#include "../graphics/camera.h"
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
#include "../orbital_camera.h"
#include "../time/time.h"
#include "../graphics/tile_map.h"

using namespace std::chrono;

GameObject* cameraGameObject = new GameObject("cameraGameObject");

Camera* camera = new Camera();

GameObject* gameObjectSprite = new GameObject("gameObjectSprite");
GameObject* gameObjectTileMap = new GameObject("TileMap");
TileMap* tileMap;

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
	Texture* textureTile1 = new Texture("rts/Tile/scifiTile_30.png");

	cameraGameObject->AddExistingComponent(camera);
	camera->gameObject->transform.SetPosition(Vector3(0, 1, -10));

	gameObjectSprite->transform.SetPosition(Vector3(0, 0, 0));

	Material* material2D = new Material();
	material2D->shader = shaderStandard2D;

	tileMap = new TileMap();
	tileMap->Setup(200, 200);
	tileMap->SetTile(0, 0, 1);
	tileMap->SetTile(1, 1, 1);
	tileMap->SetTile(2, 2, 1);
	tileMap->AddTexture(textureTile0);
	tileMap->AddTexture(textureTile1);
	tileMap->material = material2D;
	gameObjectTileMap->AddExistingComponent(tileMap);

	SpriteRenderer* spr = new SpriteRenderer(textureShip, material2D);
	gameObjectSprite->AddExistingComponent(spr);

	GameObject* t = new GameObject();
	t->transform.SetPosition(Vector3(0.32f, 0.32f, 0));
	SpriteRenderer* spr2 = new SpriteRenderer(textureShip, material2D);
	t->AddExistingComponent(spr2);

	TextRenderer* textRenderer = new TextRenderer(UiManager::fonts[0], 5, shaderText);
	textRenderer->text = "Salut à tous les amissssss\nzefzefizeifb ezfibzef";
	gameObjectSprite->AddExistingComponent(textRenderer);


	SDL_SetRelativeMouseMode(SDL_TRUE);
}

/// <summary>
/// Game loop
/// </summary>
void Game::Loop()
{
	//std::cout << "coneGameobject World: " << coneGameobject->transform.GetScale().x << " " << coneGameobject->transform.GetScale().y << " " << coneGameobject->transform.GetScale().z << " " << std::endl;
	//std::cout << "coneGameobject Local: " << coneGameobject->transform.GetLocalScale().x << " " << coneGameobject->transform.GetLocalScale().y << " " << coneGameobject->transform.GetLocalScale().z << " " << std::endl;

	/*std::cout << "cubeGameObject World: " << cubeGameObject->transform.GetScale().x << " " << cubeGameObject->transform.GetScale().y << " " << cubeGameObject->transform.GetScale().z << " " << std::endl;
	std::cout << "cubeGameObject Local: " << cubeGameObject->transform.GetLocalScale().x << " " << cubeGameObject->transform.GetLocalScale().y << " " << cubeGameObject->transform.GetLocalScale().z << " " << std::endl;

	std::cout << "cubeGameObjectChild World: " << cubeGameObject->children[0]->transform.GetScale().x << " " << cubeGameObject->children[0]->transform.GetScale().y << " " << cubeGameObject->children[0]->transform.GetScale().z << " " << std::endl;
	std::cout << "cubeGameObjectChild Local: " << cubeGameObject->children[0]->transform.GetLocalScale().x << " " << cubeGameObject->children[0]->transform.GetLocalScale().y << " " << cubeGameObject->children[0]->transform.GetLocalScale().z << " " << std::endl;
	*/

	//std::cout << "cube World: " << cubeGameObject->transform.GetPosition().x << " " << cubeGameObject->transform.GetPosition().y << " " << cubeGameObject->transform.GetPosition().z << " " << std::endl;
	//std::cout << "cube Local: " << cubeGameObject->transform.GetLocalPosition().x << " " << cubeGameObject->transform.GetLocalPosition().y << " " << cubeGameObject->transform.GetLocalPosition().z << " " << std::endl;

	//std::cout << "cube child1 World: " << cubeGameObject->children[0]->transform.GetPosition().x << " " << cubeGameObject->children[0]->transform.GetPosition().y << " " << cubeGameObject->children[0]->transform.GetPosition().z << " " << std::endl;
	//std::cout << "cube child1 Local: " << cubeGameObject->children[0]->transform.GetLocalPosition().x << " " << cubeGameObject->children[0]->transform.GetLocalPosition().y << " " << cubeGameObject->children[0]->transform.GetLocalPosition().z << " " << std::endl;

	//std::cout << "cube child2 World: " << cubeGameObject->children[0]->children[0]->transform.GetPosition().x << " " << cubeGameObject->children[0]->children[0]->transform.GetPosition().y << " " << cubeGameObject->children[0]->children[0]->transform.GetPosition().z << " " << std::endl;
	//std::cout << "cube child2 Local: " << cubeGameObject->children[0]->children[0]->transform.GetLocalPosition().x << " " << cubeGameObject->children[0]->children[0]->transform.GetLocalPosition().y << " " << cubeGameObject->children[0]->children[0]->transform.GetLocalPosition().z << " " << std::endl;

	//std::cout << "cubeGameObject: " << cubeGameObject->transform.GetLocalScale().x << std::endl;

	if (InputSystem::GetKeyDown(ESCAPE))
	{
		SDL_SetRelativeMouseMode(SDL_FALSE);
	}

	if (InputSystem::GetKeyDown(A))
	{
		EngineSettings::isWireframe = !EngineSettings::isWireframe;
	}

	if (InputSystem::GetKeyDown(W))
	{
	}
	if (InputSystem::GetKeyDown(X))
	{
	}
	if (InputSystem::GetKeyDown(C))
	{
	}

	Vector3 newCameraPosition = camera->gameObject->transform.GetPosition();
	if (InputSystem::GetKey(Z))
	{
		Vector3 vect = Graphics::usedCamera->gameObject->transform.GetUp();
		//Vector3 vect = Graphics::usedCamera->gameObject->transform.GetUp();
		vect *= Time::GetDeltaTime() * 2;
		newCameraPosition += vect;

	}
	if (InputSystem::GetKey(S)) {
		Vector3 vect = Graphics::usedCamera->gameObject->transform.GetDown();
		vect *= Time::GetDeltaTime() * 2;
		newCameraPosition += vect;
	}
	if (InputSystem::GetKey(D)) {

		Vector3 vect = Graphics::usedCamera->gameObject->transform.GetRight();
		vect *= Time::GetDeltaTime() * 2;
		newCameraPosition += vect;
	}
	if (InputSystem::GetKey(Q)) {

		Vector3 vect = Graphics::usedCamera->gameObject->transform.GetLeft();
		vect *= Time::GetDeltaTime() * 2;
		newCameraPosition += vect;
	}
	camera->gameObject->transform.SetPosition(newCameraPosition);

	if (InputSystem::GetKey(RIGHT))
	{
	}
	if (InputSystem::GetKey(LEFT))
	{
	}
	if (InputSystem::GetKey(UP))
	{
	}
	if (InputSystem::GetKey(DOWN))
	{
	}

	if (InputSystem::GetKey(Z))
	{
	}
	if (InputSystem::GetKey(S)) 
	{
	}
	if (InputSystem::GetKey(D)) 
	{
	}
	if (InputSystem::GetKey(Q)) 
	{
	}
	if (InputSystem::GetKey(P))
	{
	}
	if (InputSystem::GetKey(M))
	{
	}

	if (InputSystem::GetKeyDown(SPACE))
	{
		
	}

	//std::cout << cubeChild->transform.GetLocalPosition().x << std::endl;



	/*std::string debugText = std::string("Cube0 Position x:") + std::to_string(cubeGameObject->transform.GetPosition().x) + " y:" + std::to_string(cubeGameObject->transform.GetPosition().y) + " z:" + std::to_string(cubeGameObject->transform.GetPosition().z);
	debugText += std::string("\nCube0 Local Position x:") + std::to_string(cubeGameObject->transform.GetLocalPosition().x) + " y:" + std::to_string(cubeGameObject->transform.GetLocalPosition().y) + " z:" + std::to_string(cubeGameObject->transform.GetLocalPosition().z);

	debugText += std::string("\nCube0 Rotation x:") + std::to_string(cubeGameObject->transform.GetRotation().x) + " y:" + std::to_string(cubeGameObject->transform.GetRotation().y) + " z:" + std::to_string(cubeGameObject->transform.GetRotation().z);
	debugText += std::string("\nCube0 Local Rotation x:") + std::to_string(cubeGameObject->transform.GetLocalRotation().x) + " y:" + std::to_string(cubeGameObject->transform.GetLocalRotation().y) + " z:" + std::to_string(cubeGameObject->transform.GetLocalRotation().z);

	debugText += std::string("\nCube0 Scale x:") + std::to_string(cubeGameObject->transform.GetScale().x) + " y:" + std::to_string(cubeGameObject->transform.GetScale().y) + " z:" + std::to_string(cubeGameObject->transform.GetScale().z);
	debugText += std::string("\nCube0 Local Scale x:") + std::to_string(cubeGameObject->transform.GetLocalScale().x) + " y:" + std::to_string(cubeGameObject->transform.GetLocalScale().y) + " z:" + std::to_string(cubeGameObject->transform.GetLocalScale().z);

	debugText += std::string("\n\nCube1 Position x:") + std::to_string(cubeChild->transform.GetPosition().x) + " y:" + std::to_string(cubeChild->transform.GetPosition().y) + " z:" + std::to_string(cubeChild->transform.GetPosition().z);
	debugText += std::string("\nCube1 Local Position x:") + std::to_string(cubeChild->transform.GetLocalPosition().x) + " y:" + std::to_string(cubeChild->transform.GetLocalPosition().y) + " z:" + std::to_string(cubeChild->transform.GetLocalPosition().z);

	debugText += std::string("\nCube1 Rotation x:") + std::to_string(cubeChild->transform.GetRotation().x) + " y:" + std::to_string(cubeChild->transform.GetRotation().y) + " z:" + std::to_string(cubeChild->transform.GetRotation().z);
	debugText += std::string("\nCube1 Local Rotation x:") + std::to_string(cubeChild->transform.GetLocalRotation().x) + " y:" + std::to_string(cubeChild->transform.GetLocalRotation().y) + " z:" + std::to_string(cubeChild->transform.GetLocalRotation().z);

	debugText += std::string("\nCube1 Scale x:") + std::to_string(cubeChild->transform.GetScale().x) + " y:" + std::to_string(cubeChild->transform.GetScale().y) + " z:" + std::to_string(cubeChild->transform.GetScale().z);
	debugText += std::string("\nCube1 Local Scale x:") + std::to_string(cubeChild->transform.GetLocalScale().x) + " y:" + std::to_string(cubeChild->transform.GetLocalScale().y) + " z:" + std::to_string(cubeChild->transform.GetLocalScale().z);

	UiManager::RenderTextCanvas(debugText, 0.0f, 50, 20, 0.5f, 16, Vector3(0.5f, 0.0f, 0.2f), UiManager::fonts[0], HorizontalAlignment::H_Left, *AssetManager::GetShader(7));*/

	//std::string debugText2 = std::string("Cube1 Rotation x:") + std::to_string(cubeGameObject->children[0]->transform.GetRotation().x) + " y:" + std::to_string(cubeGameObject->children[0]->transform.GetRotation().y) + " z:" + std::to_string(cubeGameObject->children[0]->transform.GetRotation().z);
	//UiManager::RenderTextCanvas(debugText2, 0.0f, 80, 20, 0.5f,16, Vector3(0.5f, 0.0f, 0.2f), UiManager::fonts[0], HorizontalAlignment::H_Left, *AssetManager::GetShader(7));

	gameObjectSprite->transform.SetRotation(Vector3(0, 0, gameObjectSprite->transform.GetRotation().z + Time::GetDeltaTime() * 10));

	//mesh4->gameObject->transform.SetRotation(mesh4NewRotation);
}