#pragma once

#include "../engine/graphics/camera.h"
#include "../engine/game_elements/gameobject.h"
#include "../engine/graphics/3d_graphics/mesh.h"
#include "../engine/graphics/graphics.h"
#include "../engine/lighting/lighting.h"
#include "orbital_camera.h"

class Game3D
{
public:
	void Init();
	void Loop();

private:
	GameObject* cameraGameObject = new GameObject("cameraGameObject");

	//Camera* camera = new Camera();
	Camera* camera = nullptr;
	OrbitalCamera* orbitalCamera = new OrbitalCamera();

	GameObject* cubeGameObject = new GameObject("Cube0");
	GameObject* coneGameobject = new GameObject("Cone");
	GameObject* myGameObject3 = new GameObject();

	GameObject* gameObjectSprite = new GameObject("gameObjectSprite");

	MeshRenderer* mesh3 = nullptr;
	MeshRenderer* mesh4 = nullptr;
	MeshRenderer* mesh5 = nullptr;
	MeshRenderer* bottle = nullptr;
	MeshRenderer* bottle2 = nullptr;
	MeshRenderer* bottle3 = nullptr;
	MeshRenderer* chess = nullptr;
	float animation = 0;

	/*Light* pointLight = new Light();
	Light* pointLight2 = new Light();
	Light* spotLight = new Light();
	Light* spotLight2 = new Light();
	Light* directionalLight = new Light();*/

	Light* pointLight = nullptr;
	Light* pointLight2 = nullptr;
	Light* spotLight = nullptr;
	Light* spotLight2 = nullptr;
	Light* directionalLight = nullptr;

	GameObject* pointLightGameObject = new GameObject("pointLightGameObject");
	GameObject* pointLightGameObject2 = new GameObject("pointLightGameObject2");
	GameObject* spotLightGameObject = new GameObject("spotLightGameObject");
	GameObject* spotLight2GameObject = new GameObject("spotLight2GameObject");
	GameObject* directionalLightGameObject = new GameObject("directionalLightGameObject");

	GameObject* cubeChild = new GameObject("Cube1");
	float lightAnimation = 0;
};

