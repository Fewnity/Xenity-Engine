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

double Camera::GetNearClippingPlane() const
{
	return nearClippingPlane;
}

double Camera::GetFarClippingPlane() const
{
	return farClippingPlane;
}

void Camera::SetNearClippingPlane(double value)
{
	if (value >= farClippingPlane)
	{
		farClippingPlane = value + 0.01;
	}
	nearClippingPlane = value;
}

void Camera::SetFarClippingPlane(double value)
{
	if (value <= nearClippingPlane)
	{
		farClippingPlane = value + 0.01;
	}
	farClippingPlane = value;
}

#pragma endregion

