#pragma once

#include "component.h"
#include <vector>

class Camera;
class Transform;

class Box {
public:
	Box() {

	}

	Box(Vector3 position, Vector3 scale){
		this->position = position;
		this->scale = scale;
	}
	Vector3 position;
	Vector3 scale;
};

class OrbitalCamera : public Component
{
public:
	enum CameraStatus
	{
		ThirdPerson, FirstPerson, Camera1
	};

	OrbitalCamera();
	Camera* camera = nullptr;
	Transform* target = nullptr;
	void Update();


	float axis1 = 0;
	float axis2 = 0;
	Vector3 lastMovedPos;
	Vector3 cameraWithoutOffset;
	bool isStopped = false;
	float speed = 1;

	Vector3 UpdateFpsMode();
	Vector3 UpdateThirdPersonMode(Vector3 cameraOffset);
	Vector3 UpdateCamera1Mode();
	void LerpMovements(Vector3 lerpTarget, Vector3 cameraOffset);
	void LookAtFirstPerson(Vector3 target);

	CameraStatus cameraStatus = CameraStatus::ThirdPerson;
	Transform* camera1 = nullptr;
	bool GetHitDistance(Vector3 corner1, Vector3 corner2, Vector3 dirfrac, Vector3 startPosition, float* t);;
	bool DetectWalls(float* distance, Vector3 dir, Vector3 startDir);
	float currentDistance = 4;
	float targetDistance = 4;

	std::vector<Box> walls;
};

