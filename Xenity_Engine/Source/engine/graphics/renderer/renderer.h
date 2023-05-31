#pragma once

#include "../texture.h"
// #include "../shader.h"
// #include <glm/glm.hpp>
#include <vector>

class Color;
class Vector4;
class Vector3;
class Vector2;
class Camera;
// class MeshData;

enum PolygoneFillMode
{
	Fill,
	Line
};

enum CullFace
{
	Front,
	Back,
	Front_And_Back,
};

enum BufferType
{
	Array_Buffer,
	Element_Array_Buffer,
};

enum BufferMode
{
	Static,
	Dynamic,
};

enum DrawMode
{
	Patches,
	Triangles,
	Quads,
};

class Renderer
{
public:
	Renderer();
	virtual int Init() = 0;
	virtual void Stop() = 0;
	virtual void NewFrame() = 0;
	virtual void EndFrame() = 0;
	virtual void SetClearColor(Color color) = 0;
	virtual void SetProjection2D(float projectionSize, float nearClippingPlane, float farClippingPlane) = 0;
	virtual void SetProjection3D(float fov, float nearClippingPlane, float farClippingPlane) = 0;
	virtual void SetCameraPosition(Camera *camera) = 0;
	virtual void ResetTransform() = 0;
	virtual void SetTransform(Vector3 position, Vector3 rotation, Vector3 scale, bool resetTransform) = 0;
	virtual void Clear() = 0;
};
