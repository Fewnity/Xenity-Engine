#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include "../vectors/vector3.h"
#include "../component.h"
#include "../vectors/vector2.h"
#include "../vectors/vector2_int.h"
#include "camera_projection_types.h"
#include <glm/glm.hpp>

class API Camera : public Component
{
public:
	Camera();
	~Camera();
	//void SetReflection();

	std::unordered_map<std::string, Variable> GetReflection();

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
	glm::mat4 &GetProjection();

	/**
	* Get projection matrix without Clipping Planes values
	*/
	glm::mat4 &GetUnscaledProjection();

	/**
	* Set projection type
	* @param type Projection type
	*/
	void SetProjectionType(ProjectionTypes type);

	/**
	* Get projection type
	*/
	ProjectionTypes GetProjectionType();

	/**
	* Update projection matrix
	*/
	void UpdateProjection();

	/**
	* Change Frame buffer size in pixel
	* @param
	*/
	void ChangeFrameBufferSize(Vector2Int resolution);

	/**
	* Update FrameBuffer
	*/
	void UpdateFrameBuffer();

	/**
	* Bind view frame buffer
	*/
	void BindFrameBuffer();

	/**
	* Get view width in pixel
	*/
	int GetWidth()
	{
		return width;
	}

	/**
	* Get view height in pixel
	*/
	int GetHeight()
	{
		return height;
	}

	/**
	* Get view aspect ratio
	*/
	float GetAspectRatio()
	{
		return aspect;
	}

	Vector2Int framebufferSize;
	bool isEditor = false;
	unsigned int framebufferTexture = -1;
	glm::mat4 projection;
	glm::mat4 unscaledProjection;
private:
	int width, height;
	float aspect;
	float fov = 60.0f;		  // For 3D
	float projectionSize = 5; // For 2D
	float nearClippingPlane = 0.3f;
	float farClippingPlane = 1000;
	ProjectionTypes projectionType = Perspective;

	unsigned int framebuffer = -1;
	unsigned int depthframebuffer = -1;
	bool needFremeBufferUpdate = true;
};