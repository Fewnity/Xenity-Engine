#pragma once

#include "../monobehaviour.h"

class GameObject;
class Camera;


class CameraManager : public MonoBehaviour
{
public:
	CameraManager();
	void Awake();

	void Init();

	void Update();
	Camera* camera = nullptr;
private:
	GameObject* cameraGameObject = nullptr;
	float cameraZoom = 1.5f; //[0.7;2.8]
	float cameraArrowMoveSpeed = 3;
	void MoveCamera();
	void ZoomCamera();
};

