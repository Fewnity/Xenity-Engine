#include "renderer_opengl.h"

#include "../../../xenity.h"
#include "../3d_graphics/mesh_data.h"
#include "../../tools/profiler_benchmark.h"

#if defined(_WIN32) || defined(_WIN64)
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLFW_DLL
#elif defined(__PSP__)
static unsigned int __attribute__((aligned(16))) list[262144];
#include <pspkernel.h>
#define GUGL_IMPLEMENTATION
#include "../../../psp/gu2gl.h"
#elif defined(__vita__)
#include <vitaGL.h>
#endif

#include <memory>
#include <glm/gtc/type_ptr.hpp>

ProfilerBenchmark* applySettingsBenchmark = nullptr;
ProfilerBenchmark* mesh2Benchmark = nullptr;

RendererOpengl::RendererOpengl()
{
	applySettingsBenchmark = new ProfilerBenchmark("Draw", "Settings");
	mesh2Benchmark = new ProfilerBenchmark("Draw", "mesh");
}

int RendererOpengl::Init()
{
	int result = 1;
#if defined(__PSP__)
	guglInit(list);
	maxLightCount = 4;
#elif defined(__vita__)
	//result = vglInit(0x100000);
	result = vglInit(0);
	//result = vglInitExtended(0, 960, 544, 0x1000000, SCE_GXM_MULTISAMPLE_NONE);

	// Enabling V-Sync
	//vglWaitVblankStart(GL_FALSE);
	//vglUseLowPrecision(GL_TRUE);
	//vglSetupRuntimeShaderCompiler(SHARK_OPT_UNSAFE, SHARK_ENABLE, SHARK_ENABLE, SHARK_ENABLE);

	if (result == 0)
		result = 1;
#elif defined(_WIN32) || defined(_WIN64)
	result = glfwInit();
#endif

	Debug::Print("-------- OpenGL Renderer initiated --------");

	// 0 is used to say "OK"
	if (result == 1)
		result = 0;
	else
		result = -1;

	return result;
}

void RendererOpengl::Setup()
{
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
	glEnable(GL_NORMALIZE);

	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

#if defined(_WIN32) || defined(_WIN64)
	glEnable(GL_MULTISAMPLE);
#endif

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

void RendererOpengl::SetClearColor(const Color& color)
{
#if defined(__PSP__)
	glClearColor(color.GetUnsignedIntABGR());
#else
	RGBA rgba = color.GetRGBA();
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
	GLfloat fH = tan(float(fov / 360.0f * 3.14159f)) * zNear;
	GLfloat fW = fH * aspect;
	glFrustum(-fW, fW, -fH, fH, zNear, zFar);
#elif defined(__vita__)
	gluPerspective(fov, Window::GetAspectRatio(), nearClippingPlane, farClippingPlane);
#endif
}

void RendererOpengl::ResetView()
{
#if defined(__PSP__)
	glMatrixMode(GL_VIEW);
	glLoadIdentity();
	gluRotateY(180 / 180.0f * 3.14159f);
#else
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(180, 0, 1, 0);
#endif
}

void RendererOpengl::SetCameraPosition(const std::shared_ptr<Camera>& camera)
{
	auto transform = camera->GetTransform();
	Vector3 position = transform->GetPosition();
	Vector3 rotation = transform->GetRotation();
#if defined(__PSP__)
	glMatrixMode(GL_VIEW);
	glLoadIdentity();

	gluRotateZ((-rotation.z) / 180.0f * 3.14159f);
	gluRotateX(rotation.x / 180.0f * 3.14159f);
	gluRotateY((rotation.y + 180) / 180.0f * 3.14159f);

	glTranslatef(position.x, -position.y, -position.z);
#else
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(-rotation.z, 0, 0, 1);
	glRotatef(rotation.x, 1, 0, 0);
	glRotatef(rotation.y + 180, 0, 1, 0);
	glTranslatef(position.x, -position.y, -position.z);
#endif
	Setlights(camera);
}

void RendererOpengl::ResetTransform()
{
#if defined(__PSP__)
	glMatrixMode(GL_MODEL);
	glLoadIdentity();
#else
	glMatrixMode(GL_MODELVIEW);
#endif
}

void RendererOpengl::SetTransform(const Vector3& position, const Vector3& rotation, const Vector3& scale, bool resetTransform)
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

void RendererOpengl::SetTransform(const glm::mat4& mat)
{
#if defined(__PSP__)
	sceGuSetMatrix(GU_MODEL, (ScePspFMatrix4*)&mat);
#else
	glMatrixMode(GL_MODELVIEW);
	glMultMatrixf((float*)&mat);
#endif
}

void RendererOpengl::BindTexture(const std::shared_ptr<Texture>& texture)
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
	//float borderColor[] = { 1.0f, 1.0f, 1.0f, 0.0f };
	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
}

void RendererOpengl::ApplyTextureFilters(const std::shared_ptr<Texture>& texture)
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

void RendererOpengl::DrawMeshData(const std::shared_ptr <MeshData>& meshData, const std::vector<std::shared_ptr<Texture>>& textures, RenderingSettings& settings)
{
	if (!meshData->isValid)
		return;

	float material_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };  /* default value */
	float material_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };  /* default value */
	float material_specular[] = { 0.0f, 0.0f, 0.0f, 1.0f }; /* NOT default value */
	float material_emission[] = { 0.0f, 0.0f, 0.0f, 1.0f }; /* default value */
	// glMaterial(GL_DIFFUSE, 0xFFFFFFFF);
	//  glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
	//  glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
	//  glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
	//  glMaterialfv(GL_FRONT, GL_EMISSION, material_emission);
	//  glMaterialf(GL_FRONT, GL_SHININESS, 10.0);               /* NOT default value   */

	applySettingsBenchmark->Start();
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

#if defined(__PSP__)
	if (EngineSettings::useLighting && settings.useLighting && !settings.useBlend)
#else
	if (EngineSettings::useLighting && settings.useLighting)
#endif
	{
		glEnable(GL_LIGHTING);
	}
	else
	{
		glDisable(GL_LIGHTING);
	}

	glEnable(GL_TEXTURE_2D);
	applySettingsBenchmark->Stop();

	int subMeshCount = meshData->subMeshCount;
	int textureCount = textures.size();
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

	for (int i = 0; i < subMeshCount; i++)
	{
		if (i == textureCount)
			break;
		if (textures[i] == nullptr)
			continue;
		BindTexture(textures[i]);
		MeshData::SubMesh* subMesh = meshData->subMeshes[i];

		if (!meshData->hasIndices)
		{
			glDrawElements(GL_TRIANGLES, params, subMesh->vertice_count, 0, subMesh->data);
		}
		else
		{
			glDrawElements(GL_TRIANGLES, params, subMesh->index_count, subMesh->indices, subMesh->data);
		}
	}
#else
	/*glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);

	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_PRIMARY_COLOR);
	glTexEnvf(GL_TEXTURE_ENV, GL_RGB_SCALE, 2);*/

	applySettingsBenchmark->Start();

	if (Graphics::UseOpenGLFixedFunctions)
	{
		RGBA rgba = meshData->unifiedColor.GetRGBA();
		glColor4f(rgba.r, rgba.g, rgba.b, rgba.a);
	}

	applySettingsBenchmark->Stop();
	MeshData::SubMesh* subMesh = nullptr;
	int stride = 0;
	for (int i = 0; i < subMeshCount; i++)
	{
		if (i == textureCount)
			break;

		if (textures[i] == nullptr)
			continue;

		subMesh = meshData->subMeshes[i];

		if (subMesh->vertice_count == 0)
			continue;

		glBindVertexArray(subMesh->VAO);

		BindTexture(textures[i]);

		mesh2Benchmark->Start();

		if (!meshData->hasIndices)
		{
			//glDrawArrays(GL_TRIANGLES, 0, subMesh->vertice_count);
		}
		else
		{
			//glDrawElements(GL_TRIANGLES, subMesh->index_count, GL_UNSIGNED_SHORT, subMesh->indices);
			glDrawElements(GL_TRIANGLES, subMesh->index_count, GL_UNSIGNED_SHORT, 0);
		}
		glBindVertexArray(0);
		mesh2Benchmark->Stop();
	}

#endif

	Performance::AddDrawCall();
}

void RendererOpengl::DrawLine(const Vector3& a, const Vector3& b, const Color& color, RenderingSettings& settings)
{
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
	if (settings.useDepth)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	VertexNoColorNoUv ver[2];
	ver[0].x = a.x;
	ver[0].y = a.y;
	ver[0].z = a.z;

	ver[1].x = b.x;
	ver[1].y = b.y;
	ver[1].z = b.z;
	int stride = sizeof(VertexNoColorNoUv);
	glVertexPointer(3, GL_FLOAT, stride, &ver[0].x);

	RGBA vec4Color = color.GetRGBA();
	glColor4f(vec4Color.r, vec4Color.g, vec4Color.b, vec4Color.a);
	glDrawArrays(GL_LINES, 0, 2);
#endif
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

void RendererOpengl::DeleteTexture(Texture* texture)
{
#if defined(__PSP__)
#else
	unsigned int val = texture->GetTextureId();
	glDeleteTextures(1, &val);
#endif
}

void RendererOpengl::SetTextureData(const std::shared_ptr <Texture>& texture, unsigned int textureType, const unsigned char* buffer)
{
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
	glTexImage2D(GL_TEXTURE_2D, 0, textureType, texture->GetWidth(), texture->GetHeight(), 0, textureType, GL_UNSIGNED_BYTE, buffer);
	if (texture->useMipMap)
		glGenerateMipmap(GL_TEXTURE_2D);
#endif
}

void RendererOpengl::SetLight(int lightIndex, const Vector3& lightPosition, float intensity, Color color, Light::LightType type, float attenuation)
{
#if defined(__vita__)
	return;
#endif

	if (lightIndex >= maxLightCount)
		return;

	RGBA rgba = color.GetRGBA();

	glEnable(GL_LIGHT0 + lightIndex);

#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)

	float lightAttenuation[1] = { attenuation };
	if (type == Light::Directional)
		lightAttenuation[0] = { 0 };

	glLightfv(GL_LIGHT0 + lightIndex, GL_QUADRATIC_ATTENUATION, lightAttenuation);

	float typeIntensity = 1;
	if (type == Light::Directional)
		typeIntensity = 5;
	else if (type == Light::Point)
		typeIntensity = 2;

	float lightColor[] = { rgba.r * intensity * typeIntensity, rgba.g * intensity * typeIntensity, rgba.b * intensity * typeIntensity, 1.0f };
	float zeroLight[] = { 0.0f, 0.0f, 0.0f, 1.0f };

	float position[] = { -lightPosition.x, lightPosition.y, lightPosition.z, 1 };

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
	ScePspFVector3 pos = { -lightPosition.x, lightPosition.y, lightPosition.z };
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
	if (type == Light::Directional)
		attenuation = 0;
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

void RendererOpengl::Setlights(const std::shared_ptr<Camera>& camera)
{
	auto cameraTransform = camera->GetTransform();

	DisableAllLight();
	int lightCount = AssetManager::GetLightCount();
	int usedLightCount = 0;
	for (int i = 0; i < lightCount; i++)
	{
		auto light = AssetManager::GetLight(i).lock();
		if (light && light->GetIsEnabled() && light->GetGameObject()->GetLocalActive())
		{
			if (light->type == Light::Directional)
			{
				Vector3 lightRotation = light->GetTransform()->GetRotation();
				Vector3 cameraPosition = cameraTransform->GetPosition();
				Vector3 dir = Math::Get3DDirectionFromAngles(-lightRotation.y, -lightRotation.x) * 1000;
				SetLight(usedLightCount, Vector3(-cameraPosition.x, cameraPosition.y, cameraPosition.z) + dir, light->GetIntensity(), light->color, light->type, light->quadratic);
			}
			else
			{
				SetLight(usedLightCount, light->GetTransform()->GetPosition(), light->GetIntensity(), light->color, light->type, light->quadratic);
			}
			usedLightCount++;
			if (usedLightCount == maxLightCount)
				break;
		}
	}
}

void RendererOpengl::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT /*| GL_STENCIL_BUFFER_BIT*/);
	// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void RendererOpengl::SetFog(bool active)
{
	if (active)
		glEnable(GL_FOG);
	else
		glDisable(GL_FOG);
#if defined(__PSP__)
	if (active)
		sceGuFog(fogStart, fogEnd, fogColor.GetUnsignedIntABGR());
#endif
}

void RendererOpengl::SetFogValues(float start, float end, const Color& color)
{
	fogStart = start;
	fogEnd = end;
	fogColor = color;
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
	glFogi(GL_FOG_MODE, GL_LINEAR);
	//glFogi(GL_FOG_MODE, GL_EXP);
	//glFogi(GL_FOG_MODE, GL_EXP2);
	glFogf(GL_FOG_DENSITY, 1.0f);
	glFogf(GL_FOG_START, start);
	glFogf(GL_FOG_END, end);
	RGBA rgba = color.GetRGBA();
	float floatColor[] = { rgba.r, rgba.g, rgba.b, 1.0f };

	glFogfv(GL_FOG_COLOR, floatColor);
#elif defined(__PSP__)
	sceGuFog(fogStart, fogEnd, fogColor.GetUnsignedIntABGR());
#endif
}

unsigned int RendererOpengl::CreateBuffer()
{
	unsigned int id = 0;
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
	glGenBuffers(1, &id);
#endif
	return id;
}

unsigned int RendererOpengl::CreateVertexArray()
{
	unsigned int id = 0;
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
	glGenVertexArrays(1, &id);
#endif
	return id;
}

void RendererOpengl::BindVertexArray(unsigned int bufferId)
{
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
	glBindVertexArray(bufferId);
#endif
}

void RendererOpengl::DeleteBuffer(unsigned int bufferId)
{
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
	glDeleteBuffers(1, &bufferId);
#endif
}

void RendererOpengl::DeleteVertexArray(unsigned int bufferId)
{
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
	glDeleteVertexArrays(1, &bufferId);
#endif
}

void RendererOpengl::DeleteSubMeshData(MeshData::SubMesh* subMesh)
{
	if (subMesh->VAO != 0)
		DeleteVertexArray(subMesh->VAO);
	if (subMesh->VBO != 0)
		DeleteBuffer(subMesh->VBO);
	if (subMesh->EBO != 0)
		DeleteBuffer(subMesh->EBO);
}

void RendererOpengl::UploadMeshData(const std::shared_ptr<MeshData>& meshData)
{
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
	for (int i = 0; i < meshData->subMeshCount; i++)
	{
		MeshData::SubMesh* newSubMesh = meshData->subMeshes[i];

		if (newSubMesh->VAO == 0)
			newSubMesh->VAO = CreateVertexArray();

		glBindVertexArray(newSubMesh->VAO);

		if (newSubMesh->VBO == 0)
			newSubMesh->VBO = CreateBuffer();

		glBindBuffer(GL_ARRAY_BUFFER, newSubMesh->VBO);

		if (!meshData->hasNormal)
		{
			if (!meshData->hasUv)
			{
				glBufferData(GL_ARRAY_BUFFER, sizeof(VertexNoColorNoUv) * newSubMesh->vertice_count, newSubMesh->data, GL_STATIC_DRAW);
			}
			else
			{
				glBufferData(GL_ARRAY_BUFFER, sizeof(VertexNoColor) * newSubMesh->vertice_count, newSubMesh->data, GL_STATIC_DRAW);
			}
		}
		else
		{
			if (!meshData->hasUv)
			{
				glBufferData(GL_ARRAY_BUFFER, sizeof(VertexNormalsNoColorNoUv) * newSubMesh->vertice_count, newSubMesh->data, GL_STATIC_DRAW);
			}
			else
			{
				glBufferData(GL_ARRAY_BUFFER, sizeof(VertexNormalsNoColor) * newSubMesh->vertice_count, newSubMesh->data, GL_STATIC_DRAW);
			}
		}

		if (newSubMesh->EBO == 0)
			newSubMesh->EBO = CreateBuffer();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, newSubMesh->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * newSubMesh->index_count, newSubMesh->indices, GL_STATIC_DRAW);

		int stride;
		if (!meshData->hasNormal)
		{
			if (!meshData->hasUv)
			{
				stride = sizeof(VertexNoColorNoUv);
				//const VertexNoColorNoUv* data = (VertexNoColorNoUv*)subMesh->data;
				//glVertexPointer(3, GL_FLOAT, stride, &data[0].x);

				glVertexPointer(3, GL_FLOAT, stride, (void*)offsetof(VertexNoColorNoUv, x));
				return;
			}
			else
			{
				if (meshData->hasColor)
				{
					stride = sizeof(Vertex);
					//const Vertex* data = (Vertex*)subMesh->data;
					/*glTexCoordPointer(2, GL_FLOAT, stride, &data[0].u);
					glColorPointer(3, GL_FLOAT, stride, &data[0].r);
					glVertexPointer(3, GL_FLOAT, stride, &data[0].x);*/

					glTexCoordPointer(2, GL_FLOAT, stride, (void*)offsetof(Vertex, u));
					glColorPointer(3, GL_FLOAT, stride, (void*)offsetof(Vertex, r));
					glVertexPointer(3, GL_FLOAT, stride, (void*)offsetof(Vertex, x));
					return;
				}
				else
				{
					stride = sizeof(VertexNoColor);
					//const VertexNoColor* data = (VertexNoColor*)subMesh->data;
					//glTexCoordPointer(2, GL_FLOAT, stride, &data[0].u);
					//glVertexPointer(3, GL_FLOAT, stride, &data[0].x);

					if (Graphics::UseOpenGLFixedFunctions)
					{
						glEnableClientState(GL_VERTEX_ARRAY);
						glVertexPointer(3, GL_FLOAT, stride, (void*)offsetof(VertexNoColor, x));
						glEnableClientState(GL_TEXTURE_COORD_ARRAY);
						glTexCoordPointer(2, GL_FLOAT, stride, (void*)offsetof(VertexNoColor, u));
					}
					else
					{
						glEnableVertexAttribArray(0);
						glVertexAttribPointer(0, 3, GL_FLOAT, false, stride, (void*)offsetof(VertexNoColor, x));
						glEnableVertexAttribArray(1);
						glVertexAttribPointer(1, 2, GL_FLOAT, false, stride, (void*)offsetof(VertexNoColor, u));
						//glEnableVertexAttribArray(2);
						//glVertexAttribPointer(2, 3, GL_FLOAT, false, stride, (void*)offsetof(VertexNoColor, x));
					}
					//glEnableVertexAttribArray(2);
				}
			}
		}
		else
		{
			if (!meshData->hasUv)
			{
				stride = sizeof(VertexNormalsNoColorNoUv);
				//const VertexNormalsNoColorNoUv* data = (VertexNormalsNoColorNoUv*)subMesh->data;
				//glNormalPointer(GL_FLOAT, stride, &data[0].normX);
				//glVertexPointer(3, GL_FLOAT, stride, &data[0].x);

				glNormalPointer(GL_FLOAT, stride, (void*)offsetof(VertexNormalsNoColorNoUv, normX));
				glVertexPointer(3, GL_FLOAT, stride, (void*)offsetof(VertexNormalsNoColorNoUv, x));
				//return;
			}
			else
			{
				stride = sizeof(VertexNormalsNoColor);
				if (Graphics::UseOpenGLFixedFunctions)
				{
					glEnableClientState(GL_VERTEX_ARRAY);
					glVertexPointer(3, GL_FLOAT, stride, (void*)offsetof(VertexNormalsNoColor, x));
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					glTexCoordPointer(2, GL_FLOAT, stride, (void*)offsetof(VertexNormalsNoColor, u));
					glEnableClientState(GL_NORMAL_ARRAY);
					glNormalPointer(GL_FLOAT, stride, (void*)offsetof(VertexNormalsNoColor, normX));
				}
				else
				{
					glEnableVertexAttribArray(0);
					glVertexAttribPointer(0, 3, GL_FLOAT, false, stride, (void*)offsetof(VertexNormalsNoColor, x));
					glEnableVertexAttribArray(1);
					glVertexAttribPointer(1, 2, GL_FLOAT, false, stride, (void*)offsetof(VertexNormalsNoColor, u));
					glEnableVertexAttribArray(2);
					glVertexAttribPointer(2, 3, GL_FLOAT, false, stride, (void*)offsetof(VertexNormalsNoColor, normX));
				}
			}
		}

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
#endif
}

unsigned int RendererOpengl::CreateShader(Shader::ShaderType type)
{
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
	int compileType = GetShaderTypeEnum(type);
	return glCreateShader(compileType);
#endif
	return 0;
}

unsigned int RendererOpengl::CreateShaderProgram()
{
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
	return glCreateProgram();
#endif
	return 0;
}

void RendererOpengl::CompileShader(unsigned int shaderId)
{
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
	glCompileShader(shaderId);
#endif
}

int RendererOpengl::GetShaderCompilationResult(unsigned int shaderId)
{
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
	GLint vResult;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &vResult);
	return (int)vResult;
#endif
	return 0;
}

std::vector<char> RendererOpengl::GetCompilationError(unsigned int shaderId)
{
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
	int maxLength = 256;
	std::vector<char> errorLog(maxLength);
	glGetShaderInfoLog(shaderId, maxLength, &maxLength, &errorLog[0]);
	return errorLog;
#endif
	return std::vector<char>();
}

void RendererOpengl::SetShaderData(unsigned int shaderId, const char* data)
{
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
	glShaderSource(shaderId, 1, &data, NULL);
#endif
}

void RendererOpengl::DeleteShader(unsigned int shaderId)
{
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
	glDeleteShader(shaderId);
#endif
}

void RendererOpengl::DeleteShaderProgram(unsigned int programId)
{
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
	glDeleteProgram(programId);
#endif
}

void RendererOpengl::LinkShaderProgram(unsigned int programId)
{
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
#if defined(__vita__)
	glBindAttribLocation(programId, 0, "position");
	glBindAttribLocation(programId, 1, "uv");
	glBindAttribLocation(programId, 2, "normal");
#endif
	glLinkProgram(programId);
#endif
}

void RendererOpengl::UseShaderProgram(unsigned int programId)
{
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
	glUseProgram(programId);
#endif
}

unsigned int RendererOpengl::GetShaderUniformLocation(unsigned int programId, const char* name)
{
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
	return glGetUniformLocation(programId, name);
#endif
	return 0;
}

void RendererOpengl::AttachShader(unsigned int programId, unsigned int shaderId)
{
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
	glAttachShader(programId, shaderId);
#endif
}

void RendererOpengl::SetShaderAttribut(unsigned int programId, const char* attribut, const Vector4& value)
{
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
	glUniform4f(GetShaderUniformLocation(programId, attribut), value.x, value.y, value.z, value.w);
#endif
}

void RendererOpengl::SetShaderAttribut(unsigned int programId, const char* attribut, const Vector3& value)
{
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
	glUniform3f(GetShaderUniformLocation(programId, attribut), value.x, value.y, value.z);
#endif

}

void RendererOpengl::SetShaderAttribut(unsigned int programId, const char* attribut, const Vector2& value)
{
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
	glUniform2f(GetShaderUniformLocation(programId, attribut), value.x, value.y);
#endif
}

void RendererOpengl::SetShaderAttribut(unsigned int programId, const char* attribut, const float value)
{
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
	glUniform1f(GetShaderUniformLocation(programId, attribut), value);
#endif
}

void RendererOpengl::SetShaderAttribut(unsigned int programId, const char* attribut, const int value)
{
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
	glUniform1i(GetShaderUniformLocation(programId, attribut), value);
#endif
}

void RendererOpengl::SetShaderAttribut(unsigned int programId, const char* attribut, const glm::mat4& trans)
{
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
	glUniformMatrix4fv(GetShaderUniformLocation(programId, attribut), 1, false, glm::value_ptr(trans));
#endif
}

void RendererOpengl::SetShaderAttribut(unsigned int programId, const char* attribut, const glm::mat3& trans)
{
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
	glUniformMatrix3fv(GetShaderUniformLocation(programId, attribut), 1, false, glm::value_ptr(trans));
#endif
}

int RendererOpengl::GetBufferTypeEnum(BufferType bufferType)
{
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
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
#endif
	return 0;
}

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

int RendererOpengl::GetShaderTypeEnum(Shader::ShaderType shaderType)
{
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
	int compileType = GL_VERTEX_SHADER;
	switch (shaderType)
	{
	case Shader::Vertex_Shader:
		compileType = GL_VERTEX_SHADER;
		break;
	case Shader::Fragment_Shader:
		compileType = GL_FRAGMENT_SHADER;
		break;
#if !defined(__vita__)
	case Shader::Tessellation_Control_Shader:
		compileType = GL_TESS_CONTROL_SHADER;
		break;
	case Shader::Tessellation_Evaluation_Shader:
		compileType = GL_TESS_EVALUATION_SHADER;
		break;
#endif
	}
	return compileType;
#endif
	return 0;
}

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