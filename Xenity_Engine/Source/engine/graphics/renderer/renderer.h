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
#include <memory>
#include "../shader.h"

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
	virtual void SetClearColor(const Color& color) = 0;
	virtual void SetProjection2D(float projectionSize, float nearClippingPlane, float farClippingPlane) = 0;
	virtual void SetProjection3D(float fov, float nearClippingPlane, float farClippingPlane, float aspect) = 0;
	virtual void ResetView() = 0;
	virtual void SetCameraPosition(std::weak_ptr<Camera> camera) = 0;
	virtual void ResetTransform() = 0;
	virtual void SetTransform(const Vector3& position, const Vector3& rotation, const Vector3& scale, bool resetTransform) = 0;
	virtual void SetTransform(const glm::mat4& mat) = 0;
	virtual void MoveTransform(const Vector3& position) = 0;
	virtual void BindTexture(std::shared_ptr <Texture> texture) = 0;
	virtual void ApplyTextureFilters(std::shared_ptr<Texture> texture) = 0;
	virtual void DeleteTexture(Texture* texture) = 0;
	virtual void DrawMeshData(std::shared_ptr<MeshData >meshData, std::vector<std::shared_ptr<Texture>> textures, RenderingSettings& settings) = 0;
	virtual void DrawLine(const Vector3& a, const Vector3& b, const Color& color, RenderingSettings& settings) = 0;
	virtual unsigned int CreateNewTexture() = 0;
	virtual void SetTextureData(std::shared_ptr < Texture >texture, unsigned int textureType, const unsigned char* buffer) = 0;
	virtual void Clear() = 0;
	virtual void SetFog(bool active) = 0;
	virtual void SetFogValues(float start, float end, Color color) = 0;

	virtual unsigned CreateBuffer() = 0;
	virtual void BindBuffer(BufferType type, unsigned int bufferId) = 0;
	virtual void DeleteBuffer(unsigned int bufferId) = 0;
	virtual void UploadMeshData(std::shared_ptr<MeshData> meshData) = 0;

	//Shader
	virtual unsigned int CreateShader(Shader::ShaderType type) = 0;
	virtual unsigned int CreateShaderProgram() = 0;
	virtual void CompileShader(unsigned int shaderId) = 0;
	virtual int GetShaderCompilationResult(unsigned int shaderId) = 0;
	virtual std::vector<char> GetCompilationError(unsigned int shaderId) = 0;
	virtual void SetShaderData(unsigned int shaderId, const char* data) = 0;
	virtual void DeleteShader(unsigned int shaderId) = 0;
	virtual void DeleteShaderProgram(unsigned int programId) = 0;
	virtual void LinkShaderProgram(unsigned int programId) = 0;

	virtual void UseShaderProgram(unsigned int programId) = 0;
	virtual unsigned int GetShaderUniformLocation(unsigned int programId, const char* name) = 0;
	virtual void AttachShader(unsigned int programId, unsigned int shaderId) = 0;

	virtual void SetShaderAttribut(unsigned int programId, const char* attribut, const Vector4& value) = 0;
	virtual void SetShaderAttribut(unsigned int programId, const char* attribut, const Vector3& value) = 0;
	virtual void SetShaderAttribut(unsigned int programId, const char* attribut, const Vector2& value) = 0;
	virtual void SetShaderAttribut(unsigned int programId, const char* attribut, const float value) = 0;
	virtual void SetShaderAttribut(unsigned int programId, const char* attribut, const int value) = 0;
	virtual void SetShaderAttribut(unsigned int programId, const char* attribut, const glm::mat4& trans) = 0;
	virtual void SetShaderAttribut(unsigned int programId, const char* attribut, const glm::mat3& trans) = 0;

private:
	virtual void Setlights(std::weak_ptr<Camera> camera) = 0;
	virtual void SetLight(int lightIndex, Vector3 lightPosition, float intensity, Color color, Light::LightType type, float attenuation) = 0;
};
