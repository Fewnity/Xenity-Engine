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
	UpdateProjection();
}

Camera::~Camera()
{
}

#pragma endregion

#pragma region Accessors

void Camera::SetFov(const float fov)
{
	this->fov = fov;
	UpdateProjection();
}

float Camera::GetFov() const
{
	return this->fov;
}

void Camera::SetProjectionSize(const float value)
{
	projectionSize = value;
	UpdateProjection();
}

float Camera::GetProjectionSize() const
{
	return projectionSize;
}

float Camera::GetNearClippingPlane() const
{
	return nearClippingPlane;
}

float Camera::GetFarClippingPlane() const
{
	return farClippingPlane;
}

void Camera::SetNearClippingPlane(float value)
{
	if (value >= farClippingPlane)
	{
		farClippingPlane = value + 0.01f;
	}
	nearClippingPlane = value;
	UpdateProjection();
}

void Camera::SetFarClippingPlane(float value)
{
	if (value <= nearClippingPlane)
	{
		farClippingPlane = value + 0.01f;
	}
	farClippingPlane = value;
	UpdateProjection();
}

Vector2 Camera::ScreenTo2DWorld(int x, int y)
{
	float aspect = Window::GetAspectRatio();
	float cameraX = gameObject->transform.GetPosition().x;
	float cameraY = gameObject->transform.GetPosition().y;

	float vx = (x - Window::GetWidth() / 2.0f) / (Window::GetWidth() / 10.f / aspect / projectionSize * 5.0f) + cameraX;
	float vy = -(y - Window::GetHeight() / 2.0f) / (Window::GetHeight() / 10.f / projectionSize * 5.0f) + cameraY;
	return Vector2(vx, vy);
}

Vector2 Camera::MouseTo2DWorld()
{
	return ScreenTo2DWorld((int)InputSystem::mousePosition.x, (int)InputSystem::mousePosition.y);
}

void Camera::UpdateProjection() 
{
	if (projectionType == ProjectionTypes::Perspective) 
	{
		//Projection
		projection = glm::perspective(glm::radians(fov), Window::GetAspectRatio(), nearClippingPlane, farClippingPlane);
	}
	else {
		float halfAspect = Window::GetAspectRatio() / 2.0f * Graphics::usedCamera->GetProjectionSize() / 5.0f;
		float halfOne = 0.5f * Graphics::usedCamera->GetProjectionSize() / 5.0f;
		projection = glm::ortho(-halfAspect, halfAspect, -halfOne, halfOne);

		//Unscaled version for canvas
		float halfAspectUnscaled = Window::GetAspectRatio() / 2.0f;
		unscaledProjection = glm::ortho(-halfAspectUnscaled, halfAspectUnscaled, -0.5f, 0.5f);
	}
}

glm::mat4& Camera::GetProjection()
{
	return projection;
}

glm::mat4& Camera::GetUnscaledProjection()
{
	return unscaledProjection;
}

void Camera::SetProjectionType(ProjectionTypes type)
{
	projectionType = type;
}

ProjectionTypes Camera::GetProjectionType()
{
	return projectionType;
}

#pragma endregion

