#pragma once
#include <engine/api.h>

#include <engine/component.h>

#include <engine/vectors/vector3.h>
#include <engine/vectors/vector2.h>
#include <engine/vectors/vector2_int.h>

#include "camera_projection_types.h"

#include <glm/glm.hpp>

class API Camera : public Component
{
public:
	Camera();
	~Camera();

	void RemoveReferences() override;

	ReflectiveData GetReflectiveData() override;
	void OnReflectionUpdated() override;

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
	float GetProjectionSize() const;

	/**
	* @brief Get near clipping plane
	*/
	float GetNearClippingPlane() const;

	/**
	* @brief Get far clipping plane
	*/
	float GetFarClippingPlane() const;

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
	* @brief Get projection matrix
	*/
	glm::mat4& GetProjection();

	/**
	* @brief [Internal] Get projection matrix without Clipping Planes values
	*/
	glm::mat4& GetCanvasProjection();

	/**
	* @brief Set projection type
	* @param type Projection type
	*/
	void SetProjectionType(ProjectionTypes type);

	/**
	* @brief Get projection type
	*/
	ProjectionTypes GetProjectionType() const;

	/**
	* @brief [Internal] Update projection matrix
	*/
	void UpdateProjection();

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

	Vector3 GetMouseRay();

	void OnDrawGizmosSelected() override;
	void OnDrawGizmos() override;

	/**
	* @brief Get view width in pixel
	*/
	int GetWidth() const
	{
		return width;
	}

	/**
	* @brief Get view height in pixel
	*/
	int GetHeight() const
	{
		return height;
	}

	/**
	* @brief Get view aspect ratio
	*/
	float GetAspectRatio() const
	{
		return aspect;
	}

	/**
	* @brief Get if the camera is using multisampling (Windows Only)
	*/
	bool GetUseMultisampling() 
	{
		return useMultisampling;
	}

	/**
	* @brief Set if the camera is using multisampling (Windows Only)
	* @param _UseMultisampling True to enable Multisampling
	*/
	void SetUseMultisampling(bool _UseMultisampling) 
	{
		useMultisampling = _UseMultisampling;
	}

	// [Internal]
	unsigned int secondFramebufferTexture = -1;

private:
	unsigned int framebufferTexture = -1;

	glm::mat4 projection;
	glm::mat4 canvasProjection;
public:
	/**
	* @brief [Internal]
	*/
	void CopyMultiSampledFrameBuffer();

private:
	unsigned int framebuffer = -1;
	unsigned int secondFramebuffer = -1;
	int width, height;
	float aspect;
	float fov = 60.0f;		  // For 3D
	float projectionSize = 5; // For 2D
	float nearClippingPlane = 0.3f;
	float farClippingPlane = 1000;
	ProjectionTypes projectionType = ProjectionTypes::Perspective;

	unsigned int depthframebuffer = -1;
	bool needFrameBufferUpdate = true;

	bool useMultisampling = true;
	// [Internal]
	bool isProjectionDirty = true;
	// [Internal]
	bool lastMultisamplingValue = useMultisampling;
	// [Internal]
	bool isEditor = false;

public:

	/**
	* [Internal] Get if the camera is for the editor
	*/
	bool GetIsEditor() 
	{
		return isEditor;
	}

	/**
	* [Internal] Set if the camera is for the editor
	*/
	void SetIsEditor(bool _isEditor)
	{
		isEditor = _isEditor;
	}
};