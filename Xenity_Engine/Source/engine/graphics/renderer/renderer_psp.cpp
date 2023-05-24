#include "renderer_psp.h"

#include "../../../xenity.h"

//#include <glad/glad.h>
//#define GLFW_INCLUDE_NONE
//#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

RendererPSP::RendererPSP()
{
}

int RendererPSP::Init()
{
	return 0;
	//return glfwInit();
}

void RendererPSP::Stop()
{
	//glfwTerminate();
}

void RendererPSP::SetClearColor(Color color)
{
	RGBA vec4Color = color.GetRGBA();
	//glClearColor(vec4Color.r, vec4Color.g, vec4Color.b, vec4Color.a);
}

void RendererPSP::Clear()
{
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

unsigned int RendererPSP::GenerateVertexArray()
{
	unsigned int id;
	//glGenVertexArrays(1, &id);
	//return id;
	return 0;
}

unsigned int RendererPSP::GenerateBuffer()
{
	unsigned int id;
	//glGenBuffers(1, &id);
	//return id;
	return 0;
}

void RendererPSP::BindVertexArray(unsigned int vertexArrayId)
{
	//glBindVertexArray(vertexArrayId);
}

void RendererPSP::BindBuffer(BufferType type, unsigned int bufferId)
{
	int t = GetBufferTypeEnum(type);
	//glBindBuffer(t, bufferId); //Set the current GL_ARRAY_BUFFER
}

void RendererPSP::DeleteBuffer(unsigned int bufferId)
{
	//glDeleteBuffers(1, &bufferId);
}

void RendererPSP::DeleteVertexArray(unsigned int vertexArrayId)
{
	//glDeleteVertexArrays(1, &vertexArrayId);
}

void RendererPSP::SetVertexAttribArray(bool enable, int index)
{
	/*if (enable)
		glEnableVertexAttribArray(index);
	else
		glDisableVertexAttribArray(index);*/
}

void RendererPSP::SetVertexAttribPointer(int index, int length, int totalByteCount, int byteOffset)
{
	//glVertexAttribPointer(index, length, GL_FLOAT, GL_FALSE, totalByteCount * sizeof(float), (void*)(byteOffset * sizeof(float)));
}

void RendererPSP::SetBufferData(BufferType type, BufferMode mode, MeshData* meshData)
{
	int t = GetBufferTypeEnum(type);
	int m = GetBufferModeEnum(mode);
	//glBufferData(t, meshData->verticesCount * sizeof(*meshData->vertices), meshData->vertices, m);//Put vertice in the array buffer
}

void RendererPSP::SetBufferSize(BufferType type, BufferMode mode, int size)
{
	int t = GetBufferTypeEnum(type);
	int m = GetBufferModeEnum(mode);

	//glBufferData(t, sizeof(float) * size, NULL, m);
}

void RendererPSP::SetBufferSubData(BufferType type, MeshData* meshData)
{
	int t = GetBufferTypeEnum(type);
	//glBufferSubData(t, 0, sizeof(*meshData->vertices), meshData->vertices);//Put vertice in the array buffer
}

void  RendererPSP::SetBufferSubData(BufferType type, float* vertices, int size)
{
	int t = GetBufferTypeEnum(type);
	//glBufferSubData(t, 0, size, vertices);
}

void RendererPSP::DrawArray(DrawMode drawMode, int startIndex, int indiceCount)
{
	int t = GetDrawModeEnum(drawMode);
	//glDrawArrays(t, startIndex, indiceCount); // For tessellation
}

void RendererPSP::SetPolygoneMode(bool front, PolygoneFillMode fillMode)
{
	/*int side = GL_FRONT;
	if (!front)
		side = GL_BACK;

	int fill = GL_FILL;
	if (fillMode == Line)
		fill = GL_LINE;*/

	//glPolygonMode(side, fill);
}

void RendererPSP::SetLineWidth(float width)
{
	//glLineWidth(width);
}

void RendererPSP::SetViewport(int x, int y, int width, int height)
{
	//glViewport(x, y, width, height);
}


void RendererPSP::SetTextureBorderColor(Color color)
{
	RGBA vec4Color = color.GetRGBA();
	float borderColor[4] = { vec4Color.r, vec4Color.g, vec4Color.b, vec4Color.a };
	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
}

void RendererPSP::SetTextureWrapMode(Texture::WrapMode wrapMode)
{
	int mode = GetWrapModeEnum(wrapMode);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode);
}

void RendererPSP::SetDepthTest(bool enable)
{
	/*if (enable)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);*/
}

void RendererPSP::SetBlend(bool enable)
{
	/*if (enable)
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);*/
}

void RendererPSP::SetCull(bool enable)
{
	/*if (enable)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);*/
}

void RendererPSP::SetCullFace(CullFace face)
{
	int f = GetCullFaceEnum(face);
	//glCullFace(f);
}

unsigned int RendererPSP::GenerateTextureId()
{
	unsigned int id;
	//glGenTextures(1, &id);;
	//return id;
	return 0;
}

void RendererPSP::DeleteTexture(const Texture* texture)
{
	unsigned int id = texture->GetTextureId();
	//glDeleteTextures(1, &id);
}

void RendererPSP::BindTexture(const Texture* texture)
{
	/*if (texture)
		glBindTexture(GL_TEXTURE_2D, texture->GetTextureId());
	else
		glBindTexture(GL_TEXTURE_2D, 0);*/
}

void RendererPSP::SetTextureData(const Texture* texture, unsigned char* data)
{
	/*if (texture->GetChannelCount() == 4)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->GetWidth(), texture->GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	else if (texture->GetChannelCount() == 1)
		glTexImage2D(GL_TEXTURE_2D, 0, 1, texture->GetWidth(), texture->GetHeight(), 0, GL_RED, GL_UNSIGNED_BYTE, data);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture->GetWidth(), texture->GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, data);*/
}

void RendererPSP::GenerateMipmap()
{
	//glGenerateMipmap(GL_TEXTURE_2D);
}

void RendererPSP::SetTextureFilter(const Texture* texture, Texture::Filter filter)
{
	/*int minFilterValue = 0;
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
	}*/

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilterValue);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magfilterValue);
}

void RendererPSP::SetAnisotropyLevel(Texture::AnisotropicLevel level)
{
	float anisotropicValue = GetAnisotropicValueEnum(level);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, anisotropicValue);
}

void RendererPSP::EnableTextureUnit(int unit)
{
	//glActiveTexture(GL_TEXTURE0 + unit);
}

void  RendererPSP::PixelStoreUnpack() 
{
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

unsigned int RendererPSP::CreateShader(Shader::ShaderType type)
{
	//int compileType = GetShaderTypeEnum(type);
	//return glCreateShader(compileType);
	return 0;
}

unsigned int RendererPSP::CreateShaderProgram()
{
	return 0;
	//return glCreateProgram();
}

void RendererPSP::CompileShader(unsigned int shaderId)
{
	//glCompileShader(shaderId);
}

int RendererPSP::GetShaderCompilationResult(unsigned int shaderId)
{
	//GLint vResult;
	//glGetShaderiv(shaderId, GL_COMPILE_STATUS, &vResult);
	//return (int)vResult;
	return 0;
}

std::vector<char> RendererPSP::GetCompilationError(unsigned int shaderId)
{
	int maxLength = 256;
	std::vector<char> errorLog(maxLength);
	//glGetShaderInfoLog(shaderId, maxLength, &maxLength, &errorLog[0]);
	return errorLog;
}

void RendererPSP::SetShaderData(unsigned int shaderId, const char* data)
{
	//glShaderSource(shaderId, 1, &data, NULL);
}

void RendererPSP::DeleteShader(unsigned int shaderId)
{
	//glDeleteShader(shaderId);
}

void RendererPSP::DeleteShaderProgram(unsigned int programId)
{
	//glDeleteProgram(programId);
}

void RendererPSP::LinkShaderProgram(unsigned int programId)
{
	//glLinkProgram(programId);
}

void RendererPSP::UseShaderProgram(unsigned int programId)
{
	//glUseProgram(programId);
}

unsigned int RendererPSP::GetShaderUniformLocation(unsigned int programId, const char* name)
{
	return 0;
	//return glGetUniformLocation(programId, name);
}

void RendererPSP::AttachShader(unsigned int programId, unsigned int shaderId)
{
	//glAttachShader(programId, shaderId);
}

void RendererPSP::SetShaderAttribut(unsigned int programId, const char* attribut, const Vector4& value)
{
	//glUniform4f(GetShaderUniformLocation(programId, attribut), value.x, value.y, value.z, value.w);
}

void RendererPSP::SetShaderAttribut(unsigned int programId, const char* attribut, const Vector3& value)
{
	//glUniform3f(GetShaderUniformLocation(programId, attribut), value.x, value.y, value.z);
}

void RendererPSP::SetShaderAttribut(unsigned int programId, const char* attribut, const Vector2& value)
{
	//glUniform2f(GetShaderUniformLocation(programId, attribut), value.x, value.y);
}

void RendererPSP::SetShaderAttribut(unsigned int programId, const char* attribut, const float value)
{
	//glUniform1f(GetShaderUniformLocation(programId, attribut), value);
}

void RendererPSP::SetShaderAttribut(unsigned int programId, const char* attribut, const int value)
{
	//glUniform1i(GetShaderUniformLocation(programId, attribut), value);
}

void RendererPSP::SetShaderAttribut(unsigned int programId, const char* attribut, const glm::mat4& trans)
{
	//glUniformMatrix4fv(GetShaderUniformLocation(programId, attribut), 1, false, glm::value_ptr(trans));
}

void RendererPSP::SetShaderAttribut(unsigned int programId, const char* attribut, const glm::mat3& trans)
{
	//glUniformMatrix3fv(GetShaderUniformLocation(programId, attribut), 1, false, glm::value_ptr(trans));
}

int RendererPSP::GetBufferTypeEnum(BufferType bufferType)
{
	/*int type = GL_REPEAT;
	switch (bufferType)
	{
	case Array_Buffer:
		type = GL_ARRAY_BUFFER;
		break;
	case Element_Array_Buffer:
		type = GL_ELEMENT_ARRAY_BUFFER;
		break;
	}
	return type;*/
	return 0;
}

int RendererPSP::GetBufferModeEnum(BufferMode bufferMode)
{
	/*int mode = GL_STATIC_DRAW;
	switch (bufferMode)
	{
	case Static:
		mode = GL_STATIC_DRAW;
		break;
	case Dynamic:
		mode = GL_DYNAMIC_DRAW;
		break;
	}
	return mode;*/
	return 0;
}

int RendererPSP::GetWrapModeEnum(Texture::WrapMode wrapMode)
{
	/*int mode = GL_REPEAT;
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
	return mode;*/
	return 0;
}

int RendererPSP::GetCullFaceEnum(CullFace face)
{
	/*int side = GL_BACK;
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
	return side;*/
	return 0;
}

float RendererPSP::GetAnisotropicValueEnum(Texture::AnisotropicLevel level)
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

int RendererPSP::GetShaderTypeEnum(Shader::ShaderType shaderType)
{
	/*int compileType = GL_VERTEX_SHADER;
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
	}*/
	return 0;
}

int RendererPSP::GetDrawModeEnum(DrawMode drawMode)
{
	/*int mode = GL_TRIANGLES;
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

	return mode;*/
	return 0;
}