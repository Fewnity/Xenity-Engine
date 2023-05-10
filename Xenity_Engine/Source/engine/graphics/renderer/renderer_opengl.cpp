#include "renderer_opengl.h"

#include "../../../xenity.h"

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

RendererOpengl::RendererOpengl()
{
}

int RendererOpengl::Init()
{
	return glfwInit();
}

void RendererOpengl::Stop()
{
	glfwTerminate();
}

void RendererOpengl::SetClearColor(Color color)
{
	RGBA vec4Color = color.GetRGBA();
	glClearColor(vec4Color.r, vec4Color.g, vec4Color.b, vec4Color.a);
}

void RendererOpengl::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

unsigned int RendererOpengl::GenerateVertexArray()
{
	unsigned int id;
	glGenVertexArrays(1, &id);
	return id;
}

unsigned int RendererOpengl::GenerateBuffer()
{
	unsigned int id;
	glGenBuffers(1, &id);
	return id;
}

void RendererOpengl::BindVertexArray(unsigned int vertexArrayId)
{
	glBindVertexArray(vertexArrayId);
}

void RendererOpengl::BindBuffer(BufferType type, unsigned int bufferId)
{
	int t = GetBufferTypeEnum(type);
	glBindBuffer(t, bufferId); //Set the current GL_ARRAY_BUFFER
}

void RendererOpengl::DeleteBuffer(unsigned int bufferId)
{
	glDeleteBuffers(1, &bufferId);
}

void RendererOpengl::DeleteVertexArray(unsigned int vertexArrayId)
{
	glDeleteVertexArrays(1, &vertexArrayId);
}

void RendererOpengl::SetVertexAttribArray(bool enable, int index)
{
	if(enable)
		glEnableVertexAttribArray(index);
	else
		glDisableVertexAttribArray(index);
}

void RendererOpengl::SetVertexAttribPointer(int index, int length, int totalByteCount, int byteOffset)
{
	glVertexAttribPointer(index, length, GL_FLOAT, GL_FALSE, totalByteCount * sizeof(float), (void*)(byteOffset * sizeof(float)));
}

void RendererOpengl::SetBufferData(BufferType type, BufferMode mode, MeshData* meshData)
{
	int t = GetBufferTypeEnum(type);
	int m = GetBufferModeEnum(mode);
	glBufferData(t, meshData->verticesCount * sizeof(*meshData->vertices), meshData->vertices, m);//Put vertice in the array buffer
}

void RendererOpengl::SetBufferSize(BufferType type, BufferMode mode, int size)
{
	int t = GetBufferTypeEnum(type);
	int m = GetBufferModeEnum(mode);

	glBufferData(t, sizeof(float) * size, NULL, m);
}

void RendererOpengl::SetBufferSubData(BufferType type, MeshData* meshData)
{
	int t = GetBufferTypeEnum(type);
	glBufferSubData(t, 0, sizeof(*meshData->vertices), meshData->vertices);//Put vertice in the array buffer
}

void  RendererOpengl::SetBufferSubData(BufferType type, float* vertices, int size)
{
	int t = GetBufferTypeEnum(type);
	glBufferSubData(t, 0, size, vertices);
}

void RendererOpengl::DrawArray(DrawMode drawMode, int startIndex, int indiceCount)
{
	int t = GetDrawModeEnum(drawMode);
	glDrawArrays(t, startIndex, indiceCount); // For tessellation
}

void RendererOpengl::SetPolygoneMode(bool front, PolygoneFillMode fillMode)
{
	int side = GL_FRONT;
	if (!front)
		side = GL_BACK;

	int fill = GL_FILL;
	if (fillMode == Line)
		fill = GL_LINE;

	glPolygonMode(side, fill);
}

void RendererOpengl::SetLineWidth(float width)
{
	glLineWidth(width);
}

void RendererOpengl::SetViewport(int x, int y, int width, int height)
{
	glViewport(x, y, width, height);
}


void RendererOpengl::SetTextureBorderColor(Color color)
{
	RGBA vec4Color = color.GetRGBA();
	float borderColor[4] = { vec4Color.r, vec4Color.g, vec4Color.b, vec4Color.a };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
}

void RendererOpengl::SetTextureWrapMode(Texture::WrapMode wrapMode)
{
	int mode = GetWrapModeEnum(wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode);
}

void RendererOpengl::SetDepthTest(bool enable)
{
	if (enable)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
}

void RendererOpengl::SetBlend(bool enable)
{
	if (enable)
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);
}

void RendererOpengl::SetCull(bool enable)
{
	if (enable)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
}

void RendererOpengl::SetCullFace(CullFace face)
{
	int f = GetCullFaceEnum(face);
	glCullFace(f);
}

unsigned int RendererOpengl::GenerateTextureId()
{
	unsigned int id;
	glGenTextures(1, &id);;
	return id;
}

void RendererOpengl::DeleteTexture(const Texture* texture)
{
	unsigned int id = texture->GetTextureId();
	glDeleteTextures(1, &id);
}

void RendererOpengl::BindTexture(const Texture* texture)
{
	if (texture)
		glBindTexture(GL_TEXTURE_2D, texture->GetTextureId());
	else
		glBindTexture(GL_TEXTURE_2D, 0);
}

void RendererOpengl::SetTextureData(const Texture* texture, unsigned char* data)
{
	if (texture->GetChannelCount() == 4)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->GetWidth(), texture->GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	else if (texture->GetChannelCount() == 1)
		glTexImage2D(GL_TEXTURE_2D, 0, 1, texture->GetWidth(), texture->GetHeight(), 0, GL_RED, GL_UNSIGNED_BYTE, data);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture->GetWidth(), texture->GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, data);
}

void RendererOpengl::GenerateMipmap()
{
	glGenerateMipmap(GL_TEXTURE_2D);
}

void RendererOpengl::SetTextureFilter(const Texture* texture, Texture::Filter filter)
{
	int minFilterValue = 0;
	int magfilterValue = 0;
	if (filter == Texture::Bilinear)
	{
		if (texture->GetUseMipmap())
		{
			minFilterValue = GL_LINEAR_MIPMAP_LINEAR;
		}
		else
		{
			minFilterValue = GL_LINEAR;
		}
		magfilterValue = GL_LINEAR;
	}
	else if (filter == Texture::Point)
	{
		if (texture->GetUseMipmap())
		{
			minFilterValue = GL_NEAREST_MIPMAP_NEAREST;
		}
		else
		{
			minFilterValue = GL_NEAREST;
		}
		magfilterValue = GL_NEAREST;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilterValue);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magfilterValue);
}

void RendererOpengl::SetAnisotropyLevel(Texture::AnisotropicLevel level)
{
	float anisotropicValue = GetAnisotropicValueEnum(level);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, anisotropicValue);
}

void RendererOpengl::EnableTextureUnit(int unit)
{
	glActiveTexture(GL_TEXTURE0 + unit);
}

void  RendererOpengl::PixelStoreUnpack() 
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

unsigned int RendererOpengl::CreateShader(Shader::ShaderType type)
{
	int compileType = GetShaderTypeEnum(type);
	return glCreateShader(compileType);
}

unsigned int RendererOpengl::CreateShaderProgram()
{
	return glCreateProgram();
}

void RendererOpengl::CompileShader(unsigned int shaderId)
{
	glCompileShader(shaderId);
}

int RendererOpengl::GetShaderCompilationResult(unsigned int shaderId)
{
	GLint vResult;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &vResult);
	return (int)vResult;
}

std::vector<char> RendererOpengl::GetCompilationError(unsigned int shaderId)
{
	int maxLength = 256;
	std::vector<char> errorLog(maxLength);
	glGetShaderInfoLog(shaderId, maxLength, &maxLength, &errorLog[0]);
	return errorLog;
}

void RendererOpengl::SetShaderData(unsigned int shaderId, const char* data)
{
	glShaderSource(shaderId, 1, &data, NULL);
}

void RendererOpengl::DeleteShader(unsigned int shaderId)
{
	glDeleteShader(shaderId);
}

void RendererOpengl::DeleteShaderProgram(unsigned int programId)
{
	glDeleteProgram(programId);
}

void RendererOpengl::LinkShaderProgram(unsigned int programId)
{
	glLinkProgram(programId);
}

void RendererOpengl::UseShaderProgram(unsigned int programId)
{
	glUseProgram(programId);
}

unsigned int RendererOpengl::GetShaderUniformLocation(unsigned int programId, const char* name)
{
	return glGetUniformLocation(programId, name);
}

void RendererOpengl::AttachShader(unsigned int programId, unsigned int shaderId)
{
	glAttachShader(programId, shaderId);
}

void RendererOpengl::SetShaderAttribut(unsigned int programId, const char* attribut, const Vector4& value)
{
	glUniform4f(GetShaderUniformLocation(programId, attribut), value.x, value.y, value.z, value.w);
}

void RendererOpengl::SetShaderAttribut(unsigned int programId, const char* attribut, const Vector3& value)
{
	glUniform3f(GetShaderUniformLocation(programId, attribut), value.x, value.y, value.z);
}

void RendererOpengl::SetShaderAttribut(unsigned int programId, const char* attribut, const Vector2& value)
{
	glUniform2f(GetShaderUniformLocation(programId, attribut), value.x, value.y);
}

void RendererOpengl::SetShaderAttribut(unsigned int programId, const char* attribut, const float value)
{
	glUniform1f(GetShaderUniformLocation(programId, attribut), value);
}

void RendererOpengl::SetShaderAttribut(unsigned int programId, const char* attribut, const int value)
{
	glUniform1i(GetShaderUniformLocation(programId, attribut), value);
}

void RendererOpengl::SetShaderAttribut(unsigned int programId, const char* attribut, const glm::mat4& trans)
{
	glUniformMatrix4fv(GetShaderUniformLocation(programId, attribut), 1, false, glm::value_ptr(trans));
}

void RendererOpengl::SetShaderAttribut(unsigned int programId, const char* attribut, const glm::mat3& trans)
{
	glUniformMatrix3fv(GetShaderUniformLocation(programId, attribut), 1, false, glm::value_ptr(trans));
}

int RendererOpengl::GetBufferTypeEnum(BufferType bufferType)
{
	int type = GL_REPEAT;
	switch (bufferType)
	{
	case Array_Buffer:
		type = GL_ARRAY_BUFFER;
		break;
	case Element_Array_Buffer:
		type = GL_ELEMENT_ARRAY_BUFFER;
		break;
	}
	return type;
}

int RendererOpengl::GetBufferModeEnum(BufferMode bufferMode)
{
	int mode = GL_STATIC_DRAW;
	switch (bufferMode)
	{
	case Static:
		mode = GL_STATIC_DRAW;
		break;
	case Dynamic:
		mode = GL_DYNAMIC_DRAW;
		break;
	}
	return mode;
}

int RendererOpengl::GetWrapModeEnum(Texture::WrapMode wrapMode)
{
	int mode = GL_REPEAT;
	switch (wrapMode)
	{
	case Texture::WrapMode::ClampToEdge:
		mode = GL_CLAMP_TO_EDGE;
		break;
	case Texture::WrapMode::ClampToBorder:
		mode = GL_CLAMP_TO_BORDER;
		break;
	case Texture::WrapMode::MirroredRepeat:
		mode = GL_MIRRORED_REPEAT;
		break;
	case Texture::WrapMode::Repeat:
		mode = GL_REPEAT;
		break;
	case Texture::WrapMode::MirrorClampToEdge:
		mode = GL_MIRROR_CLAMP_TO_EDGE;
		break;
	}
	return mode;
}

int RendererOpengl::GetCullFaceEnum(CullFace face)
{
	int side = GL_BACK;
	switch (face)
	{
	case Front:
		side = GL_FRONT;
		break;
	case Back:
		side = GL_BACK;
		break;
	case Front_And_Back:
		side = GL_FRONT_AND_BACK;
		break;
	}
	return side;
}

float RendererOpengl::GetAnisotropicValueEnum(Texture::AnisotropicLevel level)
{
	float anisotropicValue = 16;
	switch (level)
	{
	case Texture::X0:
		anisotropicValue = 1;
		break;
	case Texture::X2:
		anisotropicValue = 2;
		break;
	case Texture::X4:
		anisotropicValue = 4;
		break;
	case Texture::X8:
		anisotropicValue = 8;
		break;
	case Texture::X16:
		anisotropicValue = 16;
		break;
	}
	return anisotropicValue;
}

int RendererOpengl::GetShaderTypeEnum(Shader::ShaderType shaderType)
{
	int compileType = GL_VERTEX_SHADER;
	switch (shaderType)
	{
	case Shader::Vertex_Shader:
		compileType = GL_VERTEX_SHADER;
		break;
	case Shader::Fragment_Shader:
		compileType = GL_FRAGMENT_SHADER;
		break;
	case Shader::Tessellation_Control_Shader:
		compileType = GL_TESS_CONTROL_SHADER;
		break;
	case Shader::Tessellation_Evaluation_Shader:
		compileType = GL_TESS_EVALUATION_SHADER;
		break;
	}
	return compileType;
}

int RendererOpengl::GetDrawModeEnum(DrawMode drawMode)
{
	int mode = GL_TRIANGLES;
	switch (drawMode)
	{
	case Patches:
		mode = GL_PATCHES;
		break;
	case Triangles:
		mode = GL_TRIANGLES;
		break;
	case Quads:
		mode = GL_QUADS;
		break;
	}

	return mode;
}