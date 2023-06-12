#include "renderer_opengl.h"

#include "../../../xenity.h"
#include "../3d_graphics/mesh_data.h"

// #include <glad/glad.h>
// #define GLFW_INCLUDE_NONE
// #include <GLFW/glfw3.h>
// #include <glm/gtc/type_ptr.hpp>

#ifdef __PSP__
static unsigned int __attribute__((aligned(16))) list[262144];
#include <pspkernel.h>
#define GUGL_IMPLEMENTATION
#include "../../../psp/gu2gl.h"
#endif

#ifdef __vita__
#include <vitaGL.h>
#endif

RendererOpengl::RendererOpengl()
{
}

int RendererOpengl::Init()
{
	int result = 1;
#ifdef __PSP__
	guglInit(list);
#elif __vita__
	result = vglInit(0x100000);
#else
	result = glfwInit();
#endif

#ifdef __vita__
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	// glCullFace(GL_BACK);
#endif

	return result;
}

void RendererOpengl::Stop()
{
#ifdef __PSP__
	guglTerm();
#elif __vita__
	vglEnd();
#else
	glfwTerminate();
#endif
}

void RendererOpengl::NewFrame()
{
#ifdef __PSP__
	guglStartFrame(list, GL_FALSE);
#endif
}

void RendererOpengl::EndFrame()
{
#ifdef __PSP__
	guglSwapBuffers(GL_TRUE, GL_FALSE);
#elif __vita__
	vglSwapBuffers(GL_FALSE);
#endif
}

void RendererOpengl::SetClearColor(Color color)
{
	RGBA rgba = color.GetRGBA();
#ifdef __PSP__
	glClearColor(color.GetUnsignedIntABGR());
#else
	glClearColor(rgba.r, rgba.g, rgba.b, rgba.a);
#endif
}

void RendererOpengl::SetProjection2D(float projectionSize, float nearClippingPlane, float farClippingPlane)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float halfRatio = Window::GetAspectRatio() / 2.0f * 10 * (projectionSize / 5.0f);
	float halfOne = 0.5f * 10 * (projectionSize / 5.0f);
	glOrtho(-halfRatio, halfRatio, -halfOne, halfOne, nearClippingPlane, farClippingPlane);
}

void RendererOpengl::SetProjection3D(float fov, float nearClippingPlane, float farClippingPlane)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
#ifdef __PSP__
	glPerspective(fov, Window::GetAspectRatio(), nearClippingPlane, farClippingPlane);
#elif __vita__
	gluPerspective(fov, Window::GetAspectRatio(), nearClippingPlane, farClippingPlane);
#endif
}

void RendererOpengl::ResetView()
{
#ifdef __PSP__
	glMatrixMode(GL_VIEW);
	glLoadIdentity();
	gluRotateY((0 + 180) / 180.0f * 3.14159f);
#else
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(0 + 180, 0, 1, 0);
#endif
}

void RendererOpengl::SetCameraPosition(Camera *camera)
{
	Transform *trans = camera->GetTransform();
#ifdef __PSP__
	glMatrixMode(GL_VIEW);
	glLoadIdentity();

	gluRotateZ((-trans->GetRotation().z) / 180.0f * 3.14159f);
	gluRotateX(trans->GetRotation().x / 180.0f * 3.14159f);
	gluRotateY((trans->GetRotation().y + 180) / 180.0f * 3.14159f);

	glTranslatef(trans->GetPosition().x, -trans->GetPosition().y, -trans->GetPosition().z);
#else
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(-trans->GetRotation().z, 0, 0, 1);
	glRotatef(trans->GetRotation().x, 1, 0, 0);
	glRotatef(trans->GetRotation().y + 180, 0, 1, 0);
	glTranslatef(trans->GetPosition().x, -trans->GetPosition().y, -trans->GetPosition().z);
#endif
}

void RendererOpengl::ResetTransform()
{
}

void RendererOpengl::SetTransform(Vector3 position, Vector3 rotation, Vector3 scale, bool resetTransform)
{
#ifdef __PSP__
	glMatrixMode(GL_MODEL);
	if (resetTransform)
		glLoadIdentity();
#else
	glMatrixMode(GL_MODELVIEW);
#endif
	glTranslatef(-position.x, position.y, position.z);

#ifdef __PSP__
	gluRotateY(-rotation.y * 3.14159265359 / 180.0f);
	gluRotateX(rotation.x * 3.14159265359 / 180.0f);
	gluRotateZ(-rotation.z * 3.14159265359 / 180.0f);

#elif __vita__
	glRotatef(-rotation.y, 0, 1, 0);
	glRotatef(rotation.x, 1, 0, 0);
	glRotatef(-rotation.z, 0, 0, 1);
#endif

	glScalef(scale.x, scale.y, scale.z);
}

void RendererOpengl::MoveTransform(Vector3 position)
{
#ifdef __PSP__
	glMatrixMode(GL_MODEL);
#else
	glMatrixMode(GL_MODELVIEW);
#endif
	glTranslatef(-position.x, position.y, position.z);
}

void RendererOpengl::BindTexture(Texture *texture)
{
#ifdef __PSP__
	glTexMode(texture->type, texture->mipmaplevelCount, 0, 1);
	glTexFunc(GL_TFX_MODULATE, GL_TCC_RGBA);
	if (texture->useMipMap)
		sceGuTexLevelMode(GU_TEXTURE_AUTO, 0); // Greater is lower quality

	glTexImage(0, texture->pW, texture->pH, texture->pW, texture->data[0]);
	if (texture->useMipMap)
	{
		glTexImage(1, texture->pW / 2, texture->pH / 2, texture->pW / 2, texture->data[1]);
		// glTexImage(2, texture->pW / 4, texture->pH / 4, texture->pW / 4, texture->data[2]);
		// glTexImage(3, texture->pW / 8, texture->pH / 8, texture->pW / 8, texture->data[3]);
	}

#endif
#ifdef __vita__
	glBindTexture(GL_TEXTURE_2D, texture->GetTextureId());
#endif
	ApplyTextureFilters(texture);
}

void RendererOpengl::ApplyTextureFilters(Texture *texture)
{
	int minFilterValue = 0;
	int magfilterValue = 0;
	if (texture->GetFilter() == Texture::Bilinear)
	{
		if (texture->GetUseMipmap())
		{
			minFilterValue = GL_LINEAR_MIPMAP_LINEAR;
			// minFilterValue = GL_LINEAR;
		}
		else
		{
			// minFilterValue = GL_LINEAR;
			minFilterValue = GL_LINEAR_MIPMAP_LINEAR;
		}
		magfilterValue = GL_LINEAR;
	}
	else if (texture->GetFilter() == Texture::Point)
	{
		if (texture->GetUseMipmap())
		{
			minFilterValue = GL_NEAREST_MIPMAP_NEAREST;
			// minFilterValue = GL_NEAREST;
		}
		else
		{
			// minFilterValue = GL_NEAREST;
			minFilterValue = GL_NEAREST_MIPMAP_NEAREST;
		}
		magfilterValue = GL_NEAREST;
	}
	int wrap = GetWrapModeEnum(texture->GetWrapMode());

#ifdef __PSP__
	glTexFilter(minFilterValue, magfilterValue);
	glTexWrap(wrap, wrap);
#else
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilterValue);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magfilterValue);
#endif
}

void RendererOpengl::DrawMeshData(MeshData *meshData, RenderingSettings settings)
{
	if (settings.invertFaces)
	{
		glFrontFace(GL_CW);
	}
	else
	{
		glFrontFace(GL_CCW);
	}

	if (settings.useDepth)
	{
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}

	if (settings.useBlend)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else
	{
		glDisable(GL_BLEND);
	}

	glEnable(GL_TEXTURE_2D);

#ifdef __PSP__
	int params = 0;

	if (meshData->hasIndices)
	{
		params |= GL_INDEX_16BIT;
	}
	params |= GL_TEXTURE_32BITF;
	if (meshData->hasColor)
	{
		params |= GL_COLOR_8888;
	}
	else
	{
		sceGuColor(meshData->unifiedColor.GetUnsignedIntABGR());
	}
	params |= GL_VERTEX_32BITF;
	params |= GL_TRANSFORM_3D;

	if (!meshData->hasIndices)
	{
		glDrawElements(GL_TRIANGLES, params, meshData->vertice_count, 0, meshData->data);
	}
	else
	{
		glDrawElements(GL_TRIANGLES, params, meshData->index_count, meshData->indices, meshData->data);
	}

#endif

#ifdef __vita__
	glEnableClientState(GL_VERTEX_ARRAY);
	if (meshData->hasColor)
	{
		glEnableClientState(GL_COLOR_ARRAY);
	}
	else
	{
		RGBA rgba = meshData->unifiedColor.GetRGBA();
		glColor4f(rgba.r, rgba.g, rgba.b, rgba.a);
	}
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	if (meshData->hasColor)
	{
		int stride = sizeof(Vertex);
		glTexCoordPointer(2, GL_FLOAT, stride, &((Vertex *)meshData->data)[0].u);
		glColorPointer(3, GL_FLOAT, stride, &((Vertex *)meshData->data)[0].r);
		glVertexPointer(3, GL_FLOAT, stride, &((Vertex *)meshData->data)[0].x);
	}
	else
	{
		int stride = sizeof(VertexNoColor);
		glTexCoordPointer(2, GL_FLOAT, stride, &((VertexNoColor *)meshData->data)[0].u);
		glVertexPointer(3, GL_FLOAT, stride, &((VertexNoColor *)meshData->data)[0].x);
	}

	if (!meshData->hasIndices)
	{
		glDrawArrays(GL_TRIANGLES, 0, meshData->vertice_count);
	}
	else
	{
		glDrawElements(GL_TRIANGLES, meshData->index_count, GL_UNSIGNED_SHORT, meshData->indices);
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	if (meshData->hasColor)
	{
		glDisableClientState(GL_COLOR_ARRAY);
	}
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

#endif
	Performance::AddDrawCall();
}

void RendererOpengl::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT /*| GL_STENCIL_BUFFER_BIT*/);
	// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

// int RendererOpengl::GetBufferTypeEnum(BufferType bufferType)
// {
// 	int type = GL_REPEAT;
// 	switch (bufferType)
// 	{
// 	case Array_Buffer:
// 		type = GL_ARRAY_BUFFER;
// 		break;
// 	case Element_Array_Buffer:
// 		type = GL_ELEMENT_ARRAY_BUFFER;
// 		break;
// 	}
// 	return type;
// }

// int RendererOpengl::GetBufferModeEnum(BufferMode bufferMode)
// {
// 	int mode = GL_STATIC_DRAW;
// 	switch (bufferMode)
// 	{
// 	case Static:
// 		mode = GL_STATIC_DRAW;
// 		break;
// 	case Dynamic:
// 		mode = GL_DYNAMIC_DRAW;
// 		break;
// 	}
// 	return mode;
// }

int RendererOpengl::GetWrapModeEnum(Texture::WrapMode wrapMode)
{
	int mode = GL_REPEAT;
	switch (wrapMode)
	{
	case Texture::WrapMode::ClampToEdge:
	case Texture::WrapMode::ClampToBorder:
		mode = GL_CLAMP;
		break;
	case Texture::WrapMode::Repeat:
		mode = GL_REPEAT;
		break;

		// case Texture::WrapMode::ClampToEdge:
		// 	mode = GL_CLAMP_TO_EDGE;
		// 	break;
		// case Texture::WrapMode::ClampToBorder:
		// 	mode = GL_CLAMP_TO_BORDER;
		// 	break;
		// case Texture::WrapMode::MirroredRepeat:
		// 	mode = GL_MIRRORED_REPEAT;
		// 	break;
		// case Texture::WrapMode::Repeat:
		// 	mode = GL_REPEAT;
		// 	break;
		// case Texture::WrapMode::MirrorClampToEdge:
		// 	mode = GL_MIRROR_CLAMP_TO_EDGE;
		// 	break;
	}
	return mode;
}

// int RendererOpengl::GetCullFaceEnum(CullFace face)
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

// float RendererOpengl::GetAnisotropicValueEnum(Texture::AnisotropicLevel level)
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

// int RendererOpengl::GetShaderTypeEnum(Shader::ShaderType shaderType)
// {
// 	int compileType = GL_VERTEX_SHADER;
// 	switch (shaderType)
// 	{
// 	case Shader::Vertex_Shader:
// 		compileType = GL_VERTEX_SHADER;
// 		break;
// 	case Shader::Fragment_Shader:
// 		compileType = GL_FRAGMENT_SHADER;
// 		break;
// 	case Shader::Tessellation_Control_Shader:
// 		compileType = GL_TESS_CONTROL_SHADER;
// 		break;
// 	case Shader::Tessellation_Evaluation_Shader:
// 		compileType = GL_TESS_EVALUATION_SHADER;
// 		break;
// 	}
// 	return compileType;
// }

// int RendererOpengl::GetDrawModeEnum(DrawMode drawMode)
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