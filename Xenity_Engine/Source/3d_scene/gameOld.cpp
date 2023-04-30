#include "gameOld.h"
#include "../xenity.h"


#include <SDL2/SDL.h>

#include <chrono>
#include <iostream>

#include "orbital_camera.h"


using namespace std::chrono;


/// <summary>
/// Init game
/// </summary>
void Game3D::Init()
{
	//loadedScenes.push_back(new Scene());
	//usedScene = loadedScenes[0];
	//AudioClip audio1 = AudioClip("camera-13695.mp3");
	//AudioSource::Play2DSound(audio1);
	//AudioSource::Play3DSound(audio1, Vector3(0, 0, 2));

	Shader* shader = new Shader("vertexStandard.shader", "fragmentStandard.shader");

	Shader* shader3 = new Shader("3D/vStandard.shader", "3D/fStandard.shader");
	//Shader* shader3 = new Shader("3D/vStandardNoLight.shader", "3D/fStandardNoLight.shader");

	//Shader* shader3 = new Shader("3D/Unlit/vColor.shader", "3D/Unlit/fColor.shader");
	//Shader* shader3 = new Shader("3D/Unlit/vTexture.shader", "3D/Unlit/fTexture.shader");

	Shader* shader2 = new Shader("vertex2.shader", "fragment2.shader");
	Shader* shaderText = new Shader("vertexText.shader", "fragmentText.shader");
	Shader* shaderTextCanvas = new Shader("vertexTextCanvas.shader", "fragmentTextCanvas.shader");
	Shader* shader2D = new Shader("vertex2D.shader", "fragment2D.shader");
	Shader* shaderStandard2D = new Shader("vertexStandard2D.shader", "fragmentStandard2D.shader");
	Shader* shaderWithTessellation = new Shader("3D/vStandard.shader", "3D/fStandard.shader", "tessellation.shader", "tessellation_evaluation.shader");

	Texture* texture1 = new Texture("Brick.png", "Brick");
	Texture* texture2 = new Texture("Dry Dirt.png", "Dry Dirt");
	//Wood and metal 1
	Texture* texture3 = new Texture("container.png", "container");
	Texture* texture4 = new Texture("container_specular.png", "container_specular");
	//Wood and metal 2
	Texture* texture5 = new Texture("Wood_Gate_Fortified_basecolor.jpg", "Wood_Gate_Fortified_basecolor");
	Texture* texture6 = new Texture("Wood_Gate_Fortified_metallic.jpg", "Wood_Gate_Fortified_metallic");
	Texture* texture7 = new Texture("ship_0000.png","ship_0000", Texture::Filter::Point, false);

	Texture* bottleTexture = new Texture("gas_bottle_fire_albedo.png", "gas_bottle_fire_albedo");
	Texture* bottleMetallicTexture = new Texture("gas_bottle_fire_metallic.png", "gas_bottle_fire_metallic");

	//Texture* chessTexture = new Texture("Black_Base_Color.png", "Black_Base_Color");
	//Texture* chessTexture2 = new Texture("Black_Roughness.png", "Black_Roughness");

	Texture* debugTexture = new Texture("debug_uv.png", "debug_uv");

	//SceneGame1* scene = new SceneGame1();
	//SceneManager::LoadScene(scene);

	Material* newMat = new Material("vStandard");
	newMat->shader = shader3;
	newMat->SetAttribut("color", Vector3(0, 1, 1));
	newMat->SetAttribut("material.diffuse", texture5);
	newMat->SetAttribut("material.specular", texture6);
	newMat->SetAttribut("material.shininess", 32.0f);
	newMat->SetAttribut("ambiantLightColor", Vector3(0.529f, 0.808f, 0.922f));

	Material* bottleMat = new Material("Bottle");
	bottleMat->shader = shader3;
	bottleMat->SetAttribut("color", Vector3(1, 1, 1));
	bottleMat->SetAttribut("material.diffuse", bottleTexture);
	bottleMat->SetAttribut("material.specular", bottleMetallicTexture);
	bottleMat->SetAttribut("material.shininess", 32.0f);
	bottleMat->SetAttribut("ambiantLightColor", Vector3(0.529f, 0.808f, 0.922f));

	Material* chessMat = new Material("Chess");
	chessMat->shader = shader3;
	chessMat->SetAttribut("color", Vector3(1, 1, 1));
	chessMat->SetAttribut("material.diffuse", bottleTexture);
	chessMat->SetAttribut("material.specular", bottleMetallicTexture);
	chessMat->SetAttribut("material.shininess", 32.0f);
	chessMat->SetAttribut("ambiantLightColor", Vector3(0.529f, 0.808f, 0.922f));

	Material* newMat2 = new Material("vStandard2");
	newMat2->shader = shader3;
	newMat2->SetAttribut("color", Vector3(0, 1, 1));
	newMat2->SetAttribut("material.diffuse", texture5);
	newMat2->SetAttribut("material.specular", texture6);
	newMat2->SetAttribut("material.shininess", 32.0f);
	newMat2->SetAttribut("ambiantLightColor", Vector3(0.529f, 0.808f, 0.922f));

	GameObject* TestGame = new GameObject("MY TEST");
	/*Benchmark* bench = new Benchmark();
	bench->Start();
	for (int i = 0; i < 500000; i++)
	{
		TestGame->transform.SetPosition(Vector3(4, i, 4));
		TestGame->transform.SetRotation(Vector3(60, i, 40));
		TestGame->transform.SetLocalScale(Vector3(3, i, 3));
	}
	bench->Stop();
	std::cout << bench->GetMicroSeconds() << std::endl;*/

	GameObject* Camera1Point = new GameObject("Camera1Point");
	Camera1Point->transform.SetPosition(Vector3(4, 4, 4));

	cameraGameObject->AddExistingComponent(camera);
	//cameraGameObject->AddExistingComponent(orbitalCamera);
	camera->gameObject->transform.SetPosition(Vector3(0, 2, -4));
	cameraGameObject->transform.SetLocalScale(Vector3(0, 0, 0));
	//orbitalCamera->target = &cubeGameObject->transform;
	//orbitalCamera->camera = camera;
	//orbitalCamera->camera1 = &Camera1Point->transform;

	ShapeSpawner::defaultScale = Vector3(0.1f, 0.1f, 0.1f);

	/*SplinePoint* splinePoint0 = spline->CreateSplinePoint(Vector3(0, 0, 0));
	SplinePoint* splinePoint1 = spline->CreateSplinePoint(Vector3(1, 1, 0));
	SplinePoint* splinePoint2 = spline->CreateSplinePoint(Vector3(3, 2, 0));
	SplinePoint* splinePoint3 = spline->CreateSplinePoint(Vector3(6, 0, 0));
	spline->AddSplinePoint(splinePoint0);
	spline->AddSplinePoint(splinePoint1);
	spline->AddSplinePoint(splinePoint2);
	spline->AddSplinePoint(splinePoint3);*/

	int splinePointCount = 100;
	for (int i = 0; i < splinePointCount; i++)
	{
		float t = i / (float)splinePointCount;
		//GameObject* newShape = ShapeSpawner::SpawnSphere();
		//newShape->transform.SetPosition(spline->GetValueAt(t));
	}

	//newShape->GetComponent<MeshRenderer>()->material = newMat;

	mesh3 = new MeshRenderer("CubeTriangulate.obj");
	mesh4 = new MeshRenderer("ConeTriangulate.obj");
	mesh5 = new MeshRenderer("CubeTriangulate.obj");
	bottle = new MeshRenderer("gas_bottle.obj");
	bottle2 = new MeshRenderer("gas_bottle.obj");
	bottle3 = new MeshRenderer("gas_bottle.obj");
	chess = new MeshRenderer("chess_tower.obj");

	//bottle = new MeshRenderer("debug_cube.obj");

	//cubeGameObject->AddExistingComponent(chess);
	GameObject* newShape = ShapeSpawner::SpawnSphere();
	newShape->transform.SetPosition(Vector3(1, 0, 2));

	GameObject* newShape2 = ShapeSpawner::SpawnCube();
	//newShape2->transform.SetPosition(Vector3(1, 0, 1));
	newShape2->transform.SetLocalScale(Vector3(1, 1, 1));

	GameObject* childBottle = new GameObject("UWU");
	GameObject* childBottle2 = new GameObject();
	cubeGameObject->AddExistingComponent(bottle);
	childBottle->AddExistingComponent(bottle2);
	childBottle2->AddExistingComponent(bottle3);
	cubeGameObject->transform.SetPosition(Vector3(0, 0, 0));
	cubeGameObject->transform.SetRotation(Vector3(45, 45, 45));
	cubeGameObject->transform.SetLocalScale(Vector3(20, 20, 20));
	cubeGameObject->AddChild(newShape2);

	newShape2->transform.SetLocalPosition(Vector3(0.02, 0, 0.02));
	newShape2->transform.SetLocalRotation(Vector3(0, 0, 0));
	childBottle->transform.SetLocalPosition(Vector3(6.121321, -1.242641, 4.12132));
	childBottle->transform.SetLocalRotation(Vector3(30, 170.264, -40.264));
	//childBottle->transform.SetLocalRotation(Vector3(150, -9.7, 139.7));
	childBottle->transform.SetLocalScale(Vector3(20, 20, 20));
	cubeGameObject->AddChild(childBottle);
	childBottle->AddChild(childBottle2);
	childBottle2->transform.SetLocalPosition(Vector3(0.0, 0.1, 0.0));
	childBottle2->transform.SetLocalRotation(Vector3(30, 30, 30));
	childBottle2->transform.SetLocalScale(Vector3(1, 1, 1));

	//newShape2->SetParent(cubeGameObject);

	//cubeGameObject->transform.SetLocalScale(Vector3(5, 5, 5));
	//cubeGameObject->transform.SetLocalScale(Vector3(0.1f, 0.1f, 0.1f));

	cubeChild->SetActive(false);
	cubeChild->transform.SetPosition(Vector3(4, 0, 0));
	cubeChild->transform.SetRotation(Vector3(0, 0, 20));
	cubeChild->transform.SetLocalScale(Vector3(1, 1, 1));
	//cubeGameObject->AddChild(cubeChild);
	MeshRenderer* mesh = static_cast<MeshRenderer*>(cubeChild->AddComponent<MeshRenderer>());
	mesh->LoadFromFile("CubeTriangulate.obj");

	GameObject* cubeChild2 = new GameObject("Cube2");
	cubeChild2->transform.SetPosition(Vector3(6.5, 0, 0));
	cubeChild2->transform.SetRotation(Vector3(0, 0, 10));
	cubeChild2->transform.SetLocalScale(Vector3(1.5f, 1.5f, 1.5f));
	cubeChild->AddChild(cubeChild2);
	MeshRenderer* mesh222 = static_cast<MeshRenderer*>(cubeChild2->AddComponent<MeshRenderer>());
	mesh222->LoadFromFile("CubeTriangulate.obj");

	//cubeGameObject->transform.SetLocalScale(Vector3(1, 1, 1));
	//cubeChild->SetParent(nullptr);

	/*cubeGameObject->AddExistingComponent(mesh3);
	cubeGameObject->transform.SetPosition(Vector3(1, 0, 0));
	//cubeGameObject->transform.SetScale(Vector3(10, 0.5, 0.5));
	GameObject* cubeChild = new GameObject();
	cubeChild->transform.SetLocalPosition(Vector3(1, 1, 0));
	cubeGameObject->AddChild(cubeChild);
	MeshRenderer * mesh = static_cast<MeshRenderer*>(cubeChild->AddComponent<MeshRenderer>());
	mesh->LoadFromFile("ConeTriangulate.obj");

	coneGameobject->AddExistingComponent(mesh4);
	coneGameobject->transform.SetPosition(Vector3(0, 0, 0));
	coneGameobject->transform.SetLocalScale(Vector3(2, 2, 2));

	coneGameobject->AddChild(cubeGameObject);
	coneGameobject->name = "Cone";

	cubeGameObject->transform.SetPosition(Vector3(5, 0, 0));*/

	myGameObject3->AddExistingComponent(mesh5);
	myGameObject3->transform.SetPosition(Vector3(0, -1, 0));

	myGameObject3->transform.SetLocalScale(Vector3(5, 1, 5));

	mesh->material = newMat;
	mesh222->material = newMat;
	mesh3->material = newMat;
	bottle->material = bottleMat;
	bottle2->material = bottleMat;
	bottle3->material = bottleMat;
	chess->material = chessMat;
	mesh4->material = newMat2;
	mesh5->material = newMat;

	Material* material2D = new Material("shaderStandard2D");
	material2D->shader = shaderStandard2D;

	pointLightGameObject->transform.SetPosition(Vector3(1.5f, 1.5, 1.5f));
	pointLightGameObject->AddExistingComponent(pointLight);
	pointLightGameObject2->AddExistingComponent(pointLight2);
	pointLight->SetupPointLight(Vector3(1, 1, 1), 6, 7);
	pointLight2->SetupPointLight(Vector3(0, 0, 1), 30, 7);
	//pointLight->SetupPointLight(Vector3(1, 0.1f, 0.1f), 10, 7);

	spotLightGameObject->transform.SetPosition(Vector3(0, 3, 0));
	spotLightGameObject->transform.SetRotation(Vector3(90.0f, 0.0f, 0.0f));
	spotLightGameObject->AddExistingComponent(spotLight);
	spotLight->SetupSpotLight(Vector3(0.05f, 0.05f, 1), 200, 7, 17, 0);

	spotLight2GameObject->transform.SetPosition(Vector3(5, 3, 0));
	spotLight2GameObject->transform.SetRotation(Vector3(90.0f, 0.0f, 0.0f));
	spotLight2GameObject->AddExistingComponent(spotLight2);
	spotLight2->SetupSpotLight(Vector3(0.05f, 0.05f, 1), 200, 7, 17, 1);

	directionalLightGameObject->transform.SetRotation(Vector3(45.0f, 45.0f, 0.0f));
	directionalLightGameObject->AddExistingComponent(directionalLight);
	directionalLight->SetupDirectionalLight(Vector3(0.6f, 0.6f, 0.7f), 1);

	SDL_SetRelativeMouseMode(SDL_TRUE);
	spotLight->intensity = 0;
	spotLight2->intensity = 0;
}

/// <summary>
/// Game loop
/// </summary>
void Game3D::Loop()
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

	Vector3 cubeNewRotation = cubeGameObject->transform.GetRotation();
	Vector3 newCameraPosition = camera->gameObject->transform.GetPosition();

	if (InputSystem::GetKey(Z))
	{
		Vector3 vect = Graphics::usedCamera->gameObject->transform.GetForward();
		vect *= Time::GetDeltaTime() * 2;
		//newCameraPosition += vect;
		cubeNewRotation.x += Time::GetDeltaTime() * 20;
	}
	if (InputSystem::GetKey(S)) {
		Vector3 vect = Graphics::usedCamera->gameObject->transform.GetForward();
		vect *= Time::GetDeltaTime() * 2;
		//newCameraPosition -= vect;
		cubeNewRotation.x += -Time::GetDeltaTime() * 20;
	}
	if (InputSystem::GetKey(D)) {

		Vector3 vect = Graphics::usedCamera->gameObject->transform.GetRight();
		vect *= Time::GetDeltaTime() * 2;
		cubeNewRotation.z += Time::GetDeltaTime() * 20;
		//newCameraPosition += vect;
	}
	if (InputSystem::GetKey(Q)) {

		Vector3 vect = Graphics::usedCamera->gameObject->transform.GetLeft();
		vect *= Time::GetDeltaTime() * 2;
		cubeNewRotation.z += -Time::GetDeltaTime() * 20;
		//newCameraPosition += vect;
	}
	camera->gameObject->transform.SetPosition(newCameraPosition);

	//Animation
	animation = SDL_GetTicks() / 500.0f;
	//animation = sin(animation) / 2.0f + 0.5f;
	animation /= 10.0f;

	//if(cubeChild->parent != nullptr)
	//cubeChild->transform.SetLocalScale(Vector3(1 + sin(animation)/2.0f, 1 + sin(animation) / 2.0f, 1 + sin(animation) / 2.0f));
	//cubeGameObject->transform.SetLocalScale(Vector3(1 + sin(animation) / 2.0f, 1 + sin(animation) / 2.0f, 1 + sin(animation) / 2.0f));

	//cubeGameObject->transform.SetLocalScale(Vector3(1, 1, 1));
	lightAnimation += Time::GetDeltaTime()/3.0f;
	pointLightGameObject->transform.SetPosition(Vector3(cos(lightAnimation * M_PI * 2) * 3, 1.5, sin(lightAnimation * M_PI * 2)*3));
	pointLightGameObject2->transform.SetPosition(Vector3(cos((lightAnimation+0.5) * M_PI * 2) * 3, 1.5, sin((lightAnimation+0.5) * M_PI * 2) * 3));

	Vector3 newCameraRotation = camera->gameObject->transform.GetRotation();
	float xInputToAdd = -InputSystem::mouseSpeedRaw.y * Time::GetDeltaTime() * 20;
	float yInputToAdd = InputSystem::mouseSpeedRaw.x * Time::GetDeltaTime() * 20;

	newCameraRotation.x += xInputToAdd;
	newCameraRotation.y += yInputToAdd;

	camera->gameObject->transform.SetRotation(newCameraRotation);

	//Vector3 mesh4NewRotation = mesh4->gameObject->transform.GetRotation();
	Vector3 mesh4NewRotation = cubeChild->transform.GetRotation();
	Vector3 cubeNewPosition = cubeGameObject->transform.GetPosition();
	//Vector3 cubeNewRotation = cubeGameObject->transform.GetRotation();
	if (InputSystem::GetKey(UP))
	{
		Vector3 vect = cubeGameObject->transform.GetForward();
		//Vector3 vect = Graphics::usedCamera->gameObject->transform.GetUp();
		vect *= Time::GetDeltaTime() * 5;
		cubeNewPosition += vect;

	}
	if (InputSystem::GetKey(DOWN)) {
		Vector3 vect = cubeGameObject->transform.GetForward();
		vect *= Time::GetDeltaTime() * 5;
		cubeNewPosition -= vect;
	}
	if (InputSystem::GetKey(RIGHT)) {

		Vector3 vect = cubeGameObject->transform.GetRight();
		vect *= Time::GetDeltaTime() * 5;
		cubeNewPosition += vect;
	}
	if (InputSystem::GetKey(LEFT)) {

		Vector3 vect = cubeGameObject->transform.GetLeft();
		vect *= Time::GetDeltaTime() * 5;
		cubeNewPosition += vect;
	}
	if (InputSystem::GetKey(L)) {

		Vector3 vect = cubeGameObject->transform.GetUp();
		vect *= Time::GetDeltaTime() * 5;
		cubeNewPosition += vect;
	}
	if (InputSystem::GetKey(M)) {

		Vector3 vect = cubeGameObject->transform.GetDown();
		vect *= Time::GetDeltaTime() * 5;
		cubeNewPosition += vect;
	}

	/*if (InputSystem::GetKey(Z))
	{
		mesh4NewRotation.x += Time::GetDeltaTime() * 25;

	}
	if (InputSystem::GetKey(S)) {
		mesh4NewRotation.x -= Time::GetDeltaTime() * 25;
	}
	if (InputSystem::GetKey(D)) {

		mesh4NewRotation.y += Time::GetDeltaTime() * 25;
	}
	if (InputSystem::GetKey(Q)) {

		mesh4NewRotation.y -= Time::GetDeltaTime() * 25;
	}
	if (InputSystem::GetKey(P))
	{
		mesh4NewRotation.z -= Time::GetDeltaTime() * 25;
	}
	if (InputSystem::GetKey(M))
	{
		mesh4NewRotation.z += Time::GetDeltaTime() * 25;
	}*/
	//cubeGameObject->transform.SetRotation(mesh4NewRotation);
	//cubeChild->transform.SetRotation(mesh4NewRotation);

	if (InputSystem::GetKeyDown(SPACE)) {
		cubeChild->SetParent(nullptr);
	}

	//std::cout << cubeChild->transform.GetLocalPosition().x << std::endl;

	//cubeGameObject->transform.SetLocalRotation(cubeGameObject->transform.GetLocalRotation() + Vector3(10,0,0) * EngineSettings::deltaTime);

	if (InputSystem::GetKey(V))
	{
		int c = AssetManager::GetTextureCount();
		for (int i = 0; i < c; i++)
		{
			AssetManager::GetTexture(i)->SetFilter(Texture::Bilinear);
		}
	}
	if (InputSystem::GetKey(B))
	{
		int c = AssetManager::GetTextureCount();
		for (int i = 0; i < c; i++)
		{
			AssetManager::GetTexture(i)->SetFilter(Texture::Point);
		}
	}

	//cubeGameObject->transform.SetRotation(mesh4NewRotation);
	cubeGameObject->transform.SetPosition(cubeNewPosition);
	cubeGameObject->transform.SetRotation(cubeNewRotation);

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