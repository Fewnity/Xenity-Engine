#include "camera.h"

#include "graphics.h"

#include <engine/graphics/renderer/renderer.h>
#include <engine/engine.h>
#include <engine/inputs/input_system.h>
#include <engine/ui/window.h>

#include <engine/game_elements/gameobject.h>
#include <engine/game_elements/transform.h>

#if defined(_WIN32) || defined(_WIN64)
	#include <glad/glad.h>
#endif

#include <engine/asset_management/asset_manager.h>

#if defined(EDITOR)
	#include <editor/editor.h>
	#include <editor/gizmo.h>
	#include <editor/ui/editor_ui.h>
#endif

#include <glm/gtc/matrix_transform.hpp>

#define _USE_MATH_DEFINES
#include <math.h>
#include <engine/debug/debug.h>


#pragma region Constructors / Destructor

Camera::Camera()
{
	componentName = "Camera";

	this->fov = 60;
	isProjectionDirty = true;

#if defined(_WIN32) || defined(_WIN64)
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glGenFramebuffers(1, &secondFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, secondFramebuffer);
#endif

	ChangeFrameBufferSize(Vector2Int(Window::GetWidth(), Window::GetHeight()));
	AssetManager::AddReflection(this);
}

void Camera::RemoveReferences()
{
	Graphics::RemoveCamera(std::dynamic_pointer_cast<Camera>(shared_from_this()));
}

ReflectiveData Camera::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, fov, "fov", true);
	Reflective::AddVariable(reflectedVariables, projectionSize, "projectionSize", true);
	Reflective::AddVariable(reflectedVariables, nearClippingPlane, "nearClippingPlane", true);
	Reflective::AddVariable(reflectedVariables, farClippingPlane, "farClippingPlane", true);
	Reflective::AddVariable(reflectedVariables, useMultisampling, "useMultisampling", true);
	Reflective::AddVariable(reflectedVariables, (int&)projectionType, "projectionType", true);
	return reflectedVariables;
}

void Camera::OnReflectionUpdated()
{
	isProjectionDirty = true;
	if (lastMultisamplingValue != useMultisampling) 
	{
		lastMultisamplingValue = useMultisampling;
		needFrameBufferUpdate = true;
	}
}

Camera::~Camera()
{
#if defined(_WIN32) || defined(_WIN64)
	if (framebuffer != -1)
	{
		glDeleteFramebuffers(1, &framebuffer);
	}
	if (secondFramebuffer != -1)
	{
		glDeleteFramebuffers(1, &secondFramebuffer);
	}
	if (framebufferTexture != -1)
	{
		glDeleteTextures(1, &framebufferTexture);
	}
	if (secondFramebufferTexture != -1)
	{
		glDeleteTextures(1, &secondFramebufferTexture);
	}
	if (depthframebuffer != -1)
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
	isProjectionDirty = true;
}

float Camera::GetFov() const
{
	return this->fov;
}

void Camera::SetProjectionSize(const float value)
{
	projectionSize = value;
	isProjectionDirty = true;
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
	isProjectionDirty = true;
}

void Camera::SetFarClippingPlane(float value)
{
	if (value <= nearClippingPlane)
	{
		farClippingPlane = value + 0.01f;
	}
	else
	{
		farClippingPlane = value;
	}
	isProjectionDirty = true;
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
	if (Graphics::UseOpenGLFixedFunctions)
	{
		if (projectionType == ProjectionTypes::Perspective)
		{
			Engine::GetRenderer().SetProjection3D(fov, nearClippingPlane, farClippingPlane, aspect);
		}
		else
		{
			Engine::GetRenderer().SetProjection2D(projectionSize, nearClippingPlane, farClippingPlane);
		}
#if defined (EDITOR)
		projection = glm::perspective(glm::radians(fov), aspect, nearClippingPlane, farClippingPlane);
#endif
	}
	else if (isProjectionDirty)
	{
		isProjectionDirty = false;
		if (projectionType == ProjectionTypes::Perspective) // 3D projection
		{
			projection = glm::perspective(glm::radians(fov), aspect, nearClippingPlane, farClippingPlane);
		}
		else // 2D projection
		{
			float halfAspect = GetAspectRatio() / 2.0f * GetProjectionSize() / 5.0f;
			float halfOne = 0.5f * GetProjectionSize() / 5.0f;
			projection = glm::orthoZO(-halfAspect, halfAspect, -halfOne, halfOne, nearClippingPlane, farClippingPlane);
		}

		// Create canvas projection
		float fixedProjectionSize = 5;
		float halfAspect = GetAspectRatio() / 2.0f * 10 * fixedProjectionSize / 5.0f;
		float halfOne = 0.5f * 10 * fixedProjectionSize / 5.0f;
		canvasProjection = glm::orthoZO(-halfAspect, halfAspect, -halfOne, halfOne, 0.03f, 100.0f);
	}
}

glm::mat4& Camera::GetProjection()
{
	return projection;
}

glm::mat4& Camera::GetCanvasProjection()
{
	return canvasProjection;
}

void Camera::SetProjectionType(ProjectionTypes type)
{
	projectionType = type;
	isProjectionDirty = true;
}

ProjectionTypes Camera::GetProjectionType() const
{
	return projectionType;
}


void Camera::UpdateFrameBuffer()
{
#if defined(_WIN32) || defined(_WIN64)
	if (needFrameBufferUpdate)
	{
		if (framebufferTexture != -1)
		{
			glDeleteTextures(1, &framebufferTexture);
			framebufferTexture = -1;
		}
		if (secondFramebufferTexture != -1)
		{
			glDeleteTextures(1, &secondFramebufferTexture);
			secondFramebufferTexture = -1;
		}
		if (depthframebuffer != -1)
		{
			glDeleteRenderbuffers(1, &depthframebuffer);
			depthframebuffer = -1;
		}

		if (useMultisampling)
		{
			int sample = 8;
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
			glGenTextures(1, &framebufferTexture);
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, framebufferTexture);
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, sample, GL_RGB, width, height, GL_TRUE);
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, framebufferTexture, 0);

			glGenRenderbuffers(1, &depthframebuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, depthframebuffer);
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, sample, GL_DEPTH_COMPONENT, width, height);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthframebuffer);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				Debug::PrintError("[Camera::UpdateFrameBuffer] Framebuffer not created");
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			//Screen buffer
			glBindFramebuffer(GL_FRAMEBUFFER, secondFramebuffer);
			glGenTextures(1, &secondFramebufferTexture);
			glBindTexture(GL_TEXTURE_2D, secondFramebufferTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glBindTexture(GL_TEXTURE_2D, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, secondFramebufferTexture, 0);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				Debug::PrintError("[Camera::UpdateFrameBuffer] Framebuffer not created");
		}
		else
		{
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
			glGenTextures(1, &secondFramebufferTexture);
			glBindTexture(GL_TEXTURE_2D, secondFramebufferTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glBindTexture(GL_TEXTURE_2D, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, secondFramebufferTexture, 0);

			glGenRenderbuffers(1, &depthframebuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, depthframebuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthframebuffer);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				Debug::PrintError("[Camera::UpdateFrameBuffer] Framebuffer not created");
		}

		needFrameBufferUpdate = false;
	}
#endif
}

void Camera::ChangeFrameBufferSize(const Vector2Int& resolution)
{
	if (width != resolution.x || height != resolution.y)
	{
		width = resolution.x;
		height = resolution.y;
		aspect = (float)width / (float)height;

		needFrameBufferUpdate = true;
		isProjectionDirty = true;
		UpdateProjection();
#if defined(__PSP__)
		Engine::GetRenderer().SetViewport(0, 0, width, height);
#endif
	}
}

void Camera::BindFrameBuffer()
{

#if defined(_WIN32) || defined(_WIN64)
	UpdateFrameBuffer();
#if defined(EDITOR)
	if (framebuffer != -1)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	}
#else
	if (useMultisampling)
	{
		if (framebuffer != -1)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		}
	}
	else
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
#endif
#endif

#if !defined(__PSP__)
	Engine::GetRenderer().SetViewport(0, 0, width, height);
#endif
}

void Camera::OnDrawGizmos()
{
#if defined(EDITOR)
	Gizmo::DrawBillboard(GetTransform()->GetPosition(), Vector2(0.2f), EditorUI::icons[Icon_Camera], Color::CreateFromRGBFloat(1, 1, 1));

	if (Editor::GetSelectedGameObject() && Editor::GetSelectedGameObject() == GetGameObject())
	{
		Color lineColor = Color::CreateFromRGBAFloat(1, 1, 1, 1);
		Gizmo::SetColor(lineColor);

		std::shared_ptr<Camera> camera = std::dynamic_pointer_cast<Camera>(shared_from_this());

		Engine::GetRenderer().SetCameraPosition(Graphics::usedCamera.lock());

		Vector3 cameraPosition = camera->GetTransform()->GetPosition();
		Vector3 cameraRotation = camera->GetTransform()->GetRotation();
		glm::mat4 cameraModelMatrix = glm::mat4(1.0f);
		if (projectionType == Orthographic)
			cameraModelMatrix = glm::scale(cameraModelMatrix, glm::vec3(1 / (5.0f * camera->aspect * 1.054f), 1 / 10.0f, 1)); // 1.054f is needed for correct size but why?
		cameraModelMatrix = glm::rotate(cameraModelMatrix, glm::radians(cameraRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
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

void Camera::CopyMultiSampledFrameBuffer()
{
	if (useMultisampling)
	{
#if defined(_WIN32) || defined(_WIN64)
		glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
#if defined(EDITOR)
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, secondFramebuffer);
#else
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
#endif
		glBlitFramebuffer(0, 0, GetWidth(), GetHeight(), 0, 0, GetWidth(), GetHeight(), GL_COLOR_BUFFER_BIT, GL_LINEAR);
#endif
	}
}

#pragma endregion
