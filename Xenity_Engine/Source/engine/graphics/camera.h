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
	* Set field of view
	* @param Field of view angle
	*/
	void SetFov(const float fov);

	/**
	* Get field of view
	*/
	float GetFov() const;

	/**
	* Set projection size
	* @param value Projection size
	*/
	void SetProjectionSize(const float value);

	/**
	* Get projection size
	*/
	float GetProjectionSize() const;

	/**
	* Get near clipping plane
	*/
	float GetNearClippingPlane() const;

	/**
	* Get far clipping plane
	*/
	float GetFarClippingPlane() const;

	/**
	* Set near clipping plane
	* @param value Neaar clipping plane
	*/
	void SetNearClippingPlane(float value);

	/**
	* Set far clipping plane
	* @param value Far clipping plane
	*/
	void SetFarClippingPlane(float value);

	/**
	* Get 2D world position from pixel coordinate
	* @param x X pixel position
	* @param x Y pixel position
	*/
	Vector2 ScreenTo2DWorld(int x, int y);

	/**
	* Get 2D world position from mouse's position
	*/
	Vector2 MouseTo2DWorld();

	/**
	* Get projection matrix
	*/
	glm::mat4& GetProjection();

	/**
	* [Internal] Get projection matrix without Clipping Planes values
	*/
	glm::mat4& GetCanvasProjection();

	/**
	* Set projection type
	* @param type Projection type
	*/
	void SetProjectionType(ProjectionTypes type);

	/**
	* Get projection type
	*/
	ProjectionTypes GetProjectionType() const;

	/**
	* [Internal] Update projection matrix
	*/
	void UpdateProjection();

	/**
	* [Internal] Change Frame buffer size in pixel
	* @param resolution The new resolution
	*/
	void ChangeFrameBufferSize(const Vector2Int& resolution);

	/**
	* [Internal] Update FrameBuffer
	*/
	void UpdateFrameBuffer();

	/**
	* [Internal] Bind view frame buffer
	*/
	void BindFrameBuffer();

	Vector3 GetMouseRay();

	void OnDrawGizmosSelected() override;
	void OnDrawGizmos() override;

	/**
	* Get view width in pixel
	*/
	int GetWidth() const
	{
		return width;
	}

	/**
	* Get view height in pixel
	*/
	int GetHeight() const
	{
		return height;
	}

	/**
	* Get view aspect ratio
	*/
	float GetAspectRatio() const
	{
		return aspect;
	}


	// [Internal]
	unsigned int secondFramebufferTexture = -1;

private:
	unsigned int framebufferTexture = -1;

public:
	glm::mat4 projection;
	glm::mat4 canvasProjection;
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

public:
	bool useMultisampling = true;
	// [Internal]
	bool lastMultisamplingValue = useMultisampling;
	// [Internal]
	bool isProjectionDirty = true;
	// [Internal]
	bool isEditor = false;
};