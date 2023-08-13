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

	void SetFov(const float fov);
	float GetFov() const;
	void SetProjectionSize(const float value);
	float GetProjectionSize() const;
	float GetNearClippingPlane() const;
	float GetFarClippingPlane() const;
	void SetNearClippingPlane(float value);
	void SetFarClippingPlane(float value);

	Vector2 ScreenTo2DWorld(int x, int y);
	Vector2 MouseTo2DWorld();
	glm::mat4 &GetProjection();
	glm::mat4 &GetUnscaledProjection();
	void SetProjectionType(ProjectionTypes type);
	ProjectionTypes GetProjectionType();
	void UpdateProjection();
	void ChangeFrameBufferSize(Vector2Int resolution);
	void UpdaterameBuffer();
	void BindFrameBuffer();

	int GetWidth()
	{
		return width;
	}

	int GetHeight()
	{
		return height;
	}

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