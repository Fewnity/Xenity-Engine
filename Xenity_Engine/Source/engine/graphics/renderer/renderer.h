#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include "../texture.h"
// #include "../shader.h"
#include <glm/glm.hpp>
#include <vector>
#include "../../lighting/lighting.h"

class Color;
class Vector4;
class Vector3;
class Vector2;
class Camera;
class Texture;
class MeshData;

class RenderingSettings
{
public:
	bool useDepth = false;
	bool useBlend = false;
	bool invertFaces = false;
	bool useTexture = true;
	bool useLighting = true;
};

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

class API Renderer
{
public:
	Renderer();
	virtual int Init() = 0;
	virtual void Setup() = 0;
	virtual void Stop() = 0;
	virtual void NewFrame() = 0;
	virtual void EndFrame() = 0;
	virtual void SetViewport(int x, int y, int width, int height) = 0;
	virtual void SetClearColor(Color color) = 0;
	virtual void SetProjection2D(float projectionSize, float nearClippingPlane, float farClippingPlane) = 0;
	virtual void SetProjection3D(float fov, float nearClippingPlane, float farClippingPlane, float aspect) = 0;
	virtual void ResetView() = 0;
	virtual void SetCameraPosition(std::weak_ptr<Camera> camera) = 0;
	virtual void ResetTransform() = 0;
	virtual void SetTransform(Vector3 position, Vector3 rotation, Vector3 scale, bool resetTransform) = 0;
	virtual void SetTransform(glm::mat4 &mat) = 0;
	virtual void MoveTransform(Vector3 position) = 0;
	virtual void BindTexture(Texture *texture) = 0;
	virtual void ApplyTextureFilters(Texture *texture) = 0;
	virtual void DrawMeshData(MeshData *meshData, RenderingSettings settings) = 0;
	virtual unsigned int CreateNewTexture() = 0;
	virtual void SetTextureData(Texture *texture, unsigned int textureType, const unsigned char *buffer) = 0;
	virtual void Clear() = 0;

private:
	virtual void Setlights(std::weak_ptr<Camera> camera) = 0;
	virtual void SetLight(int lightIndex, Vector3 lightPosition, float intensity, Color color, Light::LightType type, float attenuation) = 0;
};
