#pragma once

#include "../engine/monobehaviour.h"
#include "../engine/component.h"
#include "../xenity.h"

class CameraFollow : public MonoBehaviour
{
public:
	CameraFollow();
	void Start() override;
	void Update() override;
	ReflectiveData GetReflectiveData() override;
	std::weak_ptr<GameObject> cameraTarget;
	Vector3 offset = Vector3(3, 4, 3);
	float moveSpeed = 5.0f;

private:
};
