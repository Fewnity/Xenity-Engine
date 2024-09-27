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

	m_fov = 60;
	m_isProjectionDirty = true;

#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
	glGenFramebuffers(1, &m_framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
	glGenFramebuffers(1, &m_secondFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_secondFramebuffer);
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
	static const Quaternion offsetQ = Quaternion::Euler(0, 180, 0);
	const Quaternion newQ = baseQ * offsetQ;

	m_cameraTransformMatrix = glm::toMat4(glm::quat(newQ.w, -newQ.x, newQ.y, newQ.z));

	if (position.x != 0.0f || position.y != 0.0f || position.z != 0.0f)
		m_cameraTransformMatrix = glm::translate(m_cameraTransformMatrix, glm::vec3(position.x, -position.y, -position.z));
}

void Camera::RemoveReferences()
{
	Graphics::RemoveCamera(std::dynamic_pointer_cast<Camera>(shared_from_this()));
}

ReflectiveData Camera::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, m_projectionType, "projectionType", true);
	Reflective::AddVariable(reflectedVariables, m_fov, "fov", m_projectionType == ProjectionTypes::Perspective);
	Reflective::AddVariable(reflectedVariables, m_projectionSize, "projectionSize", m_projectionType == ProjectionTypes::Orthographic);
	Reflective::AddVariable(reflectedVariables, m_nearClippingPlane, "nearClippingPlane", true);
	Reflective::AddVariable(reflectedVariables, m_farClippingPlane, "farClippingPlane", true);
	Reflective::AddVariable(reflectedVariables, m_useMultisampling, "useMultisampling", true);
	return reflectedVariables;
}

void Camera::OnReflectionUpdated()
{
	m_isProjectionDirty = true;
	if (m_lastMultisamplingValue != m_useMultisampling)
	{
		m_lastMultisamplingValue = m_useMultisampling;
		m_needFrameBufferUpdate = true;
	}
}

Camera::~Camera()
{
#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
	if (m_framebuffer != -1)
	{
		glDeleteFramebuffers(1, &m_framebuffer);
	}
	if (m_secondFramebuffer != -1)
	{
		glDeleteFramebuffers(1, &m_secondFramebuffer);
	}
	if (m_framebufferTexture != -1)
	{
		glDeleteTextures(1, &m_framebufferTexture);
	}
	if (m_secondFramebufferTexture != -1)
	{
		glDeleteTextures(1, &m_secondFramebufferTexture);
	}
	if (m_depthframebuffer != -1)
	{
		glDeleteRenderbuffers(1, &m_depthframebuffer);
	}
#endif
	AssetManager::RemoveReflection(this);
	GetTransformRaw()->GetOnTransformUpdated().Unbind(&Camera::UpdateCameraTransformMatrix, this);
}

#pragma endregion

#pragma region Accessors

void Camera::SetFov(const float fov)
{
	m_fov = fov;
	m_isProjectionDirty = true;
}

float Camera::GetFov() const
{
	return m_fov;
}

void Camera::SetProjectionSize(const float value)
{
	m_projectionSize = value;
	m_isProjectionDirty = true;
}

void Camera::SetNearClippingPlane(float value)
{
	if (value >= m_farClippingPlane)
	{
		m_farClippingPlane = value + 0.01f;
	}
	m_nearClippingPlane = value;
	m_isProjectionDirty = true;
}

void Camera::SetFarClippingPlane(float value)
{
	if (value <= m_nearClippingPlane)
	{
		m_farClippingPlane = value + 0.01f;
	}
	else
	{
		m_farClippingPlane = value;
	}
	m_isProjectionDirty = true;
}

Vector2 Camera::ScreenTo2DWorld(int x, int y)
{
	const Vector3& camPos = GetTransformRaw()->GetPosition();
	const float vx = (x - m_width / 2.0f) / (m_width / 10.f / m_aspect / m_projectionSize * 5.0f) + camPos.x;
	const float vy = -(y - m_height / 2.0f) / (m_height / 10.f / m_projectionSize * 5.0f) + camPos.y;
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
		if (m_projectionType == ProjectionTypes::Perspective)
		{
			Engine::GetRenderer().SetProjection3D(m_fov, m_nearClippingPlane, m_farClippingPlane, m_aspect);
		}
		else
		{
			Engine::GetRenderer().SetProjection2D(m_projectionSize, m_nearClippingPlane, m_farClippingPlane);
		}
	}

	if (m_isProjectionDirty)
	{
		m_isProjectionDirty = false;
		if (m_projectionType == ProjectionTypes::Perspective) // 3D projection
		{
			m_projection = glm::perspective(glm::radians(m_fov), m_aspect, m_nearClippingPlane, m_farClippingPlane);
		}
		else // 2D projection
		{
			const float halfAspect = GetAspectRatio() / 2.0f * GetProjectionSize() / 5.0f;
			const float halfOne = 0.5f * GetProjectionSize() / 5.0f;
			m_projection = glm::orthoZO(-halfAspect, halfAspect, -halfOne, halfOne, m_nearClippingPlane, m_farClippingPlane);
			m_projection = glm::scale(m_projection, glm::vec3(1 / 10.0f, 1 / 10.0f, 1));
			//projection = glm::scale(projection, glm::vec3(1 / (5.0f * GetAspectRatio() * 1.054f), 1 / 10.0f, 1)); // 1.054f is needed for correct size but why?
		}

		// Create canvas projection
		const float fixedProjectionSize = 5;
		const float halfAspect = GetAspectRatio() / 2.0f * 10 * fixedProjectionSize / 5.0f;
		const float halfOne = 0.5f * 10 * fixedProjectionSize / 5.0f;
		m_canvasProjection = glm::orthoZO(-halfAspect, halfAspect, -halfOne, halfOne, 0.03f, 100.0f);
	}
}

Matrix4x4 createViewMatrix(const Vector3& cameraPosition, const Vector3& targetPosition, const Vector3& upVector)
{
	Vector3 forward = (targetPosition - cameraPosition).Normalized();
	forward = -forward;
	const Vector3 right = upVector.Cross(forward).Normalized();
	const Vector3 up = forward.Cross(right);

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
	const Matrix4x4 vm = createViewMatrix(GetTransformRaw()->GetPosition(), GetTransformRaw()->GetPosition() + GetTransformRaw()->GetForward(), GetTransformRaw()->GetUp());
	frustum.ExtractPlanes((float*)&GetProjection(), vm.m);
}

void Camera::SetProjectionType(const ProjectionTypes type)
{
	m_projectionType = type;
	m_isProjectionDirty = true;
}

void Camera::UpdateFrameBuffer()
{
#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
	if (m_needFrameBufferUpdate)
	{
		if (m_framebufferTexture != -1)
		{
			glDeleteTextures(1, &m_framebufferTexture);
			m_framebufferTexture = -1;
		}
		if (m_secondFramebufferTexture != -1)
		{
			glDeleteTextures(1, &m_secondFramebufferTexture);
			m_secondFramebufferTexture = -1;
		}
		if (m_depthframebuffer != -1)
		{
			glDeleteRenderbuffers(1, &m_depthframebuffer);
			m_depthframebuffer = -1;
		}

		if (m_useMultisampling)
		{
			int sample = 8;
			glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
			glGenTextures(1, &m_framebufferTexture);
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_framebufferTexture);
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, sample, GL_RGB, m_width, m_height, GL_TRUE);
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_framebufferTexture, 0);

			glGenRenderbuffers(1, &m_depthframebuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, m_depthframebuffer);
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, sample, GL_DEPTH_COMPONENT, m_width, m_height);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthframebuffer);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				Debug::PrintError("[Camera::UpdateFrameBuffer] Framebuffer not created", true);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			//Screen buffer
			glBindFramebuffer(GL_FRAMEBUFFER, m_secondFramebuffer);
			glGenTextures(1, &m_secondFramebufferTexture);
			glBindTexture(GL_TEXTURE_2D, m_secondFramebufferTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glBindTexture(GL_TEXTURE_2D, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_secondFramebufferTexture, 0);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				Debug::PrintError("[Camera::UpdateFrameBuffer] Framebuffer not created", true);
		}
		else
		{
			glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
			glGenTextures(1, &m_secondFramebufferTexture);
			glBindTexture(GL_TEXTURE_2D, m_secondFramebufferTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glBindTexture(GL_TEXTURE_2D, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_secondFramebufferTexture, 0);

			glGenRenderbuffers(1, &m_depthframebuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, m_depthframebuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthframebuffer);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				Debug::PrintError("[Camera::UpdateFrameBuffer] Framebuffer not created", true);
		}

		m_needFrameBufferUpdate = false;
	}
#endif
}

void Camera::ChangeFrameBufferSize(const Vector2Int& resolution)
{
	if (m_width != resolution.x || m_height != resolution.y)
	{
		m_width = resolution.x;
		m_height = resolution.y;
		m_aspect = (float)m_width / (float)m_height;

		m_needFrameBufferUpdate = true;
		m_isProjectionDirty = true;
		UpdateProjection();
#if defined(__PSP__)
		Engine::GetRenderer().SetViewport(0, 0, m_width, m_height);
#endif
	}
}

void Camera::BindFrameBuffer()
{
#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
	UpdateFrameBuffer();
#if defined(EDITOR)
	if (m_framebuffer != -1)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
	}
#else
	if (m_useMultisampling)
	{
		if (m_framebuffer != -1)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
		}
	}
	else
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
#endif
#endif

#if !defined(__PSP__)
	Engine::GetRenderer().SetViewport(0, 0, m_width, m_height);
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
	const Quaternion& baseQ = GetTransformRaw()->GetRotation();
	static const Quaternion offsetQ = Quaternion::Euler(180, 0, 0);
	const Quaternion newQ = baseQ * offsetQ;

	const glm::mat4 cameraModelMatrix = glm::toMat4(glm::quat(newQ.w, -newQ.x, newQ.y, newQ.z));

	// Get screen mouse position (inverted)
	const glm::vec3 mousePositionGLM = glm::vec3(m_width -InputSystem::mousePosition.x, InputSystem::mousePosition.y, 0.0f); // Invert Y for OpenGL coordinates

	// Get world mouse position (position at the near clipping plane)
	const glm::vec3 vec3worldCoords = glm::unProject(mousePositionGLM, cameraModelMatrix, m_projection, glm::vec4(0, 0, m_width, m_height));

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
	const glm::vec3 topLeftNear = glm::unProject(glm::vec3(0, 0, 0.0f), cameraModelMatrix, m_projection, screenSizeNorm);
	const glm::vec3 topLeftFar = glm::unProject(glm::vec3(0, 0, 1.0f), cameraModelMatrix, m_projection, screenSizeNorm);

	Gizmo::DrawLine(Vector3(-topLeftNear.x, topLeftNear.y, topLeftNear.z), Vector3(-topLeftFar.x, topLeftFar.y, topLeftFar.z));

	//Top right
	const glm::vec3 topRightNear = glm::unProject(glm::vec3(1, 0, 0.0f), cameraModelMatrix, m_projection, screenSizeNorm);
	const glm::vec3 topRightFar = glm::unProject(glm::vec3(1, 0, 1.0f), cameraModelMatrix, m_projection, screenSizeNorm);

	Gizmo::DrawLine(Vector3(-topRightNear.x, topRightNear.y, topRightNear.z) , Vector3(-topRightFar.x, topRightFar.y, topRightFar.z) );

	//Bottom left
	const glm::vec3 bottomLeftNear = glm::unProject(glm::vec3(0, 1, 0.0f), cameraModelMatrix, m_projection, screenSizeNorm);
	const glm::vec3 bottomLeftFar = glm::unProject(glm::vec3(0, 1, 1.0f), cameraModelMatrix, m_projection, screenSizeNorm);

	Gizmo::DrawLine(Vector3(-bottomLeftNear.x, bottomLeftNear.y, bottomLeftNear.z) , Vector3(-bottomLeftFar.x, bottomLeftFar.y, bottomLeftFar.z) );

	//Bottom right
	const glm::vec3 bottomRightNear = glm::unProject(glm::vec3(1, 1, 0.0f), cameraModelMatrix, m_projection, screenSizeNorm);
	const glm::vec3 bottomRightFar = glm::unProject(glm::vec3(1, 1, 1.0f), cameraModelMatrix, m_projection, screenSizeNorm);

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
	if (m_useMultisampling)
	{
#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_framebuffer);
#if defined(EDITOR)
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_secondFramebuffer);
#else
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
#endif
		glBlitFramebuffer(0, 0, GetWidth(), GetHeight(), 0, 0, GetWidth(), GetHeight(), GL_COLOR_BUFFER_BIT, GL_LINEAR);
#endif
	}
}

#pragma endregion
