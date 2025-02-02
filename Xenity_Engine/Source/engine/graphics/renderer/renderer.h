// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

/**
 * [Internal]
 */

#include <glm/fwd.hpp>

#include <vector>

#include <engine/api.h>
#include <engine/graphics/shader.h>
#include <engine/graphics/3d_graphics/mesh_data.h>
#include <engine/lighting/lighting.h>
#include <engine/graphics/material_rendering_modes.h>

class Color;
class Vector4;
class Vector3;
class Vector2;
class Camera;
class Texture;
class Material;
struct LightsIndices;

struct RenderingSettings
{
	MaterialRenderingModes renderingMode = MaterialRenderingModes::Opaque;
	bool useDepth = false;
	bool invertFaces = false;
	bool useTexture = true;
	bool useLighting = true;
	bool max_depth = false; // Use the maximum depth value for the depth test to not render on top of other objects
	bool wireframe = false;
};

enum class PolygoneFillMode
{
	Fill,
	Line
};

enum class CullFace
{
	Front,
	Back,
	Front_And_Back,
};

enum class DrawMode
{
	Patches,
	Triangles,
	Quads,
};

class API Renderer
{
public:
	virtual ~Renderer() = default;
	Renderer& operator=(const Renderer&) = delete;

	virtual int Init() = 0;
	virtual void Setup() = 0;
	virtual void Stop() = 0;

	virtual void NewFrame() = 0;
	virtual void EndFrame() = 0;
	virtual void SetClearColor(const Color& color) = 0;
	virtual void Clear() = 0;

	// Fog
	virtual void SetFog(bool active) = 0;
	virtual void SetFogValues(float start, float end, const Color& color) = 0;

	// Projection
	virtual void SetViewport(int x, int y, int width, int height) = 0;
	virtual void SetProjection2D(float projectionSize, float nearClippingPlane, float farClippingPlane) = 0;
	virtual void SetProjection3D(float fov, float nearClippingPlane, float farClippingPlane, float aspect) = 0;
	virtual void SetCameraPosition(const Camera& camera) = 0;
	virtual void SetCameraPosition(const Vector3& position, const Vector3& rotation) = 0;
	virtual void ResetView() = 0;

	// Transform
	virtual void ResetTransform() = 0;
	virtual void SetTransform(const Vector3& position, const Vector3& rotation, const Vector3& scale, bool resetTransform) = 0;
	virtual void SetTransform(const glm::mat4& mat) = 0;

	// Texture
	virtual unsigned int CreateNewTexture() = 0;
	virtual void BindTexture(const Texture& texture) = 0;
	virtual void SetTextureData(const Texture& texture, unsigned int textureType, const unsigned char* buffer) = 0;
	virtual void DeleteTexture(Texture& texture) = 0;

	// Mesh
	virtual void UploadMeshData(MeshData& meshData) = 0;
	virtual void DeleteSubMeshData(MeshData::SubMesh& subMesh) = 0;
	virtual void DrawSubMesh(const MeshData::SubMesh& subMesh, const Material& material, RenderingSettings& settings) = 0;
	virtual void DrawSubMesh(const MeshData::SubMesh& subMesh, const Material& material, const Texture& texture, RenderingSettings& settings) = 0;
	virtual void DrawLine(const Vector3& a, const Vector3& b, const Color& color, RenderingSettings& settings) = 0;

	virtual void Setlights(const LightsIndices& lightsIndices) = 0;

	//Shader
	virtual void UseShaderProgram(unsigned int programId) {}

private:
	virtual void SetLight(const int lightIndex, const Light& light, const Vector3& lightPosition, const Vector3& lightDirection) = 0;
};
