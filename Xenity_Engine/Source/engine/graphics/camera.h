#pragma once

#include "../vectors/vector3.h"
#include "../component.h"
#include "../vectors/vector2.h"
#include "camera_projection_types.h"

class Camera : public Component
{
public:
	Camera();
	~Camera();
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
	glm::mat4& GetUnscaledProjection();
	void SetProjectionType(ProjectionTypes type);
	ProjectionTypes GetProjectionType();
	void UpdateProjection();

private:
	float fov = 60.0f; //For 3D
	float projectionSize = 5; //For 2D
	float nearClippingPlane = 0.3f;
	float farClippingPlane = 1000;
	ProjectionTypes projectionType = Perspective;

	glm::mat4 projection;
	glm::mat4 unscaledProjection;
};