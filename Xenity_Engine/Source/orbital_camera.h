#pragma once

#include "component.h"

class Camera;
class Transform;

class OrbitalCamera : public Component
{
public:
	OrbitalCamera();
	Camera* camera = nullptr;
	Transform* target = nullptr;
	void Update();
};

