#pragma once

#include "graphics/camera.h"
#include "gameobject.h"
#include "graphics/mesh.h"
#include "graphics/graphics.h"
#include "lighting/lighting.h"
#include "orbital_camera.h"

class Game3D
{
public:
	void Init();
	void Loop();

private:
	GameObject* cameraGameObject = new GameObject("cameraGameObject");

	Camera* camera = new Camera();
	OrbitalCamera* orbitalCamera = new OrbitalCamera();

	GameObject* cubeGameObject = new GameObject("Cube0");
	GameObject* coneGameobject = new GameObject("Cone");
	GameObject* myGameObject3 = new GameObject();

	GameObject* gameObjectSprite = new GameObject("gameObjectSprite");

	MeshRenderer* mesh3 = nullptr;
	MeshRenderer* mesh4 = nullptr;
	MeshRenderer* mesh5 = nullptr;
	float animation = 0;

	Light* pointLight = new Light();
	Light* spotLight = new Light();
	Light* spotLight2 = new Light();
	Light* directionalLight = new Light();
	GameObject* pointLightGameObject = new GameObject("pointLightGameObject");
	GameObject* spotLightGameObject = new GameObject("spotLightGameObject");
	GameObject* spotLight2GameObject = new GameObject("spotLight2GameObject");
	GameObject* directionalLightGameObject = new GameObject("directionalLightGameObject");

	GameObject* cubeChild = new GameObject("Cube1");
};

