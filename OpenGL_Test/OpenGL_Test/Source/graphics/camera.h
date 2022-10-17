#pragma once
#ifndef CAMERA_H /* Include guard */
#define CAMERA_H

#include "../vectors/vector3.h"
#include "../component.h"

class Camera : public Component
{
public:
	Camera();
	//Vector3 position;
	//Vector3 rotation;
	Vector3 GetSphericalCoordinate();
	Vector3 GetSphericalCoordinate2();
	void SetFov(double fov);
	double GetFov();
private:
	double fov;
};

#endif