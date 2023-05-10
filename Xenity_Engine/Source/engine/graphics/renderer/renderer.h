#pragma once

#include "../texture.h"
#include "../shader.h"
#include <glm/glm.hpp>
#include <vector>

class Color;
class Vector4;
class Vector3;
class Vector2;
class MeshData;

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
	
	virtual void SetClearColor(Color color) = 0;
	virtual void Clear() = 0;

	//Geometry
	virtual unsigned int GenerateVertexArray() = 0;
	virtual unsigned int GenerateBuffer() = 0;
	virtual void BindVertexArray(unsigned int vertexArrayid) = 0;
	virtual void BindBuffer(BufferType type, unsigned int bufferid) = 0;
	virtual void DeleteBuffer(unsigned int bufferId) = 0;
	virtual void DeleteVertexArray(unsigned int arrayId) = 0;

	virtual void SetVertexAttribArray(bool enable, int index) = 0;
	virtual void SetVertexAttribPointer(int index, int length, int totalByteCount, int byteOffset) = 0;
	virtual void SetBufferData(BufferType type, BufferMode mode, MeshData * meshData) = 0;
	virtual void SetBufferSize(BufferType type, BufferMode mode, int size) = 0;
	virtual void SetBufferSubData(BufferType type, MeshData* meshData) = 0;
	virtual void SetBufferSubData(BufferType type, float* vertices, int size) = 0;
	virtual void DrawArray(DrawMode drawMode, int startIndex, int indiceCount) = 0;
	virtual void PixelStoreUnpack() = 0;

	virtual void SetPolygoneMode(bool front, PolygoneFillMode fillMode) = 0;
	virtual void SetLineWidth(float width) = 0;
	virtual void SetViewport(int x, int y, int width, int height) = 0;
	virtual void SetTextureBorderColor(Color color) = 0;
	virtual void SetTextureWrapMode(Texture::WrapMode wrapMode) = 0;
	virtual void SetDepthTest(bool enable) = 0;
	virtual void SetBlend(bool enable) = 0;
	virtual void SetCull(bool enable) = 0;
	virtual void SetCullFace(CullFace face) = 0;

	//Texture
	virtual unsigned int GenerateTextureId() = 0;
	virtual void DeleteTexture(const Texture* texture) = 0;
	virtual void BindTexture(const Texture* texture) = 0;
	virtual void SetTextureData(const Texture* texture, unsigned char* data) = 0;
	virtual void GenerateMipmap() = 0;
	virtual void SetTextureFilter(const Texture* texture, Texture::Filter filter) = 0;
	virtual void SetAnisotropyLevel(Texture::AnisotropicLevel level) = 0;
	virtual void EnableTextureUnit(int unit) = 0;
	
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
};

