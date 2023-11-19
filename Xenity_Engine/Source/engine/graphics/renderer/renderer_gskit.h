#if defined(_EE)
#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif


#pragma once

#include "renderer.h"
#include "../../lighting/lighting.h"

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

class API RendererGsKit : public Renderer
{
public:
	RendererGsKit();
	int Init() override;
	void Setup() override;
	void Stop() override;
	void NewFrame() override;
	void EndFrame() override;
	void SetViewport(int x, int y, int width, int height) override;
	void SetClearColor(const Color& color) override;
	void SetProjection2D(float projectionSize, float nearClippingPlane, float farClippingPlane) override;
	void SetProjection3D(float fov, float nearClippingPlane, float farClippingPlane, float aspect) override;
	void ResetView() override;
	void SetCameraPosition(const std::shared_ptr<Camera>& camera) override;
	void ResetTransform() override;
	void SetTransform(const Vector3& position, const Vector3& rotation, const Vector3& scale, bool resetTransform) override;
	void SetTransform(const glm::mat4& mat) override;
	void BindTexture(const std::shared_ptr <Texture>& texture) override;
	void DrawMeshData(const std::shared_ptr <MeshData>& meshData, const std::vector<std::shared_ptr<Texture>>& textures, RenderingSettings& settings) override;
	void DrawLine(const Vector3& a, const Vector3& bn, const Color& color, RenderingSettings& settings) override;
	unsigned int CreateNewTexture() override;
	void DeleteTexture(Texture* texture) override;
	void SetTextureData(const std::shared_ptr <Texture>& texture, unsigned int textureType, const unsigned char* buffer) override;
	void Clear() override;
	void SetFog(bool active) override;
	void SetFogValues(float start, float end, const Color& color) override;

	void DeleteSubMeshData(MeshData::SubMesh* subMesh) override;
	void UploadMeshData(const std::shared_ptr<MeshData>& meshData) override;

	//Shader
	unsigned int CreateShader(Shader::ShaderType type) override;
	unsigned int CreateShaderProgram() override;
	void CompileShader(unsigned int shaderId) override;
	int GetShaderCompilationResult(unsigned int shaderId) override;
	std::vector<char> GetCompilationError(unsigned int shaderId) override;
	void SetShaderData(unsigned int shaderId, const char* data) override;
	void DeleteShader(unsigned int shaderId) override;
	void DeleteShaderProgram(unsigned int programId) override;
	void LinkShaderProgram(unsigned int programId) override;

	void UseShaderProgram(unsigned int programId) override;
	unsigned int GetShaderUniformLocation(unsigned int programId, const char* name) override;
	void AttachShader(unsigned int programId, unsigned int shaderId) override;

	void SetShaderAttribut(unsigned int programId, const char* attribut, const Vector4& value) override;
	void SetShaderAttribut(unsigned int programId, const char* attribut, const Vector3& value) override;
	void SetShaderAttribut(unsigned int programId, const char* attribut, const Vector2& value) override;
	void SetShaderAttribut(unsigned int programId, const char* attribut, const float value) override;
	void SetShaderAttribut(unsigned int programId, const char* attribut, const int value) override;
	void SetShaderAttribut(unsigned int programId, const char* attribut, const glm::mat4& trans) override;
	void SetShaderAttribut(unsigned int programId, const char* attribut, const glm::mat3& trans) override;
	void Setlights(const std::shared_ptr<Camera>& camera) override;

private:
	void ApplyTextureFilters(const std::shared_ptr<Texture>& texture);
	unsigned int CreateVertexArray();
	unsigned int CreateBuffer();
	void BindVertexArray(unsigned int bufferId);
	void DeleteBuffer(unsigned int bufferId);
	void DeleteVertexArray(unsigned int bufferId);

	int GetBufferTypeEnum(BufferType bufferType);
	// int GetBufferModeEnum(BufferMode bufferMode);
	int GetWrapModeEnum(Texture::WrapMode wrapMode);
	int maxLightCount = 8;
	void DisableAllLight();
	void SetLight(int lightIndex, const Vector3& lightPosition, float intensity, Color color, Light::LightType type, float attenuation) override;

	float fogStart = 0;
	float fogEnd = 10;
	Color fogColor;
	// int GetCullFaceEnum(CullFace face);
	// float GetAnisotropicValueEnum(Texture::AnisotropicLevel level);
	int GetShaderTypeEnum(Shader::ShaderType shaderType);
	// int GetDrawModeEnum(DrawMode drawMode);
};
#endif