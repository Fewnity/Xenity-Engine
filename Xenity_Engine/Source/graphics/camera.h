#pragma once

#include "../vectors/vector3.h"
#include "../component.h"

class Camera : public Component
{
public:
	Camera();
	~Camera();
	Vector3 GetSphericalCoordinate();
	Vector3 GetSphericalCoordinate2();
	void SetFov(double fov);
	double GetFov();
private:
	double fov = 60;
};