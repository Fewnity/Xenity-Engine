#pragma once

#include "../engine/game_elements/gameobject.h"
#include "../engine/graphics/camera.h"
//#include "../graphics/iDrawable.h"

class Material;
class Texture;

class RenderingTest2D
{
public:
	void Init();
	void Loop();
	void LoadGameData();

private:
	GameObject* cameraGameObject = new GameObject("cameraGameObject");
	Camera* camera = new Camera();
	Material* material2D = nullptr;
	Material* material2DWithZ = nullptr;
	Texture* crosshair = nullptr;
	Texture* textureTile0 = nullptr;
	Texture* gradient = nullptr;


	GameObject* gradientGameObject = new GameObject("gradientGameObject");
	GameObject* tileGameObject = new GameObject("tileGameObject");

	GameObject* tileGameObjectBack = new GameObject("tileGameObjectBack");
	GameObject* tileGameObjectFront = new GameObject("tileGameObjectFront");

	GameObject* gradientGameObjectBack = new GameObject("gradientGameObjectBack");
	GameObject* gradientGameObjectFront = new GameObject("gradientGameObjectFront");
};

