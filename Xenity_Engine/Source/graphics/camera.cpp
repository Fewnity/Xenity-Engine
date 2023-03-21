#include "camera.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include "graphics.h"
#include "../tools/math.h"
#include "../inputs/input_system.h"
#include "../ui/window.h"

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

void Camera::SetProjectionSize(const double value)
{
	projectionSize = value;
}

double Camera::GetProjectionSize() const
{
	return projectionSize;
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

Vector2 Camera::ScreenTo2DWorld(int x, int y)
{
	float aspect = Window::GetAspectRatio();
	float cameraX = Graphics::usedCamera->gameObject->transform.GetPosition().x;
	float cameraY = Graphics::usedCamera->gameObject->transform.GetPosition().y;

	float vx = (x - Window::GetWidth() / 2.0f) / (Window::GetWidth() / 10.f / aspect / Graphics::usedCamera->GetProjectionSize() * 5.0f) + cameraX;
	float vy = -(y - Window::GetHeight() / 2.0f) / (Window::GetHeight() / 10.f / Graphics::usedCamera->GetProjectionSize() * 5.0f) + cameraY;
	return Vector2(vx, vy);
}

Vector2 Camera::MouseTo2DWorld()
{
	return ScreenTo2DWorld(InputSystem::mousePosition.x, InputSystem::mousePosition.y);
}

#pragma endregion

