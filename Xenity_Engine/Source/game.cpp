#include "game.h"
#include "audio/audio_clip.h"
#include "gameobject.h"
#include "graphics/graphics.h"
#include "graphics/mesh.h"
#include "graphics/camera.h"
#include <SDL2/SDL.h>
#include "lighting/lighting.h"
#include "inputs/input_system.h"
#include "engine_settings.h"

GameObject cameraGameObject = GameObject();
Camera camera = Camera();

GameObject myGameObject1 = GameObject();
GameObject myGameObject2 = GameObject();
GameObject myGameObject3 = GameObject();
Mesh* mesh3;
Mesh* mesh4;
Mesh* mesh5;
float animation = 0;
#include<chrono>
#include <iostream>
using namespace std::chrono;

void Game::Init() {
	//loadedScenes.push_back(new Scene());
	//usedScene = loadedScenes[0];
	//AudioClip audio1 = AudioClip("camera-13695.mp3");
	//AudioSource::Play2DSound(audio1);
	//AudioSource::Play3DSound(audio1, Vector3(0, 0, 2));

	camera.SetGameObject(&cameraGameObject);
	camera.gameObject->transform.position = Vector3(0, 2, 2);

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

	myGameObject1.components.push_back(mesh3);
	mesh3->SetGameObject(&myGameObject1);

	myGameObject1.transform.position.x = 4;

	myGameObject2.components.push_back(mesh4);
	mesh4->SetGameObject(&myGameObject2);
	myGameObject2.transform.position.x = 0;

	myGameObject3.components.push_back(mesh5);
	mesh5->SetGameObject(&myGameObject3);
	myGameObject3.transform.position.x = 0;
	myGameObject3.transform.position.y = -2;

	Shader* shader = new Shader("vertexStandard.shader", "fragmentStandard.shader");
	Shader *shader3 = new Shader("vertexStandard.shader", "fragmentStandard.shader");
	Shader *shader2 = new Shader("vertex2.shader", "fragment2.shader");

	Shader *shaderText = new Shader("vertexText.shader", "fragmentText.shader");

	Texture *texture1 = new Texture("Brick.png", 0);

	Texture *texture2 = new Texture("Dry Dirt.png", 1);

	//Wood and metal 1
	Texture *texture3 = new Texture("container.png", 2);
	Texture *texture4 = new Texture("container_specular.png", 3);

	//Wood and metal 2
	Texture *texture5 = new Texture("Wood_Gate_Fortified_basecolor.jpg", 4);
	Texture *texture6 = new Texture("Wood_Gate_Fortified_metallic.jpg", 5);

	shader->SetShaderTexture("material.diffuse", texture3);
	shader->SetShaderTexture("material.specular", texture4);
	shader3->SetShaderTexture("material.diffuse", texture5);
	shader3->SetShaderTexture("material.specular", texture6);


	shader->Use();

	//int frameCount = 0;

	//mesh1.shader = &shader;
	//mesh2.shader = &shader2;
	mesh3->shader = shader3;
	mesh4->shader = shader3;
	mesh5->shader = shader3;

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
		myGameObject1.active = !myGameObject1.active;
	}
	if (InputSystem::GetKeyDown(X))
	{
		myGameObject2.active = !myGameObject2.active;
	}
	if (InputSystem::GetKeyDown(C))
	{
		myGameObject3.active = !myGameObject3.active;
	}

	if (InputSystem::GetKey(Z))
	{
		Vector3 vect = Graphics::usedCamera->GetSphericalCoordinate();
		vect /= 20.0f;
		camera.gameObject->transform.position.x += vect.x;
		camera.gameObject->transform.position.y += vect.y;
		camera.gameObject->transform.position.z += vect.z;
	}
	if (InputSystem::GetKey(S)) {
		Vector3 vect = Graphics::usedCamera->GetSphericalCoordinate();
		vect /= 20.0f;
		camera.gameObject->transform.position.x -= vect.x;
		camera.gameObject->transform.position.y -= vect.y;
		camera.gameObject->transform.position.z -= vect.z;
	}
	if (InputSystem::GetKey(D)) {

		Vector3 vect = Graphics::usedCamera->GetSphericalCoordinate2();
		vect /= 20.0f;
		camera.gameObject->transform.position.x += vect.x;
		camera.gameObject->transform.position.z += vect.z;
	}
	if (InputSystem::GetKey(Q)) {

		Vector3 vect = Graphics::usedCamera->GetSphericalCoordinate2();
		vect /= 20.0f;
		camera.gameObject->transform.position.x -= vect.x;
		camera.gameObject->transform.position.z -= vect.z;
	}

	//Animation
	animation = (float)SDL_GetTicks64() / 500;
	animation = sin(animation) / 2.0f + 0.5f;

	if (camera.gameObject->transform.rotation.x + -InputSystem::mouseSpeed.y / 4.0f < 90 && camera.gameObject->transform.rotation.x + -InputSystem::mouseSpeed.y / 4.0f > -90)
		camera.gameObject->transform.rotation.x += -InputSystem::mouseSpeed.y / 4.0f;

	camera.gameObject->transform.rotation.y += -InputSystem::mouseSpeed.x / 4.0f;

	//mesh3->gameObject->transform.rotation.y = 45;
	//mesh4->gameObject->transform.rotation.y -= 0.1f;
	mesh5->gameObject->transform.scale = Vector3(10, 1, 10);
}