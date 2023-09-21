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
#include "../asset_management/asset_manager.h"
#include "2d_graphics/sprite_manager.h"

#if defined(EDITOR)
#include "../../xenity_editor.h"
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
	AssetManager::AddReflection(this);
}

/*void Camera::SetReflection()
{
	reflectedVariables["fov"] = &fov;
	reflectedVariables["projectionSize"] = &projectionSize;
	reflectedVariables["nearClippingPlane"] = &nearClippingPlane;
	reflectedVariables["farClippingPlane"] = &farClippingPlane;
}*/

std::unordered_map<std::string, ReflectionEntry> Camera::GetReflection()
{
	std::unordered_map<std::string, ReflectionEntry> reflectedVariables;
	Reflection::AddReflectionVariable(reflectedVariables, fov, "fov", true);
	Reflection::AddReflectionVariable(reflectedVariables, projectionSize, "projectionSize", true);
	Reflection::AddReflectionVariable(reflectedVariables, nearClippingPlane, "nearClippingPlane", true);
	Reflection::AddReflectionVariable(reflectedVariables, farClippingPlane, "farClippingPlane", true);
	Reflection::AddReflectionVariable(reflectedVariables, (int&)projectionType, "projectionType", true);
	return reflectedVariables;
}

Camera::~Camera()
{
#if defined(EDITOR)
	if (framebuffer >= 0)
	{
		glDeleteFramebuffers(1, &framebuffer);
	}
	if (framebufferTexture >= 0)
	{
		glDeleteTextures(1, &framebufferTexture);
	}
	if (depthframebuffer >= 0)
	{
		glDeleteRenderbuffers(1, &depthframebuffer);
	}
#endif
	AssetManager::RemoveReflection(this);
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
	Vector3 camPos = GetTransform()->GetPosition();
	float vx = (x - width / 2.0f) / (width / 10.f / aspect / projectionSize * 5.0f) + camPos.x;
	float vy = -(y - height / 2.0f) / (height / 10.f / projectionSize * 5.0f) + camPos.y;
	return Vector2(vx, vy);
}

Vector2 Camera::MouseTo2DWorld()
{
	return ScreenTo2DWorld((int)InputSystem::mousePosition.x, (int)InputSystem::mousePosition.y);
}

void Camera::UpdateProjection()
{
	if (Engine::UseOpenGLFixedFunctions)
	{
		if (projectionType == ProjectionTypes::Perspective)
		{
			Engine::renderer->SetProjection3D(fov, nearClippingPlane, farClippingPlane, aspect);
		}
		else
		{
			Engine::renderer->SetProjection2D(projectionSize, nearClippingPlane, farClippingPlane);
		}
#if defined (EDITOR)
		projection = glm::perspective(glm::radians(fov), aspect, nearClippingPlane, farClippingPlane);
#endif
	}
	else
	{
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
	UpdateProjection();
}

ProjectionTypes Camera::GetProjectionType()
{
	return projectionType;
}


void Camera::UpdateFrameBuffer()
{
#if defined(_WIN32) || defined(_WIN64)
	if (needFrameBufferUpdate)
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
		needFrameBufferUpdate = false;
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
		needFrameBufferUpdate = true;
		UpdateProjection();
#if defined(__PSP__)
		Engine::renderer->SetViewport(0, 0, width, height);
#endif
	}
}

void Camera::BindFrameBuffer()
{
#if defined(_WIN32) || defined(_WIN64)
	if (framebuffer != -1)
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
#endif
	UpdateFrameBuffer();

#if !defined(EDITOR)
#if !defined(__PSP__)
	Engine::renderer->SetViewport(0, 0, width, height);
#endif
#else
	glViewport(0, 0, framebufferSize.x, framebufferSize.y);
#endif
}

void Camera::OnDrawGizmos()
{
#if defined(EDITOR)
	auto transform = GetTransform();
	float distance = Vector3::Distance(transform->GetPosition(), Graphics::usedCamera.lock()->GetTransform()->GetPosition());
	float alpha = 1;
	if (distance <= 1.3f)
		alpha = distance - 0.3f;

	SpriteManager::DrawSprite(transform->GetPosition(), Graphics::usedCamera.lock()->GetTransform()->GetRotation(), Vector3(0.2f), EditorUI::icons[Icon_Camera], Color::CreateFromRGBAFloat(1, 1, 1, alpha));

	if (Engine::selectedGameObject.lock() && Engine::selectedGameObject.lock() == GetGameObject())
	{
		Color lineColor = Color::CreateFromRGBAFloat(1, 1, 1, 1);
		Gizmo::SetColor(lineColor);

		std::shared_ptr<Camera> camera = std::dynamic_pointer_cast<Camera>(shared_from_this());

		Engine::renderer->SetCameraPosition(Graphics::usedCamera.lock());

		Vector3 cameraPosition = camera->GetTransform()->GetPosition();
		Vector3 cameraRotation = camera->GetTransform()->GetRotation();
		glm::mat4 cameraModelMatrix = glm::mat4(1.0f);
		if (projectionType == Orthographic)
			cameraModelMatrix = glm::scale(cameraModelMatrix, glm::vec3(1 / (5.0f * camera->aspect * 1.054f), 1 / 10.0f, 1)); // 1.054f is needed for correct size but why?
		cameraModelMatrix = glm::rotate(cameraModelMatrix, glm::radians(cameraRotation.z * -1), glm::vec3(0.0f, 0.0f, 1.0f));
		cameraModelMatrix = glm::rotate(cameraModelMatrix, glm::radians(cameraRotation.x * -1), glm::vec3(1.0f, 0.0f, 0.0f));
		cameraModelMatrix = glm::rotate(cameraModelMatrix, glm::radians(cameraRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		cameraModelMatrix = glm::translate(cameraModelMatrix, glm::vec3(-cameraPosition.x, cameraPosition.y, cameraPosition.z));

		glm::vec4 screenSizeNorm = glm::vec4(0, 0, 1, 1);

		//Top left
		glm::vec3 topLeftNear = glm::unProject(glm::vec3(0, 0, 0.5f), cameraModelMatrix, camera->projection, screenSizeNorm);
		glm::vec3 topLeftFar = glm::unProject(glm::vec3(0, 0, 1.0f), cameraModelMatrix, camera->projection, screenSizeNorm);

		Gizmo::DrawLine(Vector3(topLeftNear.x, topLeftNear.y, topLeftNear.z) * -1, Vector3(topLeftFar.x, topLeftFar.y, topLeftFar.z) * -1);

		//Top right
		glm::vec3 topRightNear = glm::unProject(glm::vec3(1, 0, 0.5f), cameraModelMatrix, camera->projection, screenSizeNorm);
		glm::vec3 topRightFar = glm::unProject(glm::vec3(1, 0, 1.0f), cameraModelMatrix, camera->projection, screenSizeNorm);

		Gizmo::DrawLine(Vector3(topRightNear.x, topRightNear.y, topRightNear.z) * -1, Vector3(topRightFar.x, topRightFar.y, topRightFar.z) * -1);

		//Bottom left
		glm::vec3 bottomLeftNear = glm::unProject(glm::vec3(0, 1, 0.5f), cameraModelMatrix, camera->projection, screenSizeNorm);
		glm::vec3 bottomLeftFar = glm::unProject(glm::vec3(0, 1, 1.0f), cameraModelMatrix, camera->projection, screenSizeNorm);

		Gizmo::DrawLine(Vector3(bottomLeftNear.x, bottomLeftNear.y, bottomLeftNear.z) * -1, Vector3(bottomLeftFar.x, bottomLeftFar.y, bottomLeftFar.z) * -1);

		//Bottom right
		glm::vec3 bottomRightNear = glm::unProject(glm::vec3(1, 1, 0.5f), cameraModelMatrix, camera->projection, screenSizeNorm);
		glm::vec3 bottomRightFar = glm::unProject(glm::vec3(1, 1, 1.0f), cameraModelMatrix, camera->projection, screenSizeNorm);

		Gizmo::DrawLine(Vector3(bottomRightNear.x, bottomRightNear.y, bottomRightNear.z) * -1, Vector3(bottomRightFar.x, bottomRightFar.y, bottomRightFar.z) * -1);


		Gizmo::DrawLine(Vector3(topLeftFar.x, topLeftFar.y, topLeftFar.z) * -1, Vector3(topRightFar.x, topRightFar.y, topRightFar.z) * -1);
		Gizmo::DrawLine(Vector3(topLeftNear.x, topLeftNear.y, topLeftNear.z) * -1, Vector3(topRightNear.x, topRightNear.y, topRightNear.z) * -1);

		Gizmo::DrawLine(Vector3(bottomLeftFar.x, bottomLeftFar.y, bottomLeftFar.z) * -1, Vector3(bottomRightFar.x, bottomRightFar.y, bottomRightFar.z) * -1);
		Gizmo::DrawLine(Vector3(bottomLeftNear.x, bottomLeftNear.y, bottomLeftNear.z) * -1, Vector3(bottomRightNear.x, bottomRightNear.y, bottomRightNear.z) * -1);

		Gizmo::DrawLine(Vector3(bottomLeftFar.x, bottomLeftFar.y, bottomLeftFar.z) * -1, Vector3(topLeftFar.x, topLeftFar.y, topLeftFar.z) * -1);
		Gizmo::DrawLine(Vector3(bottomRightFar.x, bottomRightFar.y, bottomRightFar.z) * -1, Vector3(topRightFar.x, topRightFar.y, topRightFar.z) * -1);

		Gizmo::DrawLine(Vector3(bottomLeftNear.x, bottomLeftNear.y, bottomLeftNear.z) * -1, Vector3(topLeftNear.x, topLeftNear.y, topLeftNear.z) * -1);
		Gizmo::DrawLine(Vector3(bottomRightNear.x, bottomRightNear.y, bottomRightNear.z) * -1, Vector3(topRightNear.x, topRightNear.y, topRightNear.z) * -1);
	}
#endif
}

#pragma endregion
