#include "camera.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include "graphics.h"
#include "../tools/math.h"
#include "../engine.h"
#include "renderer/renderer.h"
// #include "../inputs/input_system.h"
// #include "../ui/window.h"

#include "../game_elements/gameobject.h"

#include <glm/gtc/matrix_transform.hpp>

#pragma region Constructors / Destructor

Camera::Camera()
{
	componentName = "Camera";

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
	// float aspect = Window::GetAspectRatio();
	// float cameraX = GetTransform()->GetPosition().x;
	// float cameraY = GetTransform()->GetPosition().y;

	// float vx = (x - Window::GetWidth() / 2.0f) / (Window::GetWidth() / 10.f / aspect / projectionSize * 5.0f) + cameraX;
	// float vy = -(y - Window::GetHeight() / 2.0f) / (Window::GetHeight() / 10.f / projectionSize * 5.0f) + cameraY;
	// return Vector2(vx, vy);
	return Vector2(0, 0);
}

Vector2 Camera::MouseTo2DWorld()
{
	return ScreenTo2DWorld(0, 0);
	// return ScreenTo2DWorld((int)InputSystem::mousePosition.x, (int)InputSystem::mousePosition.y);
}

#ifdef __PSP__
#include "../../psp/gu2gl.h"
void Camera::Apply()
{
	// glMatrixMode(GL_VIEW);
	// glLoadIdentity();

	// gluRotateZ((-GetTransform()->GetRotation().z) / 180.0f * 3.14159f);
	// gluRotateX(GetTransform()->GetRotation().x / 180.0f * 3.14159f);
	// gluRotateY((GetTransform()->GetRotation().y + 180) / 180.0f * 3.14159f);

	// glTranslatef(GetTransform()->GetPosition().x, -GetTransform()->GetPosition().y, -GetTransform()->GetPosition().z);

	// glMatrixMode(GL_MODEL);
	// glLoadIdentity();
}
#endif

#ifdef __vita__
#include <vitaGL.h>
void Camera::Apply()
{
	// glRotatef(-GetTransform()->GetRotation().z, 0, 0, 1);
	// glRotatef(GetTransform()->GetRotation().x, 1, 0, 0);
	// glRotatef(GetTransform()->GetRotation().y + 180, 0, 1, 0);
	// glTranslatef(GetTransform()->GetPosition().x, -GetTransform()->GetPosition().y, -GetTransform()->GetPosition().z);
}
#endif

void Camera::UpdateProjection()
{
	if (projectionType == ProjectionTypes::Perspective)
	{
		Engine::renderer->SetProjection3D(fov, nearClippingPlane, farClippingPlane);
	}
	else
	{
		Engine::renderer->SetProjection2D(projectionSize, nearClippingPlane, farClippingPlane);
	}

	// if (projectionType == ProjectionTypes::Perspective)
	// {
	// 	// Projection
	// 	projection = glm::perspective(glm::radians(fov), Window::GetAspectRatio(), nearClippingPlane, farClippingPlane);
	// }
	// else
	// {
	// 	float halfAspect = Window::GetAspectRatio() / 2.0f * Graphics::usedCamera->GetProjectionSize() / 5.0f;
	// 	float halfOne = 0.5f * Graphics::usedCamera->GetProjectionSize() / 5.0f;
	// 	projection = glm::orthoZO(-halfAspect, halfAspect, -halfOne, halfOne, nearClippingPlane, farClippingPlane);

	// 	// Unscaled version for canvas
	// 	float halfAspectUnscaled = Window::GetAspectRatio() / 2.0f;
	// 	unscaledProjection = glm::ortho(-halfAspectUnscaled, halfAspectUnscaled, -0.5f, 0.5f);
	// }
}

glm::mat4 &Camera::GetProjection()
{
	return projection;
}

glm::mat4 &Camera::GetUnscaledProjection()
{
	return unscaledProjection;
}

void Camera::SetProjectionType(ProjectionTypes type)
{
	projectionType = type;
	UpdateProjection();
}

ProjectionTypes Camera::GetProjectionType()
{
	return projectionType;
}

#pragma endregion
