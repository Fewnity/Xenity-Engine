#pragma once

#include "../vectors/vector3.h"
#include "../component.h"

class Camera : public Component
{
public:
	Camera();
	~Camera();
	void SetFov(const double fov);
	double GetFov() const;
private:
	double fov = 60;
};