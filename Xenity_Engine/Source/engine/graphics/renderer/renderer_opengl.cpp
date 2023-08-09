#include "renderer_opengl.h"

#include "../../../xenity.h"
#include "../3d_graphics/mesh_data.h"

#if defined(_WIN32) || defined(_WIN64)
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLFW_DLL
#endif

#if defined(__PSP__)
static unsigned int __attribute__((aligned(16))) list[262144];
#include <pspkernel.h>
#define GUGL_IMPLEMENTATION
#include "../../../psp/gu2gl.h"
#endif

#if defined(__vita__)
#include <vitaGL.h>
#endif

#include <memory>

RendererOpengl::RendererOpengl()
{
}

int RendererOpengl::Init()
{
	int result = 1;
#if defined(__PSP__)
	guglInit(list);
	maxLightCount = 4;
#elif defined(__vita__)
	result = vglInit(0x100000);
#elif defined(_WIN32) || defined(_WIN64)
	result = glfwInit();
#endif

	Debug::Print("-------- OpenGL Renderer initiated --------");

	return result;
}

void RendererOpengl::Setup()
{
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
	glEnable(GL_NORMALIZE);
#endif

#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	// glCullFace(GL_BACK);
#endif
}

void RendererOpengl::Stop()
{
#if defined(__PSP__)
	guglTerm();
#elif defined(__vita__)
	vglEnd();
#else
	glfwTerminate();
#endif
}

void RendererOpengl::NewFrame()
{
#if defined(__PSP__)
	guglStartFrame(list, GL_FALSE);
#endif
}

void RendererOpengl::EndFrame()
{
#if defined(__PSP__)
	guglSwapBuffers(GL_FALSE, GL_FALSE);
#elif defined(__vita__)
	vglSwapBuffers(GL_FALSE);
#endif
}

void RendererOpengl::SetViewport(int x, int y, int width, int height)
{
	glViewport(x, y, width, height);
}

void RendererOpengl::SetClearColor(Color color)
{
	RGBA rgba = color.GetRGBA();
#if defined(__PSP__)
	glClearColor(color.GetUnsignedIntABGR());
#else
	glClearColor(rgba.r, rgba.g, rgba.b, rgba.a);
#endif
}

void RendererOpengl::SetProjection2D(float projectionSize, float nearClippingPlane, float farClippingPlane)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float halfRatio = Graphics::usedCamera.lock()->GetAspectRatio() / 2.0f * 10 * (projectionSize / 5.0f);
	float halfOne = 0.5f * 10 * (projectionSize / 5.0f);
	glOrtho(-halfRatio, halfRatio, -halfOne, halfOne, nearClippingPlane, farClippingPlane);
}

void RendererOpengl::SetProjection3D(float fov, float nearClippingPlane, float farClippingPlane, float aspect)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
#if defined(__PSP__)
	glPerspective(fov, Window::GetAspectRatio(), nearClippingPlane, farClippingPlane);
#elif defined(_WIN32) || defined(_WIN64)
	GLfloat zNear = nearClippingPlane;
	GLfloat zFar = farClippingPlane;
	//GLfloat aspect = Graphics::usedCamera.lock()->GetAspectRatio();
	GLfloat fH = tan(float(fov / 360.0f * 3.14159f)) * zNear;
	GLfloat fW = fH * aspect;
	glFrustum(-fW, fW, -fH, fH, zNear, zFar);
	// glPerspective(fov, Window::GetAspectRatio(), nearClippingPlane, farClippingPlane);
#elif defined(__vita__)
	gluPerspective(fov, Window::GetAspectRatio(), nearClippingPlane, farClippingPlane);
#endif
}

void RendererOpengl::ResetView()
{
#if defined(__PSP__)
	glMatrixMode(GL_VIEW);
	glLoadIdentity();
	gluRotateY((0 + 180) / 180.0f * 3.14159f);
#else
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(0 + 180, 0, 1, 0);
#endif
}

void RendererOpengl::SetCameraPosition(std::weak_ptr<Camera> camera)
{
	if (auto cameraLock = camera.lock())
	{
		auto transform = cameraLock->GetTransform();

#if defined(__PSP__)
		glMatrixMode(GL_VIEW);
		glLoadIdentity();

		gluRotateZ((-transform->GetRotation().z) / 180.0f * 3.14159f);
		gluRotateX(transform->GetRotation().x / 180.0f * 3.14159f);
		gluRotateY((transform->GetRotation().y + 180) / 180.0f * 3.14159f);

		glTranslatef(transform->GetPosition().x, -transform->GetPosition().y, -transform->GetPosition().z);
#else
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef(-transform->GetRotation().z, 0, 0, 1);
		glRotatef(transform->GetRotation().x, 1, 0, 0);
		glRotatef(transform->GetRotation().y + 180, 0, 1, 0);
		glTranslatef(transform->GetPosition().x, -transform->GetPosition().y, -transform->GetPosition().z);
#endif
		Setlights(camera);
	}
}

void RendererOpengl::ResetTransform()
{
}

void RendererOpengl::SetTransform(Vector3 position, Vector3 rotation, Vector3 scale, bool resetTransform)
{
#if defined(__PSP__)
	glMatrixMode(GL_MODEL);
	if (resetTransform)
		glLoadIdentity();
#else
	glMatrixMode(GL_MODELVIEW);
#endif
	glTranslatef(-position.x, position.y, position.z);

#if defined(__PSP__)
	gluRotateY(-rotation.y * 3.14159265359 / 180.0f);
	gluRotateX(rotation.x * 3.14159265359 / 180.0f);
	gluRotateZ(-rotation.z * 3.14159265359 / 180.0f);
#else
	glRotatef(-rotation.y, 0, 1, 0);
	glRotatef(rotation.x, 1, 0, 0);
	glRotatef(-rotation.z, 0, 0, 1);
#endif

	glScalef(scale.x, scale.y, scale.z);
}

void RendererOpengl::SetTransform(glm::mat4 &mat)
{
#if defined(__PSP__)
	ScePspFMatrix4 matrix;
	matrix.x.x = mat[0][0];
	matrix.x.y = -mat[1][0];
	matrix.x.z = -mat[2][0];
	matrix.x.w = mat[3][0];

	matrix.y.x = -mat[0][1];
	matrix.y.y = mat[1][1];
	matrix.y.z = -mat[2][1];
	matrix.y.w = mat[3][1];

	matrix.z.x = -mat[0][2];
	matrix.z.y = -mat[1][2];
	matrix.z.z = mat[2][2];
	matrix.z.w = mat[3][2];

	matrix.w.x = mat[3][0];
	matrix.w.y = mat[3][1];
	matrix.w.z = mat[3][2];
	matrix.w.w = mat[3][3];

	sceGuSetMatrix(GU_MODEL, &matrix);
#elif defined(__vita)
	glMatrixMode(GL_MODELVIEW);
	float mat2[4][4];
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			mat2[i][j] = mat[i][j];
		}
	}
	glMultMatrixf((GLfloat *)mat2);
#endif
}

void RendererOpengl::MoveTransform(Vector3 position)
{
#if defined(__PSP__)
	glMatrixMode(GL_MODEL);
#else
	glMatrixMode(GL_MODELVIEW);
#endif
	glTranslatef(-position.x, position.y, position.z);
}

void RendererOpengl::BindTexture(Texture *texture)
{
#if defined(__PSP__)
	glTexMode(texture->type, texture->mipmaplevelCount, 0, 1);
	glTexFunc(GL_TFX_MODULATE, GL_TCC_RGBA);
	if (texture->useMipMap)
		sceGuTexLevelMode(GU_TEXTURE_AUTO, -1); // Greater is lower quality
	// sceGuTexLevelMode(GL_TEXTURE_CONST, 1); // Set mipmap level to use
	// sceGuTexLevelMode(GL_TEXTURE_SLOPE, 2); //??? has no effect

	glTexImage(0, texture->pW, texture->pH, texture->pW, texture->data[0]);
	if (texture->useMipMap)
	{
		glTexImage(1, texture->pW / 2, texture->pH / 2, texture->pW / 2, texture->data[1]);
		// glTexImage(2, texture->pW / 4, texture->pH / 4, texture->pW / 4, texture->data[2]);
		// glTexImage(3, texture->pW / 8, texture->pH / 8, texture->pW / 8, texture->data[3]);
	}

#else
	glBindTexture(GL_TEXTURE_2D, texture->GetTextureId());
#endif
	ApplyTextureFilters(texture);
	float borderColor[] = {0.0f, 0.0f, 0.0f, 0.0f};
	// glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
}

void RendererOpengl::ApplyTextureFilters(Texture *texture)
{
	int minFilterValue = GL_LINEAR;
	int magfilterValue = GL_LINEAR;
	if (texture->GetFilter() == Texture::Bilinear)
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
	else if (texture->GetFilter() == Texture::Point)
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
	int wrap = GetWrapModeEnum(texture->GetWrapMode());

#if defined(__PSP__)
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
	float material_ambient[] = {0.0f, 0.0f, 0.0f, 1.0f};  /* default value */
	float material_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};  /* default value */
	float material_specular[] = {0.0f, 0.0f, 0.0f, 1.0f}; /* NOT default value */
	float material_emission[] = {0.0f, 0.0f, 0.0f, 1.0f}; /* default value */
	// glMaterial(GL_DIFFUSE, 0xFFFFFFFF);
	//  glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
	//  glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
	//  glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
	//  glMaterialfv(GL_FRONT, GL_EMISSION, material_emission);
	//  glMaterialf(GL_FRONT, GL_SHININESS, 10.0);               /* NOT default value   */

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

	if (EngineSettings::useLighting && settings.useLighting)
	{
		glEnable(GL_LIGHTING);
	}
	else
	{
		glDisable(GL_LIGHTING);
	}

	glEnable(GL_TEXTURE_2D);

#if defined(__PSP__)
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
	if (meshData->hasNormal)
	{
		params |= GU_NORMAL_32BITF;
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

#else
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
	if (meshData->hasNormal)
	{
		glEnableClientState(GL_NORMAL_ARRAY);
	}

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	if (meshData->hasColor)
	{
		int stride = sizeof(Vertex);
		glTexCoordPointer(2, GL_FLOAT, stride, &((Vertex *)meshData->data)[0].u);
		glColorPointer(3, GL_FLOAT, stride, &((Vertex *)meshData->data)[0].r);
		glVertexPointer(3, GL_FLOAT, stride, &((Vertex *)meshData->data)[0].x);
	}
	else if (meshData->hasNormal)
	{
		int stride = sizeof(VertexNormalsNoColor);
		glTexCoordPointer(2, GL_FLOAT, stride, &((VertexNormalsNoColor *)meshData->data)[0].u);
		glNormalPointer(GL_FLOAT, stride, &((VertexNormalsNoColor *)meshData->data)[0].normX);
		glVertexPointer(3, GL_FLOAT, stride, &((VertexNormalsNoColor *)meshData->data)[0].x);
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
	if (meshData->hasNormal)
	{
		glDisableClientState(GL_NORMAL_ARRAY);
	}
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

#endif
	Performance::AddDrawCall();
}

unsigned int RendererOpengl::CreateNewTexture()
{
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
	unsigned int textureId;
	glGenTextures(1, &textureId);
	return textureId;
#endif
	return 0;
}

void RendererOpengl::DeleteTexture(Texture *texture)
{
#if defined(__PSP__)
#else
	unsigned int val = texture->GetTextureId();
	glDeleteTextures(1, &val);
#endif
}

void RendererOpengl::SetTextureData(Texture *texture, unsigned int textureType, const unsigned char *buffer)
{
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
	glTexImage2D(GL_TEXTURE_2D, 0, textureType, texture->GetWidth(), texture->GetHeight(), 0, textureType, GL_UNSIGNED_BYTE, buffer);
	if (texture->useMipMap)
		glGenerateMipmap(GL_TEXTURE_2D);
#endif
}

void RendererOpengl::SetLight(int lightIndex, Vector3 lightPosition, float intensity, Color color, Light::LightType type, float attenuation)
{
#if defined(__vita__)
	return;
#endif

	if (lightIndex >= maxLightCount)
		return;

	RGBA rgba = color.GetRGBA();

	glEnable(GL_LIGHT0 + lightIndex);

#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)

	float lightAttenuation[] = {attenuation};
	glLightfv(GL_LIGHT0 + lightIndex, GL_QUADRATIC_ATTENUATION, lightAttenuation);

	float typeIntensity = 1;
	if (type == Light::Directional)
		typeIntensity = 5;
	else if (type == Light::Point)
		typeIntensity = 2;

	float lightColor[] = {rgba.r * intensity * typeIntensity, rgba.g * intensity * typeIntensity, rgba.b * intensity * typeIntensity, 1.0f};
	float zeroLight[] = {0.0f, 0.0f, 0.0f, 1.0f};

	float position[] = {lightPosition.x, lightPosition.y, lightPosition.z, 1};

	// Assign created components to GL_LIGHT0
	if (type == Light::Directional)
	{
		glLightfv(GL_LIGHT0 + lightIndex, GL_AMBIENT, lightColor);
		glLightfv(GL_LIGHT0 + lightIndex, GL_DIFFUSE, zeroLight);
	}
	else
	{
		glLightfv(GL_LIGHT0 + lightIndex, GL_AMBIENT, zeroLight);
		glLightfv(GL_LIGHT0 + lightIndex, GL_DIFFUSE, lightColor);
	}
	glLightfv(GL_LIGHT0 + lightIndex, GL_SPECULAR, zeroLight);
	glLightfv(GL_LIGHT0 + lightIndex, GL_POSITION, position);
#elif defined(__PSP__)
	color.SetFromRGBAfloat(rgba.r * intensity, rgba.g * intensity, rgba.b * intensity, 1);
	ScePspFVector3 pos = {lightPosition.x, lightPosition.y, lightPosition.z};
	sceGuLight(lightIndex, GU_POINTLIGHT, GU_AMBIENT_AND_DIFFUSE, &pos);
	if (type == Light::Directional)
	{
		sceGuLightColor(lightIndex, GU_AMBIENT, color.GetUnsignedIntABGR());
		sceGuLightColor(lightIndex, GU_DIFFUSE, 0x00000000);
	}
	else
	{
		sceGuLightColor(lightIndex, GU_DIFFUSE, color.GetUnsignedIntABGR());
		sceGuLightColor(lightIndex, GU_AMBIENT, 0x00000000);
	}
	sceGuLightColor(lightIndex, GU_SPECULAR, 0x00000000);
	sceGuLightAtt(lightIndex, 0.0f, 0.0f, attenuation);
#endif
}

void RendererOpengl::DisableAllLight()
{
	for (int lightIndex = 0; lightIndex < maxLightCount; lightIndex++)
	{
		glDisable(GL_LIGHT0 + lightIndex);
	}
}

void RendererOpengl::Setlights(std::weak_ptr<Camera> camera)
{
	if (auto cameraLock = camera.lock())
	{
		auto transform = cameraLock->GetTransform();

		DisableAllLight();
		int lightCount = AssetManager::GetLightCount();
		for (int i = 0; i < lightCount; i++)
		{
			auto light = AssetManager::GetLight(i).lock();
			if (light->type == Light::Directional)
			{
				Vector3 dir = Math::GetDirectionFromAngles(-light->GetTransform()->GetRotation().y, -light->GetTransform()->GetRotation().x) * 1000;
				SetLight(i, Vector3(-transform->GetPosition().x, transform->GetPosition().y, transform->GetPosition().z) + dir, light->intensity, light->color, light->type, light->quadratic);
			}
			else
				SetLight(i, light->GetTransform()->GetPosition(), light->intensity, light->color, light->type, light->quadratic);
		}
	}
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
#if defined(_WIN32) || defined(_WIN64)
		mode = GL_CLAMP_TO_EDGE;
#else
		mode = GL_CLAMP;
#endif
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