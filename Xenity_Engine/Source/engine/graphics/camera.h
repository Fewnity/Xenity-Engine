// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <glm/glm.hpp>

#include <engine/api.h>
#include <engine/component.h>
#include <engine/vectors/vector3.h>
#include <engine/vectors/vector2.h>
#include <engine/vectors/vector2_int.h>
#include "camera_projection_types.h"

struct Plane
{
	float A, B, C, D;

	void Normalize() {
		float length = sqrt(A * A + B * B + C * C);
		A /= length;
		B /= length;
		C /= length;
		D /= length;
	}
};

struct Matrix4x4 {
	float m[16];

	static Matrix4x4 identity() {
		return { 1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1 };
	}
};

struct Frustum
{
	Plane planes[6];

	void ExtractPlanes(const float* projMatrix, const float* viewMatrix)
	{
		float clip[16];
		// Multiply the projection matrix by the view matrix to get the clipping matrix
		for (int i = 0; i < 4; ++i) 
		{
			for (int j = 0; j < 4; ++j) 
			{
				clip[i * 4 + j] = viewMatrix[i * 4 + 0] * projMatrix[0 * 4 + j] +
					viewMatrix[i * 4 + 1] * projMatrix[1 * 4 + j] +
					viewMatrix[i * 4 + 2] * projMatrix[2 * 4 + j] +
					viewMatrix[i * 4 + 3] * projMatrix[3 * 4 + j];
			}
		}

		// Extract the right plane
		planes[0].A = clip[3] - clip[0];
		planes[0].B = clip[7] - clip[4];
		planes[0].C = clip[11] - clip[8];
		planes[0].D = clip[15] - clip[12];
		planes[0].Normalize();

		// Extract the left plane
		planes[1].A = clip[3] + clip[0];
		planes[1].B = clip[7] + clip[4];
		planes[1].C = clip[11] + clip[8];
		planes[1].D = clip[15] + clip[12];
		planes[1].Normalize();

		// Extract the bottom plane
		planes[2].A = clip[3] + clip[1];
		planes[2].B = clip[7] + clip[5];
		planes[2].C = clip[11] + clip[9];
		planes[2].D = clip[15] + clip[13];
		planes[2].Normalize();

		// Extract the top plane
		planes[3].A = clip[3] - clip[1];
		planes[3].B = clip[7] - clip[5];
		planes[3].C = clip[11] - clip[9];
		planes[3].D = clip[15] - clip[13];
		planes[3].Normalize();

		// Extract the far plane
		planes[4].A = clip[3] - clip[2];
		planes[4].B = clip[7] - clip[6];
		planes[4].C = clip[11] - clip[10];
		planes[4].D = clip[15] - clip[14];
		planes[4].Normalize();

		// Extract the near plane
		planes[5].A = clip[3] + clip[2];
		planes[5].B = clip[7] + clip[6];
		planes[5].C = clip[11] + clip[10];
		planes[5].D = clip[15] + clip[14];
		planes[5].Normalize();
	}
};

class API Camera : public Component
{
public:
	Camera();
	~Camera();

	Frustum frustum;

	/**
	* @brief Set field of view
	* @param Field of view angle
	*/
	void SetFov(const float fov);

	/**
	* @brief Get field of view
	*/
	float GetFov() const;

	/**
	* @brief Set projection size
	* @param value Projection size
	*/
	void SetProjectionSize(const float value);

	/**
	* @brief Get projection size
	*/
	float GetProjectionSize() const
	{
		return m_projectionSize;
	}

	/**
	* @brief Get near clipping plane
	*/
	float GetNearClippingPlane() const
	{
		return m_nearClippingPlane;
	}

	/**
	* @brief Get far clipping plane
	*/
	float GetFarClippingPlane() const
	{
		return m_farClippingPlane;
	}

	/**
	* @brief Set near clipping plane
	* @param value Neaar clipping plane
	*/
	void SetNearClippingPlane(float value);

	/**
	* @brief Set far clipping plane
	* @param value Far clipping plane
	*/
	void SetFarClippingPlane(float value);

	/**
	* @brief Get 2D world position from pixel coordinate
	* @param x X pixel position
	* @param x Y pixel position
	*/
	Vector2 ScreenTo2DWorld(int x, int y);

	/**
	* @brief Get 2D world position from mouse's position
	*/
	Vector2 MouseTo2DWorld();

	/**
	* @brief Set projection type
	* @param type Projection type
	*/
	void SetProjectionType(const ProjectionTypes type);

	/**
	* @brief Get projection matrix
	*/
	const glm::mat4& GetProjection() const
	{
		return m_projection;
	}

	/**
	* @brief Get projection type
	*/
	ProjectionTypes GetProjectionType() const
	{
		return m_projectionType;
	}

	Vector3 GetMouseRay();

	/**
	* @brief Get view width in pixel
	*/
	int GetWidth() const
	{
		return m_width;
	}

	/**
	* @brief Get view height in pixel
	*/
	int GetHeight() const
	{
		return m_height;
	}

	/**
	* @brief Get view aspect ratio
	*/
	float GetAspectRatio() const
	{
		return m_aspect;
	}

	/**
	* @brief Get if the camera is using multisampling (Windows Only)
	*/
	bool GetUseMultisampling() const
	{
		return m_useMultisampling;
	}

	/**
	* @brief Set if the camera is using multisampling (Windows Only)
	* @param _UseMultisampling True to enable Multisampling
	*/
	void SetUseMultisampling(bool useMultisampling)
	{
		m_useMultisampling = useMultisampling;
	}

	void OnComponentAttached() override;

	void UpdateCameraTransformMatrix();

protected:
	friend class SceneMenu;
	friend class GameMenu;
	friend class Shader;
	friend class RendererOpengl;
	friend class RendererGU;
	friend class RendererGsKit;
	friend class RendererVU1;
	friend class RendererRSX;
	friend class Graphics;
	friend class SceneManager;
	friend class Window;
	friend class SpriteManager;

	void RemoveReferences() override;

	ReflectiveData GetReflectiveData() override;
	void OnReflectionUpdated() override;
	void OnDrawGizmosSelected() override;
	void OnDrawGizmos() override;

	/**
	* @brief [Internal] Update projection matrix
	*/
	void UpdateProjection();

	void UpdateFrustum();

	/**
	* @brief [Internal] Get projection matrix without Clipping Planes values
	*/
	const glm::mat4& GetCanvasProjection() const
	{
		return m_canvasProjection;
	}

	/**
	* @brief [Internal] Change Frame buffer size in pixel
	* @param resolution The new resolution
	*/
	void ChangeFrameBufferSize(const Vector2Int& resolution);

	/**
	* @brief [Internal] Update FrameBuffer
	*/
	void UpdateFrameBuffer();

	/**
	* @brief [Internal] Bind view frame buffer
	*/
	void BindFrameBuffer();

	// [Internal]
	unsigned int m_secondFramebufferTexture = -1;

	unsigned int m_framebufferTexture = -1;

	glm::mat4 m_projection;
	glm::mat4 m_canvasProjection;
	glm::mat4 m_cameraTransformMatrix;

	/**
	* @brief [Internal]
	*/
	void CopyMultiSampledFrameBuffer();

	unsigned int m_framebuffer = -1;
	unsigned int m_secondFramebuffer = -1;
	int m_width, m_height;
	float m_aspect;
	float m_fov = 60.0f;		  // For 3D
	float m_projectionSize = 5; // For 2D
	float m_nearClippingPlane = 0.3f;
	float m_farClippingPlane = 1000;
	ProjectionTypes m_projectionType = ProjectionTypes::Perspective;

	unsigned int m_depthframebuffer = -1;
	bool m_needFrameBufferUpdate = true;

	bool m_useMultisampling = true;
	// [Internal]
	bool m_isProjectionDirty = true;
	// [Internal]
	bool m_lastMultisamplingValue = m_useMultisampling;
	// [Internal]
	bool m_isEditor = false;

	/**
	* [Internal] Get if the camera is for the editor
	*/
	bool IsEditor() const
	{
		return m_isEditor;
	}

	/**
	* [Internal] Set if the camera is for the editor
	*/
	void SetIsEditor(bool isEditor)
	{
		m_isEditor = isEditor;
	}
};