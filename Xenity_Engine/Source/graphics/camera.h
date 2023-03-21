#pragma once

#include "../vectors/vector3.h"
#include "../component.h"
#include "../vectors/vector2.h"

class Camera : public Component
{
public:
	Camera();
	~Camera();
	void SetFov(const double fov);
	double GetFov() const;
	void SetProjectionSize(const double value);
	double GetProjectionSize() const;
	double GetNearClippingPlane() const;
	double GetFarClippingPlane() const;
	void SetNearClippingPlane(double value);
	void SetFarClippingPlane(double value);

	Vector2 ScreenTo2DWorld(int x, int y);
	Vector2 MouseTo2DWorld();

private:
	double fov = 60.0f; //For 3D
	double projectionSize = 5; //For 2D
	double nearClippingPlane = 0.3f;
	double farClippingPlane = 1000;
};