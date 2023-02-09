#include "game.h"
#include "audio/audio_clip.h"
#include "gameobject.h"
#include "graphics/graphics.h"
#include "graphics/mesh.h"
#include "graphics/camera.h"
#include <SDL2/SDL.h>
#include "scene_manager/SceneManager.h"
#include "game/sceneGame1.h"
#include "lighting/lighting.h"
#include "inputs/input_system.h"
#include "engine_settings.h"
#include<chrono>
#include <iostream>
#include "graphics/spriteRenderer.h"
#include "asset_manager.h"
#include "debug.h"

using namespace std::chrono;

GameObject* cameraGameObject = new GameObject();
Camera* camera = new Camera();

GameObject* cubeGameObject = new GameObject();
GameObject* coneGameobject = new GameObject();
GameObject* myGameObject3 = new GameObject();

GameObject* gameObjectSprite = new GameObject();

Mesh* mesh3 = nullptr;
Mesh* mesh4 = nullptr;
Mesh* mesh5 = nullptr;
float animation = 0;

void Game::Init() {
	//loadedScenes.push_back(new Scene());
	//usedScene = loadedScenes[0];
	//AudioClip audio1 = AudioClip("camera-13695.mp3");
	//AudioSource::Play2DSound(audio1);
	//AudioSource::Play3DSound(audio1, Vector3(0, 0, 2));

	Shader* shader = new Shader("vertexStandard.shader", "fragmentStandard.shader");
	Shader* shader3 = new Shader("vertexStandard.shader", "fragmentStandard.shader");
	Shader* shader2 = new Shader("vertex2.shader", "fragment2.shader");
	Shader* shaderText = new Shader("vertexText.shader", "fragmentText.shader");
	Shader* shader2D = new Shader("vertex2D.shader", "fragment2D.shader");
	Shader* shaderStandard2D = new Shader("vertexStandard2D.shader", "fragmentStandard2D.shader");

	Texture* texture1 = new Texture("Brick.png");
	Texture* texture2 = new Texture("Dry Dirt.png");
	//Wood and metal 1
	Texture* texture3 = new Texture("container.png");
	Texture* texture4 = new Texture("container_specular.png");
	//Wood and metal 2
	Texture* texture5 = new Texture("Wood_Gate_Fortified_basecolor.jpg");
	Texture* texture6 = new Texture("Wood_Gate_Fortified_metallic.jpg");
	Texture* texture7 = new Texture("ship_0000.png", Texture::Filter::Point, false);

	SceneGame1* scene = new SceneGame1();
	SceneManager::LoadScene(scene);

	cameraGameObject->AddExistingComponent(camera);
	camera->gameObject->transform.SetPosition(Vector3(0, 2, 2));

	/*time_point<high_resolution_clock> start_point, end_point;
	start_point = high_resolution_clock::now();

	for (int i = 0; i < 10000; i++)
	{

	}

	end_point = high_resolution_clock::now();
	auto start = time_point_cast<microseconds>(start_point).time_since_epoch().count();
	auto end = time_point_cast<microseconds>(end_point).time_since_epoch().count();
	std::cout << "Time taken = " << (end - start) << " microseconds" << std::endl;*/

	mesh3 = new Mesh("CubeTriangulate.obj");
	mesh4 = new Mesh("ConeTriangulate.obj");
	mesh5 = new Mesh("CubeTriangulate.obj");

	cubeGameObject->AddExistingComponent(mesh3);
	cubeGameObject->transform.SetLocalPosition(Vector3(2, 0, 0));
	//cubeGameObject->transform.SetScale(Vector3(10, 0.5, 0.5));
	GameObject* cubeChild = new GameObject();
	cubeChild->transform.SetLocalPosition(Vector3(1, 1, 0));
	cubeGameObject->AddChild(cubeChild);
	Mesh * mesh = static_cast<Mesh*>(cubeChild->AddComponent<Mesh>());
	mesh->LoadFromFile("ConeTriangulate.obj");

	coneGameobject->AddExistingComponent(mesh4);
	coneGameobject->transform.SetPosition(Vector3(0, 0, 0));

	coneGameobject->AddChild(cubeGameObject);
	coneGameobject->name = "Cone";

	myGameObject3->AddExistingComponent(mesh5);
	myGameObject3->transform.SetPosition(Vector3(0, -2, 0));

	mesh5->gameObject->transform.SetScale(Vector3(10, 1, 10));

	shader->SetShaderTexture("material.diffuse", texture3);
	shader->SetShaderTexture("material.specular", texture4);
	shader3->SetShaderTexture("material.diffuse", texture5);
	shader3->SetShaderTexture("material.specular", texture6);

	//mesh1.shader = &shader;
	//mesh2.shader = &shader2;
	mesh->shader = shader3;
	mesh3->shader = shader3;
	mesh4->shader = shader3;
	mesh5->shader = shader3;

	SpriteRenderer* spr = new SpriteRenderer();
	spr->texture = texture7;
	spr->shader = shaderStandard2D;
	spr->width = 100;
	spr->height = 100;
	gameObjectSprite->AddExistingComponent(spr);

	SDL_SetRelativeMouseMode(SDL_TRUE);
}

void Game::Loop()
{
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
		cubeGameObject->SetActive(!cubeGameObject->GetActive());
	}
	if (InputSystem::GetKeyDown(X))
	{
		coneGameobject->SetActive(!coneGameobject->GetActive());
	}
	if (InputSystem::GetKeyDown(C))
	{
		myGameObject3->SetActive(!myGameObject3->GetActive());
	}

	Vector3 newCameraPosition = camera->gameObject->transform.GetPosition();
	if (InputSystem::GetKey(Z))
	{
		Vector3 vect = Graphics::usedCamera->GetSphericalCoordinate();
		vect *= EngineSettings::deltaTime * 2;
		newCameraPosition.x += vect.x;
		newCameraPosition.y += vect.y;
		newCameraPosition.z += vect.z;

	}
	if (InputSystem::GetKey(S)) {
		Vector3 vect = Graphics::usedCamera->GetSphericalCoordinate();
		vect *= EngineSettings::deltaTime * 2;
		newCameraPosition.x -= vect.x;
		newCameraPosition.y -= vect.y;
		newCameraPosition.z -= vect.z;
	}
	if (InputSystem::GetKey(D)) {

		Vector3 vect = Graphics::usedCamera->GetSphericalCoordinate2();
		vect *= EngineSettings::deltaTime * 2;
		newCameraPosition.x += vect.x;
		newCameraPosition.z += vect.z;
	}
	if (InputSystem::GetKey(Q)) {

		Vector3 vect = Graphics::usedCamera->GetSphericalCoordinate2();
		vect *= EngineSettings::deltaTime * 2;
		newCameraPosition.x -= vect.x;
		newCameraPosition.z -= vect.z;
	}
	camera->gameObject->transform.SetPosition(newCameraPosition);

	//Animation
	animation = (float)SDL_GetTicks64() / 500;
	animation = sin(animation) / 2.0f + 0.5f;

	Vector3 newCameraRotation = camera->gameObject->transform.GetRotation();
	float xInputToAdd = -InputSystem::mouseSpeed.y * EngineSettings::deltaTime * 20;
	float yInputToAdd = -InputSystem::mouseSpeed.x * EngineSettings::deltaTime * 20;

	newCameraRotation.x += xInputToAdd;
	newCameraRotation.y += yInputToAdd;

	camera->gameObject->transform.SetRotation(newCameraRotation);

	//mesh3->gameObject->transform.rotation.y = 45;
	//mesh4->gameObject->transform.rotation.y -= 0.1f;

	Vector3 mesh4NewRotation = mesh4->gameObject->transform.GetRotation();
	if (InputSystem::GetKey(RIGHT)) {
		mesh4NewRotation.y -= EngineSettings::deltaTime * 25;
	}
	if (InputSystem::GetKey(LEFT)) {
		mesh4NewRotation.y += EngineSettings::deltaTime * 25;
	}
	if (InputSystem::GetKey(UP)) {
		mesh4NewRotation.x -= EngineSettings::deltaTime * 25;
	}
	if (InputSystem::GetKey(DOWN)) {
		mesh4NewRotation.x += EngineSettings::deltaTime * 25;
	}

	cubeGameObject->transform.SetLocalRotation(cubeGameObject->transform.GetLocalRotation() + Vector3(10,0,0) * EngineSettings::deltaTime);

	if (InputSystem::GetKey(V)) {
		int c = AssetManager::textures.size();
		for (int i = 0; i < c; i++)
		{
			AssetManager::textures[i]->SetFilter(Texture::Bilinear);
		}
	}
	if (InputSystem::GetKey(B)) {
		int c = AssetManager::textures.size();
		for (int i = 0; i < c; i++)
		{
			AssetManager::textures[i]->SetFilter(Texture::Point);
		}
	}

	mesh4->gameObject->transform.SetRotation(mesh4NewRotation);
}