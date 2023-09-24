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

class API RendererOpengl : public Renderer
{
public:
	RendererOpengl();
	int Init();
	void Setup();
	void Stop();
	void NewFrame();
	void EndFrame();
	void SetViewport(int x, int y, int width, int height);
	void SetClearColor(const Color& color);
	void SetProjection2D(float projectionSize, float nearClippingPlane, float farClippingPlane);
	void SetProjection3D(float fov, float nearClippingPlane, float farClippingPlane, float aspect);
	void ResetView();
	void SetCameraPosition(std::weak_ptr<Camera> camera);
	void ResetTransform();
	void SetTransform(const Vector3& position, const Vector3& rotation, const Vector3& scale, bool resetTransform);
	void SetTransform(const glm::mat4 &mat);
	void MoveTransform(const Vector3& position);
	void BindTexture(std::shared_ptr <Texture> texture);
	void ApplyTextureFilters(std::shared_ptr <Texture> texture);
	void DrawMeshData(std::shared_ptr <MeshData> meshData, std::vector<std::shared_ptr<Texture>> textures, RenderingSettings& settings);
	void DrawLine(const Vector3& a, const Vector3& bn, const Color& color, RenderingSettings& settings);
	unsigned int CreateNewTexture();
	void DeleteTexture(Texture* texture);
	void SetTextureData(std::shared_ptr <Texture> texture, unsigned int textureType, const unsigned char *buffer);
	void Clear();
	void SetFog(bool active);
	void SetFogValues(float start, float end, Color color);

	unsigned int CreateBuffer();
	void BindBuffer(BufferType type, unsigned int bufferId);
	void DeleteBuffer(unsigned int bufferId);

	void UploadMeshData(std::shared_ptr<MeshData> meshData);

	//Shader
	unsigned int CreateShader(Shader::ShaderType type);
	unsigned int CreateShaderProgram();
	void CompileShader(unsigned int shaderId);
	int GetShaderCompilationResult(unsigned int shaderId);
	std::vector<char> GetCompilationError(unsigned int shaderId);
	void SetShaderData(unsigned int shaderId, const char* data);
	void DeleteShader(unsigned int shaderId);
	void DeleteShaderProgram(unsigned int programId);
	void LinkShaderProgram(unsigned int programId);

	void UseShaderProgram(unsigned int programId);
	unsigned int GetShaderUniformLocation(unsigned int programId, const char* name);
	void AttachShader(unsigned int programId, unsigned int shaderId);

	void SetShaderAttribut(unsigned int programId, const char* attribut, const Vector4& value);
	void SetShaderAttribut(unsigned int programId, const char* attribut, const Vector3& value);
	void SetShaderAttribut(unsigned int programId, const char* attribut, const Vector2& value);
	void SetShaderAttribut(unsigned int programId, const char* attribut, const float value);
	void SetShaderAttribut(unsigned int programId, const char* attribut, const int value);
	void SetShaderAttribut(unsigned int programId, const char* attribut, const glm::mat4& trans);
	void SetShaderAttribut(unsigned int programId, const char* attribut, const glm::mat3& trans);


private:
	int GetBufferTypeEnum(BufferType bufferType);
	// int GetBufferModeEnum(BufferMode bufferMode);
	int GetWrapModeEnum(Texture::WrapMode wrapMode);
	int maxLightCount = 8;
	void DisableAllLight();
	void Setlights(std::weak_ptr<Camera> camera);
	void SetLight(int lightIndex, Vector3 lightPosition, float intensity, Color color, Light::LightType type, float attenuation);

	float fogStart = 0;
	float fogEnd = 10;
	Color fogColor;
	// int GetCullFaceEnum(CullFace face);
	// float GetAnisotropicValueEnum(Texture::AnisotropicLevel level);
	int GetShaderTypeEnum(Shader::ShaderType shaderType);
	// int GetDrawModeEnum(DrawMode drawMode);
};
