// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "camera.h"

#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
#include <glad/glad.h>
#endif
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#define _USE_MATH_DEFINES
#include <math.h>

#if defined(EDITOR)
#include <editor/gizmo.h>
#include <editor/ui/editor_ui.h>
#endif

#include <engine/graphics/renderer/renderer.h>
#include <engine/engine.h>
#include <engine/inputs/input_system.h>
#include <engine/ui/window.h>
#include <engine/game_elements/transform.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/debug/debug.h>
#include "graphics.h"

#pragma region Constructors / Destructor

Camera::Camera()
{
	m_componentName = "Camera";

	this->fov = 60;
	isProjectionDirty = true;

#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glGenFramebuffers(1, &secondFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, secondFramebuffer);
#endif

	ChangeFrameBufferSize(Vector2Int(Window::GetWidth(), Window::GetHeight()));
	AssetManager::AddReflection(this);
}

void Camera::OnComponentAttached()
{
	GetTransformRaw()->GetOnTransformUpdated().Bind(&Camera::UpdateCameraTransformMatrix, this);
}

void Camera::UpdateCameraTransformMatrix()
{
	const Vector3& position = GetTransformRaw()->GetPosition();
	const Quaternion& baseQ = GetTransformRaw()->GetRotation();
	const Quaternion offsetQ = Quaternion::Euler(0, 180, 0);
	Quaternion newQ = baseQ * offsetQ;

	cameraTransformMatrix = glm::toMat4(glm::quat(newQ.w, -newQ.x, newQ.y, newQ.z));

	if (position.x != 0.0f || position.y != 0.0f || position.z != 0.0f)
		cameraTransformMatrix = glm::translate(cameraTransformMatrix, glm::vec3(position.x, -position.y, -position.z));
}

void Camera::RemoveReferences()
{
	Graphics::RemoveCamera(std::dynamic_pointer_cast<Camera>(shared_from_this()));
}

ReflectiveData Camera::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, projectionType, "projectionType", true);
	Reflective::AddVariable(reflectedVariables, fov, "fov", projectionType == ProjectionTypes::Perspective);
	Reflective::AddVariable(reflectedVariables, projectionSize, "projectionSize", projectionType == ProjectionTypes::Orthographic);
	Reflective::AddVariable(reflectedVariables, nearClippingPlane, "nearClippingPlane", true);
	Reflective::AddVariable(reflectedVariables, farClippingPlane, "farClippingPlane", true);
	Reflective::AddVariable(reflectedVariables, useMultisampling, "useMultisampling", true);
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
#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
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
	GetTransformRaw()->GetOnTransformUpdated().Unbind(&Camera::UpdateCameraTransformMatrix, this);
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
	return fov;
}

void Camera::SetProjectionSize(const float value)
{
	projectionSize = value;
	isProjectionDirty = true;
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
	const Vector3& camPos = GetTransformRaw()->GetPosition();
	const float vx = (x - width / 2.0f) / (width / 10.f / aspect / projectionSize * 5.0f) + camPos.x;
	const float vy = -(y - height / 2.0f) / (height / 10.f / projectionSize * 5.0f) + camPos.y;
	return Vector2(vx, vy);
}

Vector2 Camera::MouseTo2DWorld()
{
	return ScreenTo2DWorld((int)InputSystem::mousePosition.x, (int)InputSystem::mousePosition.y);
}

void Camera::UpdateProjection()
{
	if constexpr (Graphics::UseOpenGLFixedFunctions)
	{
		if (projectionType == ProjectionTypes::Perspective)
		{
			Engine::GetRenderer().SetProjection3D(fov, nearClippingPlane, farClippingPlane, aspect);
		}
		else
		{
			Engine::GetRenderer().SetProjection2D(projectionSize, nearClippingPlane, farClippingPlane);
		}
	}

	if (isProjectionDirty)
	{
		isProjectionDirty = false;
		if (projectionType == ProjectionTypes::Perspective) // 3D projection
		{
			projection = glm::perspective(glm::radians(fov), aspect, nearClippingPlane, farClippingPlane);
		}
		else // 2D projection
		{
			const float halfAspect = GetAspectRatio() / 2.0f * GetProjectionSize() / 5.0f;
			const float halfOne = 0.5f * GetProjectionSize() / 5.0f;
			projection = glm::orthoZO(-halfAspect, halfAspect, -halfOne, halfOne, nearClippingPlane, farClippingPlane);
			projection = glm::scale(projection, glm::vec3(1 / 10.0f, 1 / 10.0f, 1));
			//projection = glm::scale(projection, glm::vec3(1 / (5.0f * GetAspectRatio() * 1.054f), 1 / 10.0f, 1)); // 1.054f is needed for correct size but why?
		}

		// Create canvas projection
		const float fixedProjectionSize = 5;
		const float halfAspect = GetAspectRatio() / 2.0f * 10 * fixedProjectionSize / 5.0f;
		const float halfOne = 0.5f * 10 * fixedProjectionSize / 5.0f;
		canvasProjection = glm::orthoZO(-halfAspect, halfAspect, -halfOne, halfOne, 0.03f, 100.0f);
	}
}

Matrix4x4 createViewMatrix(const Vector3& cameraPosition, const Vector3& targetPosition, const Vector3& upVector)
{
	Vector3 forward = (targetPosition - cameraPosition).Normalized();
	forward = -forward;
	Vector3 right = upVector.Cross(forward).Normalized();
	Vector3 up = forward.Cross(right);

	Matrix4x4 viewMatrix = Matrix4x4::identity();

	viewMatrix.m[0] = right.x;
	viewMatrix.m[1] = up.x;
	viewMatrix.m[2] = forward.x;

	viewMatrix.m[4] = right.y;
	viewMatrix.m[5] = up.y;
	viewMatrix.m[6] = forward.y;

	viewMatrix.m[8] = right.z;
	viewMatrix.m[9] = up.z;
	viewMatrix.m[10] = forward.z;

	viewMatrix.m[12] = static_cast<float>(-right.Dot(cameraPosition));
	viewMatrix.m[13] = static_cast<float>(-up.Dot(cameraPosition));
	viewMatrix.m[14] = static_cast<float>(-forward.Dot(cameraPosition));

	return viewMatrix;
}

void Camera::UpdateFrustum()
{
	Matrix4x4 vm = createViewMatrix(GetTransformRaw()->GetPosition(), GetTransformRaw()->GetPosition() + GetTransformRaw()->GetForward(), GetTransformRaw()->GetUp());
	frustum.ExtractPlanes((float*)&GetProjection(), vm.m);
}

void Camera::SetProjectionType(const ProjectionTypes type)
{
	projectionType = type;
	isProjectionDirty = true;
}

void Camera::UpdateFrameBuffer()
{
#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
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
				Debug::PrintError("[Camera::UpdateFrameBuffer] Framebuffer not created", true);
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
				Debug::PrintError("[Camera::UpdateFrameBuffer] Framebuffer not created", true);
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
				Debug::PrintError("[Camera::UpdateFrameBuffer] Framebuffer not created", true);
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
#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
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
	Gizmo::DrawBillboard(GetTransformRaw()->GetPosition(), Vector2(0.2f), EditorUI::icons[(int)IconName::Icon_Camera], Color::CreateFromRGBFloat(1, 1, 1));
#endif
}

Vector3 Camera::GetMouseRay()
{
	const std::shared_ptr<Transform> cameraTransform = GetTransform();

	const Quaternion& baseQ = cameraTransform->GetRotation();
	Quaternion offsetQ = Quaternion::Euler(180, 0, 0);
	Quaternion newQ = baseQ * offsetQ;

	glm::mat4 cameraModelMatrix = glm::toMat4(glm::quat(newQ.w, -newQ.x, newQ.y, newQ.z));

	// Get screen mouse position (inverted)
	const glm::vec3 mousePositionGLM = glm::vec3(width -InputSystem::mousePosition.x, InputSystem::mousePosition.y, 0.0f); // Invert Y for OpenGL coordinates

	// Get world mouse position (position at the near clipping plane)
	const glm::vec3 vec3worldCoords = glm::unProject(mousePositionGLM, cameraModelMatrix, projection, glm::vec4(0, 0, width, height));

	// Normalise direction
	return Vector3(-vec3worldCoords.x, vec3worldCoords.y, vec3worldCoords.z).Normalized();
}

void Camera::OnDrawGizmosSelected()
{
#if defined(EDITOR)
	const Color lineColor = Color::CreateFromRGBAFloat(1, 1, 1, 1);
	Gizmo::SetColor(lineColor);

	Engine::GetRenderer().SetCameraPosition(*Graphics::usedCamera);

	const Vector3& cameraPosition = GetTransformRaw()->GetPosition();
	const Vector3& cameraRotation = GetTransformRaw()->GetEulerAngles();
	glm::mat4 cameraModelMatrix = glm::mat4(1.0f);
	cameraModelMatrix = glm::rotate(cameraModelMatrix, glm::radians(-cameraRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	cameraModelMatrix = glm::rotate(cameraModelMatrix, glm::radians(cameraRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	cameraModelMatrix = glm::rotate(cameraModelMatrix, glm::radians(cameraRotation.y+180), glm::vec3(0.0f, 1.0f, 0.0f));
	cameraModelMatrix = glm::translate(cameraModelMatrix, glm::vec3(cameraPosition.x, -cameraPosition.y, -cameraPosition.z));

	const glm::vec4 screenSizeNorm = glm::vec4(0, 0, 1, 1);

	//Top left
	const glm::vec3 topLeftNear = glm::unProject(glm::vec3(0, 0, 0.0f), cameraModelMatrix, projection, screenSizeNorm);
	const glm::vec3 topLeftFar = glm::unProject(glm::vec3(0, 0, 1.0f), cameraModelMatrix, projection, screenSizeNorm);

	Gizmo::DrawLine(Vector3(-topLeftNear.x, topLeftNear.y, topLeftNear.z), Vector3(-topLeftFar.x, topLeftFar.y, topLeftFar.z));

	//Top right
	const glm::vec3 topRightNear = glm::unProject(glm::vec3(1, 0, 0.0f), cameraModelMatrix, projection, screenSizeNorm);
	const glm::vec3 topRightFar = glm::unProject(glm::vec3(1, 0, 1.0f), cameraModelMatrix, projection, screenSizeNorm);

	Gizmo::DrawLine(Vector3(-topRightNear.x, topRightNear.y, topRightNear.z) , Vector3(-topRightFar.x, topRightFar.y, topRightFar.z) );

	//Bottom left
	const glm::vec3 bottomLeftNear = glm::unProject(glm::vec3(0, 1, 0.0f), cameraModelMatrix, projection, screenSizeNorm);
	const glm::vec3 bottomLeftFar = glm::unProject(glm::vec3(0, 1, 1.0f), cameraModelMatrix, projection, screenSizeNorm);

	Gizmo::DrawLine(Vector3(-bottomLeftNear.x, bottomLeftNear.y, bottomLeftNear.z) , Vector3(-bottomLeftFar.x, bottomLeftFar.y, bottomLeftFar.z) );

	//Bottom right
	const glm::vec3 bottomRightNear = glm::unProject(glm::vec3(1, 1, 0.0f), cameraModelMatrix, projection, screenSizeNorm);
	const glm::vec3 bottomRightFar = glm::unProject(glm::vec3(1, 1, 1.0f), cameraModelMatrix, projection, screenSizeNorm);

	Gizmo::DrawLine(Vector3(-bottomRightNear.x, bottomRightNear.y, bottomRightNear.z) , Vector3(-bottomRightFar.x, bottomRightFar.y, bottomRightFar.z) );


	Gizmo::DrawLine(Vector3(-topLeftFar.x, topLeftFar.y, topLeftFar.z) , Vector3(-topRightFar.x, topRightFar.y, topRightFar.z) );
	Gizmo::DrawLine(Vector3(-topLeftNear.x, topLeftNear.y, topLeftNear.z) , Vector3(-topRightNear.x, topRightNear.y, topRightNear.z) );

	Gizmo::DrawLine(Vector3(-bottomLeftFar.x, bottomLeftFar.y, bottomLeftFar.z), Vector3(-bottomRightFar.x, bottomRightFar.y, bottomRightFar.z) );
	Gizmo::DrawLine(Vector3(-bottomLeftNear.x, bottomLeftNear.y, bottomLeftNear.z) , Vector3(-bottomRightNear.x, bottomRightNear.y, bottomRightNear.z) );

	Gizmo::DrawLine(Vector3(-bottomLeftFar.x, bottomLeftFar.y, bottomLeftFar.z), Vector3(-topLeftFar.x, topLeftFar.y, topLeftFar.z) );
	Gizmo::DrawLine(Vector3(-bottomRightFar.x, bottomRightFar.y, bottomRightFar.z) , Vector3(-topRightFar.x, topRightFar.y, topRightFar.z) );

	Gizmo::DrawLine(Vector3(-bottomLeftNear.x, bottomLeftNear.y, bottomLeftNear.z) , Vector3(-topLeftNear.x, topLeftNear.y, topLeftNear.z) );
	Gizmo::DrawLine(Vector3(-bottomRightNear.x, bottomRightNear.y, bottomRightNear.z) , Vector3(-topRightNear.x, topRightNear.y, topRightNear.z) );
#endif
}

void Camera::CopyMultiSampledFrameBuffer()
{
	if (useMultisampling)
	{
#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
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
