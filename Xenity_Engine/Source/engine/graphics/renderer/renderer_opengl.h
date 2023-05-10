#pragma once

#include "renderer.h"

class RendererOpengl : public Renderer
{
public:
	RendererOpengl();
	int Init();
	void Stop();
	void SetClearColor(Color color);
	void Clear();

	//Geometry
	unsigned int GenerateVertexArray();
	unsigned int GenerateBuffer();
	void BindVertexArray(unsigned int vertexArrayId);
	void BindBuffer(BufferType type, unsigned int bufferid);
	void DeleteBuffer(unsigned int bufferId);
	void DeleteVertexArray(unsigned int vertexArrayId);

	void SetVertexAttribArray(bool enable, int index);
	void SetVertexAttribPointer(int index, int length, int totalByteCount, int byteOffset);
	void SetBufferData(BufferType type, BufferMode mode, MeshData* meshData);
	void SetBufferSize(BufferType type, BufferMode mode, int size);
	void SetBufferSubData(BufferType type, MeshData* meshData);
	void SetBufferSubData(BufferType type, float* vertices, int size);
	void DrawArray(DrawMode drawMode, int startIndex, int indiceCount);

	void SetPolygoneMode(bool front, PolygoneFillMode fillMode);
	void SetLineWidth(float width);
	void SetViewport(int x, int y, int width, int height);
	void SetTextureBorderColor(Color color);
	void SetTextureWrapMode(Texture::WrapMode wrapMode);
	void SetDepthTest(bool enable);
	void SetBlend(bool enable);
	void SetCull(bool enable);
	void SetCullFace(CullFace face);

	//Texture
	unsigned int GenerateTextureId();
	void DeleteTexture(const Texture* texture);
	void BindTexture(const Texture* texture);
	void SetTextureData(const Texture* texture, unsigned char* data);
	void GenerateMipmap();
	void SetTextureFilter(const Texture* texture, Texture::Filter filter);
	void SetAnisotropyLevel(Texture::AnisotropicLevel level);
	void EnableTextureUnit(int unit);
	void PixelStoreUnpack();

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
	int GetBufferModeEnum(BufferMode bufferMode);
	int GetWrapModeEnum(Texture::WrapMode wrapMode);
	int GetCullFaceEnum(CullFace face);
	float GetAnisotropicValueEnum(Texture::AnisotropicLevel level);
	int GetShaderTypeEnum(Shader::ShaderType shaderType);
	int GetDrawModeEnum(DrawMode drawMode);
};

