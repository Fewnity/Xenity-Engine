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
#include "../inputs/input_system.h"
#include "../ui/window.h"
#if defined(_WIN32) || defined(_WIN64)
#include <glad/glad.h>
#endif

#pragma region Constructors / Destructor

Camera::Camera()
{
	componentName = "Camera";
	//SetReflection();

	this->fov = 60;
	UpdateProjection();

#if defined(EDITOR)
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
#endif
	ChangeFrameBufferSize(Vector2Int(Window::GetWidth(), Window::GetHeight()));
}

/*void Camera::SetReflection()
{
	reflectedVariables["fov"] = &fov;
	reflectedVariables["projectionSize"] = &projectionSize;
	reflectedVariables["nearClippingPlane"] = &nearClippingPlane;
	reflectedVariables["farClippingPlane"] = &farClippingPlane;
}*/

std::unordered_map<std::string, Variable> Camera::GetReflection()
{
	std::unordered_map<std::string, Variable> reflectedVariables;
	reflectedVariables.insert_or_assign("fov", fov);
	reflectedVariables.insert_or_assign("projectionSize", projectionSize);
	reflectedVariables.insert_or_assign("nearClippingPlane", nearClippingPlane);
	reflectedVariables.insert_or_assign("farClippingPlane", farClippingPlane);
	return reflectedVariables;
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
	 //float aspect = Window::GetAspectRatio();
	 float cameraX = GetTransform()->GetPosition().x;
	 float cameraY = GetTransform()->GetPosition().y;

	 //float vx = (x - Window::GetWidth() / 2.0f) / (Window::GetWidth() / 10.f / aspect / projectionSize * 5.0f) + cameraX;
	 //float vy = -(y - Window::GetHeight() / 2.0f) / (Window::GetHeight() / 10.f / projectionSize * 5.0f) + cameraY;
	 float vx = (x - width / 2.0f) / (width / 10.f / aspect / projectionSize * 5.0f) + cameraX;
	 float vy = -(y - height / 2.0f) / (height / 10.f / projectionSize * 5.0f) + cameraY;
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
		Engine::renderer->SetProjection3D(fov, nearClippingPlane, farClippingPlane, aspect);
	}
	else
	{
		Engine::renderer->SetProjection2D(projectionSize, nearClippingPlane, farClippingPlane);
	}
#if defined(EDITOR)
	 if (projectionType == ProjectionTypes::Perspective)
	 {
	 	// Projection
	 	projection = glm::perspective(glm::radians(fov), aspect, nearClippingPlane, farClippingPlane);
	 }
	 else
	 {
	 	float halfAspect = Window::GetAspectRatio() / 2.0f * GetProjectionSize() / 5.0f;
	 	float halfOne = 0.5f * GetProjectionSize() / 5.0f;
	 	projection = glm::orthoZO(-halfAspect, halfAspect, -halfOne, halfOne, nearClippingPlane, farClippingPlane);

	 	// Unscaled version for canvas
	 	float halfAspectUnscaled = Window::GetAspectRatio() / 2.0f;
	 	unscaledProjection = glm::ortho(-halfAspectUnscaled, halfAspectUnscaled, -0.5f, 0.5f);
	 }
#endif
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


void Camera::UpdaterameBuffer()
{
#if defined(_WIN32) || defined(_WIN64)
	if (needFremeBufferUpdate)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		if (framebufferTexture >= 0)
		{
			glDeleteTextures(1, &framebufferTexture);
		}
		if (depthframebuffer >= 0)
		{
			glDeleteRenderbuffers(1, &depthframebuffer);
		}

		glGenTextures(1, &framebufferTexture);
		glBindTexture(GL_TEXTURE_2D, framebufferTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, framebufferSize.x, framebufferSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glGenRenderbuffers(1, &depthframebuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthframebuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, framebufferSize.x, framebufferSize.y);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTexture, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthframebuffer);
		//Window::SetResolution(framebufferSize.x, framebufferSize.y);
		needFremeBufferUpdate = false;
	}
#endif
}

void Camera::ChangeFrameBufferSize(Vector2Int resolution)
{
	if (framebufferSize != resolution)
	{
		width = resolution.x;
		height = resolution.y;
		aspect = (float)width / (float)height;

		framebufferSize = resolution;
		needFremeBufferUpdate = true;
		UpdateProjection();
#if defined(__PSP__)
		Engine::renderer->SetViewport(0, 0, width, height);
#endif
	}
}

void Camera::BindFrameBuffer() 
{
#if defined(_WIN32) || defined(_WIN64)
	if(framebuffer != -1)
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
#endif
	UpdaterameBuffer();

#if !defined(EDITOR)
#if !defined(__PSP__)
	Engine::renderer->SetViewport(0, 0, width, height);
#endif
#else
	glViewport(0, 0, framebufferSize.x, framebufferSize.y);
#endif
}

#pragma endregion
