#include "game.h"
#include "audio/audio_clip.h"
#include "gameobject.h"
#include "graphics/graphics.h"
#include "graphics/mesh.h"
#include "graphics/material.h"
#include "graphics/camera.h"
#include <SDL2/SDL.h>
#include "scene_manager/SceneManager.h"
#include "game/sceneGame1.h"
#include "lighting/lighting.h"
#include "inputs/input_system.h"
#include "engine_settings.h"
#include "vectors/vector3.h"
#include<chrono>
#include <iostream>
#include "graphics/spriteRenderer.h"
#include "asset_manager.h"
#include "debug.h"
#include "graphics/text_renderer.h"
#include "ui/ui_manager.h"

using namespace std::chrono;

GameObject* cameraGameObject = new GameObject();
Camera* camera = new Camera();

GameObject* cubeGameObject = new GameObject("Cube0");
GameObject* coneGameobject = new GameObject("Cone");
GameObject* myGameObject3 = new GameObject();

GameObject* gameObjectSprite = new GameObject();

Mesh* mesh3 = nullptr;
Mesh* mesh4 = nullptr;
Mesh* mesh5 = nullptr;
float animation = 0;

Light* pointLight = new Light();
Light* spotLight = new Light();
Light* spotLight2 = new Light();
Light* directionalLight = new Light();
GameObject* pointLightGameObject = new GameObject();
GameObject* spotLightGameObject = new GameObject();
GameObject* spotLight2GameObject = new GameObject();
GameObject* directionalLightGameObject = new GameObject();

/// <summary>
/// Init game
/// </summary>
void Game::Init() {
	//loadedScenes.push_back(new Scene());
	//usedScene = loadedScenes[0];
	//AudioClip audio1 = AudioClip("camera-13695.mp3");
	//AudioSource::Play2DSound(audio1);
	//AudioSource::Play3DSound(audio1, Vector3(0, 0, 2));

	Shader* shader = new Shader("vertexStandard.shader", "fragmentStandard.shader");
	//Shader* shader3 = new Shader("vertexStandard.shader", "fragmentStandard.shader");

	Shader* shader3 = new Shader("3D/vStandard.shader", "3D/fStandard.shader");
	//Shader* shader3 = new Shader("3D/vStandardNoLight.shader", "3D/fStandardNoLight.shader");
	 
	//Shader* shader3 = new Shader("3D/Unlit/vColor.shader", "3D/Unlit/fColor.shader");
	//Shader* shader3 = new Shader("3D/Unlit/vTexture.shader", "3D/Unlit/fTexture.shader");

	Shader* shader2 = new Shader("vertex2.shader", "fragment2.shader");
	Shader* shaderText = new Shader("vertexText.shader", "fragmentText.shader");
	Shader* shaderTextCanvas = new Shader("vertexTextCanvas.shader", "fragmentTextCanvas.shader");
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

	Material* newMat = new Material();
	newMat->shader = shader3;
	newMat->SetAttribut("color", Vector3(0,1,1));
	newMat->SetAttribut("material.diffuse", texture5);
	newMat->SetAttribut("material.specular", texture6);
	newMat->SetAttribut("material.shininess", 32.0f);
	newMat->SetAttribut("ambiantLightColor", Vector3(0.529f, 0.808f, 0.922f));

	Material* newMat2 = new Material();
	newMat2->shader = shader3;
	newMat2->SetAttribut("color", Vector3(0, 1, 1));
	newMat2->SetAttribut("material.diffuse", texture2);
	newMat2->SetAttribut("material.shininess", 32.0f);
	newMat2->SetAttribut("ambiantLightColor", Vector3(0.529f, 0.808f, 0.922f));

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
	cubeGameObject->transform.SetPosition(Vector3(2, 0, 0));
	cubeGameObject->transform.SetRotation(Vector3(0, 0, 10));
	cubeGameObject->transform.SetLocalScale(Vector3(2, 2, 2));

	GameObject* cubeChild = new GameObject("Cube1");
	cubeChild->transform.SetPosition(Vector3(4, 0, 0));
	cubeChild->transform.SetRotation(Vector3(0, 0, 20));
	cubeChild->transform.SetLocalScale(Vector3(1, 1, 1));
	cubeGameObject->AddChild(cubeChild);
	Mesh* mesh = static_cast<Mesh*>(cubeChild->AddComponent<Mesh>());
	mesh->LoadFromFile("CubeTriangulate.obj");

	GameObject* cubeChild2 = new GameObject("Cube2");
	cubeChild2->transform.SetPosition(Vector3(6.5, 0, 0));
	cubeChild2->transform.SetRotation(Vector3(0, 0, 10));
	cubeChild2->transform.SetLocalScale(Vector3(1.5, 1.5, 1.5));
	cubeChild->AddChild(cubeChild2);
	Mesh* mesh222 = static_cast<Mesh*>(cubeChild2->AddComponent<Mesh>());
	mesh222->LoadFromFile("CubeTriangulate.obj");

	/*cubeGameObject->AddExistingComponent(mesh3);
	cubeGameObject->transform.SetPosition(Vector3(1, 0, 0));
	//cubeGameObject->transform.SetScale(Vector3(10, 0.5, 0.5));
	GameObject* cubeChild = new GameObject();
	cubeChild->transform.SetLocalPosition(Vector3(1, 1, 0));
	cubeGameObject->AddChild(cubeChild);
	Mesh * mesh = static_cast<Mesh*>(cubeChild->AddComponent<Mesh>());
	mesh->LoadFromFile("ConeTriangulate.obj");

	coneGameobject->AddExistingComponent(mesh4);
	coneGameobject->transform.SetPosition(Vector3(0, 0, 0));
	coneGameobject->transform.SetLocalScale(Vector3(2, 2, 2));

	coneGameobject->AddChild(cubeGameObject);
	coneGameobject->name = "Cone";

	cubeGameObject->transform.SetPosition(Vector3(5, 0, 0));*/

	myGameObject3->AddExistingComponent(mesh5);
	myGameObject3->transform.SetPosition(Vector3(0, -2, 0));

	mesh5->gameObject->transform.SetLocalScale(Vector3(10, 1, 10));

	mesh->material = newMat;
	mesh222->material = newMat;
	mesh3->material = newMat;
	mesh4->material = newMat2;
	mesh5->material = newMat;

	gameObjectSprite->transform.SetPosition(Vector3(0, 0, 0));
	//gameObjectSprite->transform.SetRotation(Vector3(45, 45, 45));

	SpriteRenderer* spr = new SpriteRenderer(texture7, shaderStandard2D);
	gameObjectSprite->AddExistingComponent(spr);

	GameObject *t = new GameObject();
	t->transform.SetPosition(Vector3(0.32f, 0.32f, 0));
	SpriteRenderer* spr2 = new SpriteRenderer(texture7, shaderStandard2D);
	t->AddExistingComponent(spr2);

	TextRenderer* textRenderer = new TextRenderer(UiManager::fonts[0], 5, shaderText);
	textRenderer->text = "Salut à tous les amissssss";
	gameObjectSprite->AddExistingComponent(textRenderer);

	pointLightGameObject->transform.SetPosition(Vector3(1.5f, 1.5, 1.5f));
	pointLightGameObject->AddExistingComponent(pointLight);
	pointLight->SetupPointLight(Vector3(1, 0.1f, 0.1f), 10, 7);

	spotLightGameObject->transform.SetPosition(Vector3(0, 3, 0));
	spotLightGameObject->transform.SetRotation(Vector3(0.0f, -1.0f, 0.0f));
	spotLightGameObject->AddExistingComponent(spotLight);
	spotLight->SetupSpotLight(Vector3(0.05f, 0.05f, 1), 200, 7, 17, 0);

	spotLight2GameObject->transform.SetPosition(Vector3(5, 3, 0));
	spotLight2GameObject->transform.SetRotation(Vector3(0.0f, -1.0f, 0.0f));
	spotLight2GameObject->AddExistingComponent(spotLight2);
	spotLight2->SetupSpotLight(Vector3(0.05f, 0.05f, 1), 200, 7, 17, 1);

	directionalLightGameObject->transform.SetRotation(Vector3(0.0f, -1.0f, -1.0f));
	directionalLightGameObject->AddExistingComponent(directionalLight);
	directionalLight->SetupDirectionalLight(Vector3(0.6f, 0.6f, 0.7f), 1);

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
		newCameraPosition += vect;

	}
	if (InputSystem::GetKey(S)) {
		Vector3 vect = Graphics::usedCamera->GetSphericalCoordinate();
		vect *= EngineSettings::deltaTime * 2;
		newCameraPosition -= vect;
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
	animation = SDL_GetTicks64() /500.0f;
	//animation = sin(animation) / 2.0f + 0.5f;

	cubeGameObject->transform.SetLocalScale(Vector3(2 + sin(animation)/2.0f, 2 + sin(animation) / 2.0f, 2 + sin(animation) / 2.0f));

	Vector3 newCameraRotation = camera->gameObject->transform.GetRotation();
	float xInputToAdd = -InputSystem::mouseSpeed.y * EngineSettings::deltaTime * 20;
	float yInputToAdd = -InputSystem::mouseSpeed.x * EngineSettings::deltaTime * 20;

	newCameraRotation.x += xInputToAdd;
	newCameraRotation.y += yInputToAdd;

	camera->gameObject->transform.SetRotation(newCameraRotation);

	//Vector3 mesh4NewRotation = mesh4->gameObject->transform.GetRotation();
	Vector3 mesh4NewRotation = cubeGameObject->transform.GetRotation();
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
	if (InputSystem::GetKey(P)) {
		mesh4NewRotation.z -= EngineSettings::deltaTime * 25;
	}
	if (InputSystem::GetKey(M)) {
		mesh4NewRotation.z += EngineSettings::deltaTime * 25;
	}

	//cubeGameObject->transform.SetLocalRotation(cubeGameObject->transform.GetLocalRotation() + Vector3(10,0,0) * EngineSettings::deltaTime);

	if (InputSystem::GetKey(V)) {
		int c = AssetManager::GetTextureCount();
		for (int i = 0; i < c; i++)
		{
			AssetManager::GetTexture(i)->SetFilter(Texture::Bilinear);
		}
	}
	if (InputSystem::GetKey(B)) {
		int c = AssetManager::GetTextureCount();
		for (int i = 0; i < c; i++)
		{
			AssetManager::GetTexture(i)->SetFilter(Texture::Point);
		}
	}

	cubeGameObject->transform.SetRotation(mesh4NewRotation);

	std::string debugText = std::string("Cube0 Rotation x:") + std::to_string(cubeGameObject->transform.GetRotation().x) + " y:" + std::to_string(cubeGameObject->transform.GetRotation().y) + " z:" + std::to_string(cubeGameObject->transform.GetRotation().z);
	UiManager::RenderTextCanvas(*AssetManager::GetShader(4), debugText, 0.0f, 50, 20, 0.5f, Vector3(0.5f, 0.0f, 0.2f), UiManager::fonts[0]);

	std::string debugText2 = std::string("Cube1 Rotation x:") + std::to_string(cubeGameObject->children[0]->transform.GetRotation().x) + " y:" + std::to_string(cubeGameObject->children[0]->transform.GetRotation().y) + " z:" + std::to_string(cubeGameObject->children[0]->transform.GetRotation().z);
	UiManager::RenderTextCanvas(*AssetManager::GetShader(4), debugText2, 0.0f, 80, 20, 0.5f, Vector3(0.5f, 0.0f, 0.2f), UiManager::fonts[0]);

	gameObjectSprite->transform.SetRotation(Vector3(0, 0, gameObjectSprite->transform.GetRotation().z + EngineSettings::deltaTime*10));

	//mesh4->gameObject->transform.SetRotation(mesh4NewRotation);
}