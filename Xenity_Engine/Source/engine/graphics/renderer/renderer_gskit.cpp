#include "renderer_gskit.h"

#include "../../../xenity.h"
#include "../3d_graphics/mesh_data.h"
#include "../../tools/profiler_benchmark.h"

#include <memory>
#include <glm/gtc/type_ptr.hpp>

RendererGsKit::RendererGsKit()
{
}

int RendererGsKit::Init()
{

	int result = 0;

	return result;
}

void RendererGsKit::Setup()
{
}

void RendererGsKit::Stop()
{
}

void RendererGsKit::NewFrame()
{
}

void RendererGsKit::EndFrame()
{
}

void RendererGsKit::SetViewport(int x, int y, int width, int height)
{
}

void RendererGsKit::SetClearColor(const Color &color)
{
}

void RendererGsKit::SetProjection2D(float projectionSize, float nearClippingPlane, float farClippingPlane)
{
}

void RendererGsKit::SetProjection3D(float fov, float nearClippingPlane, float farClippingPlane, float aspect)
{
}

void RendererGsKit::ResetView()
{
}

void RendererGsKit::SetCameraPosition(const std::shared_ptr<Camera> &camera)
{
}

void RendererGsKit::ResetTransform()
{
}

void RendererGsKit::SetTransform(const Vector3 &position, const Vector3 &rotation, const Vector3 &scale, bool resetTransform)
{
}

void RendererGsKit::SetTransform(const glm::mat4 &mat)
{
}

void RendererGsKit::BindTexture(const std::shared_ptr<Texture> &texture)
{
}

void RendererGsKit::ApplyTextureFilters(const std::shared_ptr<Texture> &texture)
{
}

void RendererGsKit::DrawMeshData(const std::shared_ptr<MeshData> &meshData, const std::vector<std::shared_ptr<Texture>> &textures, RenderingSettings &settings)
{
}

void RendererGsKit::DrawLine(const Vector3 &a, const Vector3 &b, const Color &color, RenderingSettings &settings)
{
}

unsigned int RendererGsKit::CreateNewTexture()
{

	return 0;
}

void RendererGsKit::DeleteTexture(Texture *texture)
{
}

void RendererGsKit::SetTextureData(const std::shared_ptr<Texture> &texture, unsigned int textureType, const unsigned char *buffer)
{
}

void RendererGsKit::SetLight(int lightIndex, const Vector3 &lightPosition, float intensity, Color color, Light::LightType type, float attenuation)
{
}

void RendererGsKit::DisableAllLight()
{
}

void RendererGsKit::Setlights(const std::shared_ptr<Camera> &camera)
{
}

void RendererGsKit::Clear()
{
}

void RendererGsKit::SetFog(bool active)
{
}

void RendererGsKit::SetFogValues(float start, float end, const Color &color)
{
}

unsigned int RendererGsKit::CreateBuffer()
{
	return 0;
}

unsigned int RendererGsKit::CreateVertexArray()
{
	return 0;
}

void RendererGsKit::BindVertexArray(unsigned int bufferId)
{
}

void RendererGsKit::DeleteBuffer(unsigned int bufferId)
{
}

void RendererGsKit::DeleteVertexArray(unsigned int bufferId)
{
}

void RendererGsKit::DeleteSubMeshData(MeshData::SubMesh *subMesh)
{
}

void RendererGsKit::UploadMeshData(const std::shared_ptr<MeshData> &meshData)
{
}

unsigned int RendererGsKit::CreateShader(Shader::ShaderType type)
{

	return 0;
}

unsigned int RendererGsKit::CreateShaderProgram()
{

	return 0;
}

void RendererGsKit::CompileShader(unsigned int shaderId)
{
}

int RendererGsKit::GetShaderCompilationResult(unsigned int shaderId)
{

	return 0;
}

std::vector<char> RendererGsKit::GetCompilationError(unsigned int shaderId)
{

	return std::vector<char>();
}

void RendererGsKit::SetShaderData(unsigned int shaderId, const char *data)
{
}

void RendererGsKit::DeleteShader(unsigned int shaderId)
{
}

void RendererGsKit::DeleteShaderProgram(unsigned int programId)
{
}

void RendererGsKit::LinkShaderProgram(unsigned int programId)
{
}

void RendererGsKit::UseShaderProgram(unsigned int programId)
{
}

unsigned int RendererGsKit::GetShaderUniformLocation(unsigned int programId, const char *name)
{

	return 0;
}

void RendererGsKit::AttachShader(unsigned int programId, unsigned int shaderId)
{
}

void RendererGsKit::SetShaderAttribut(unsigned int programId, const char *attribut, const Vector4 &value)
{
}

void RendererGsKit::SetShaderAttribut(unsigned int programId, const char *attribut, const Vector3 &value)
{
}

void RendererGsKit::SetShaderAttribut(unsigned int programId, const char *attribut, const Vector2 &value)
{
}

void RendererGsKit::SetShaderAttribut(unsigned int programId, const char *attribut, const float value)
{
}

void RendererGsKit::SetShaderAttribut(unsigned int programId, const char *attribut, const int value)
{
}

void RendererGsKit::SetShaderAttribut(unsigned int programId, const char *attribut, const glm::mat4 &trans)
{
}

void RendererGsKit::SetShaderAttribut(unsigned int programId, const char *attribut, const glm::mat3 &trans)
{
}

int RendererGsKit::GetBufferTypeEnum(BufferType bufferType)
{

	return 0;
}

int RendererGsKit::GetWrapModeEnum(Texture::WrapMode wrapMode)
{
	return 0;
}

// int RendererGsKit::GetCullFaceEnum(CullFace face)
// {
// 	int side = GL_BACK;
// 	switch (face)
// 	{
// 	case Front:
// 		side = GL_FRONT;
// 		break;
// 	case Back:
// 		side = GL_BACK;
// 		break;
// 	case Front_And_Back:
// 		side = GL_FRONT_AND_BACK;
// 		break;
// 	}
// 	return side;
// }

// float RendererGsKit::GetAnisotropicValueEnum(Texture::AnisotropicLevel level)
// {
// 	float anisotropicValue = 16;
// 	switch (level)
// 	{
// 	case Texture::X0:
// 		anisotropicValue = 1;
// 		break;
// 	case Texture::X2:
// 		anisotropicValue = 2;
// 		break;
// 	case Texture::X4:
// 		anisotropicValue = 4;
// 		break;
// 	case Texture::X8:
// 		anisotropicValue = 8;
// 		break;
// 	case Texture::X16:
// 		anisotropicValue = 16;
// 		break;
// 	}
// 	return anisotropicValue;
// }

int RendererGsKit::GetShaderTypeEnum(Shader::ShaderType shaderType)
{
	return 0;
}

// int RendererGsKit::GetDrawModeEnum(DrawMode drawMode)
// {
// 	int mode = GL_TRIANGLES;
// 	switch (drawMode)
// 	{
// 	case Patches:
// 		mode = GL_PATCHES;
// 		break;
// 	case Triangles:
// 		mode = GL_TRIANGLES;
// 		break;
// 	case Quads:
// 		mode = GL_QUADS;
// 		break;
// 	}

// 	return mode;
// }