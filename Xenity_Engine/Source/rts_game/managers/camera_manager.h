#pragma once

#include "../../engine/monobehaviour.h"

class Transform;
class Camera;

class CameraManager : public MonoBehaviour
{
public:
	CameraManager();

	void Init();
	void Update();

	Camera* camera = nullptr;
private:
	Transform* cameraTransform = nullptr;
	float cameraZoom = 1.5f; //[0.7;2.8]
	float cameraArrowMoveSpeed = 3;
	void MoveCamera();
	void CheckCameraZoom();
};

