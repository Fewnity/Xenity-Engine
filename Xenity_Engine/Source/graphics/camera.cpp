#include "camera.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include "graphics.h"
#include "../tools/math.h"

#pragma region Constructors / Destructor

Camera::Camera() 
{
	this->fov = 60;
	if (Graphics::usedCamera == nullptr) 
	{
		Graphics::usedCamera = this;
	}
}

Camera::~Camera()
{
}

#pragma endregion

#pragma region Accessors

void Camera::SetFov(const double fov)
{
	this->fov = fov;
}

double Camera::GetFov() const
{
	return this->fov;
}

#pragma endregion

