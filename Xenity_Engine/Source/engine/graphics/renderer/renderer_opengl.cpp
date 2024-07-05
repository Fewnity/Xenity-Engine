// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#if !defined(_EE) && !defined(__PSP__)
#include "renderer_opengl.h"

#include <memory>
#include <engine/graphics/graphics.h>
#include <engine/graphics/camera.h>
#include <engine/graphics/3d_graphics/mesh_data.h>
#include <engine/graphics/material.h>

#include <engine/asset_management/asset_manager.h>
#include <engine/game_elements/gameobject.h>
#include <engine/game_elements/transform.h>
#include <engine/ui/window.h>

#include <engine/debug/debug.h>
#include <engine/debug/performance.h>
#include <engine/tools/profiler_benchmark.h>

#if defined(_WIN32) || defined(_WIN64)
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLFW_DLL
#elif defined(__vita__)
#include <vitaGL.h>
#endif

#include <engine/tools/math.h>
#include <glm/gtc/type_ptr.hpp>

RendererOpengl::RendererOpengl()
{
}

int RendererOpengl::Init()
{
	lastSettings.useTexture = false;

	int result = 1;
#if defined(__vita__)
	//result = vglInit(0x100000);
	result = vglInit(0);
	//result = vglInitExtended(0, 960, 544, 0x1000000, SCE_GXM_MULTISAMPLE_NONE);

	// Enabling V-Sync
	//vglWaitVblankStart(GL_FALSE);
	//vglUseLowPrecision(GL_TRUE);
	//vglSetupRuntimeShaderCompiler(SHARK_OPT_UNSAFE, SHARK_ENABLE, SHARK_ENABLE, SHARK_ENABLE);

	if (result == 0)
		result = 1;

	Window::SetResolution(960, 544);
#elif defined(_WIN32) || defined(_WIN64)
	result = glfwInit();
	Window::SetResolution(1280, 720);
#endif

	Debug::Print("-------- OpenGL Renderer initiated --------", true);

	// 0 is used to say "OK"
	if (result == 1)
		result = 0;
	else
		result = -1;

	return result;
}

void RendererOpengl::Setup()
{
	// TODO: this part needs to be improved
	glEnable(GL_NORMALIZE);

	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

#if defined(_WIN32) || defined(_WIN64)
	glEnable(GL_MULTISAMPLE);
#endif
}

void RendererOpengl::Stop()
{
#if defined(__vita__)
	vglEnd();
#else
	glfwTerminate();
#endif
}

void RendererOpengl::NewFrame()
{
	lastUsedColor = Vector4(-1, -1, -1, -1);
}

void RendererOpengl::EndFrame()
{
	usedTexture = 0;
#if defined(__vita__)
	vglSwapBuffers(GL_FALSE);
#endif
}

void RendererOpengl::SetViewport(int x, int y, int width, int height)
{
	glViewport(x, y, width, height);
}

void RendererOpengl::SetClearColor(const Color& color)
{
	const RGBA& rgba = color.GetRGBA();
	glClearColor(rgba.r, rgba.g, rgba.b, rgba.a);
}

void RendererOpengl::SetProjection2D(float projectionSize, float nearClippingPlane, float farClippingPlane)
{
	if (Graphics::usedCamera)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		const float halfRatio = Graphics::usedCamera->GetAspectRatio() / 2.0f * 10 * (projectionSize / 5.0f);
		const float halfOne = 0.5f * 10 * (projectionSize / 5.0f);
		glOrtho(-halfRatio, halfRatio, -halfOne, halfOne, nearClippingPlane, farClippingPlane);
	}
}

void RendererOpengl::SetProjection3D(float fov, float nearClippingPlane, float farClippingPlane, float aspect)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
#if defined(_WIN32) || defined(_WIN64)
	const GLfloat zNear = nearClippingPlane;
	const GLfloat zFar = farClippingPlane;
	const GLfloat fH = tan(float(fov / 360.0f * 3.14159f)) * zNear;
	const GLfloat fW = fH * aspect;
	glFrustum(-fW, fW, -fH, fH, zNear, zFar);
#elif defined(__vita__)
	gluPerspective(fov, Window::GetAspectRatio(), nearClippingPlane, farClippingPlane);
#endif
}

void RendererOpengl::ResetView()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(180, 0, 1, 0);
}

void RendererOpengl::SetCameraPosition(const Camera& camera)
{
	std::shared_ptr<Transform> transform = camera.GetTransform();
	const Vector3& position = transform->GetPosition();
	const Vector3& rotation = transform->GetRotation();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(-rotation.z, 0, 0, 1);
	glRotatef(rotation.x, 1, 0, 0);
	glRotatef(rotation.y + 180, 0, 1, 0);
	glTranslatef(position.x, -position.y, -position.z);
}

void RendererOpengl::SetCameraPosition(const Vector3& position, const Vector3& rotation)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(-rotation.z, 0, 0, 1);
	glRotatef(rotation.x, 1, 0, 0);
	glRotatef(rotation.y + 180, 0, 1, 0);
	glTranslatef(position.x, -position.y, -position.z);
}

void RendererOpengl::ResetTransform()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void RendererOpengl::SetTransform(const Vector3& position, const Vector3& rotation, const Vector3& scale, bool resetTransform)
{
	glMatrixMode(GL_MODELVIEW);
	glTranslatef(-position.x, position.y, position.z);

	glRotatef(-rotation.y, 0, 1, 0);
	glRotatef(rotation.x, 1, 0, 0);
	glRotatef(-rotation.z, 0, 0, 1);

	glScalef(scale.x, scale.y, scale.z);
}

void RendererOpengl::SetTransform(const glm::mat4& mat)
{
	glMatrixMode(GL_MODELVIEW);
	glMultMatrixf((float*)&mat);
}

void RendererOpengl::BindTexture(const Texture& texture)
{
	glBindTexture(GL_TEXTURE_2D, texture.GetTextureId());
	ApplyTextureFilters(texture);
	//float borderColor[] = { 1.0f, 1.0f, 1.0f, 0.0f };
	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
}

void RendererOpengl::ApplyTextureFilters(const Texture& texture)
{
	// Get the right filter depending of the texture settings
	int minFilterValue = GL_LINEAR;
	int magfilterValue = GL_LINEAR;
	if (texture.GetFilter() == Filter::Bilinear)
	{
		if (texture.GetUseMipmap())
		{
			minFilterValue = GL_LINEAR_MIPMAP_LINEAR;
		}
		else
		{
			minFilterValue = GL_LINEAR;
		}
		magfilterValue = GL_LINEAR;
	}
	else if (texture.GetFilter() == Filter::Point)
	{
		if (texture.GetUseMipmap())
		{
			minFilterValue = GL_NEAREST_MIPMAP_NEAREST;
		}
		else
		{
			minFilterValue = GL_NEAREST;
		}
		magfilterValue = GL_NEAREST;
	}
	const int wrap = GetWrapModeEnum(texture.GetWrapMode());

	// Apply filters

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilterValue);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magfilterValue);
}

void RendererOpengl::DrawSubMesh(const MeshData::SubMesh& subMesh, const Material& material, RenderingSettings& settings)
{
	DrawSubMesh(subMesh, material, *material.GetTexture(), settings);
}

void RendererOpengl::DrawSubMesh(const MeshData::SubMesh& subMesh, const Material& material, const Texture& texture, RenderingSettings& settings)
{
	//float material_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };  /* default value */
	//float material_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };  /* default value */
	//float material_specular[] = { 0.0f, 0.0f, 0.0f, 1.0f }; /* NOT default value */
	//float material_emission[] = { 0.0f, 0.0f, 0.0f, 1.0f }; /* default value */
	// glMaterial(GL_DIFFUSE, 0xFFFFFFFF);
	//  glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
	//  glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
	//  glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
	//  glMaterialfv(GL_FRONT, GL_EMISSION, material_emission);
	//  glMaterialf(GL_FRONT, GL_SHININESS, 10.0);               /* NOT default value   */

	// Apply rendering settings
	if (lastSettings.invertFaces != settings.invertFaces)
	{
		if (settings.invertFaces)
		{
			glFrontFace(GL_CW);
		}
		else
		{
			glFrontFace(GL_CCW);
		}
	}

	if (lastSettings.useDepth != settings.useDepth)
	{
		if (settings.useDepth)
		{
			glEnable(GL_DEPTH_TEST);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}
	}

	if (lastSettings.useBlend != settings.useBlend)
	{
		if (settings.useBlend)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		else
		{
			glDisable(GL_BLEND);
		}
	}

	if (lastSettings.useLighting != settings.useLighting)
	{
		if (settings.useLighting)
		{
			glEnable(GL_LIGHTING);
		}
		else
		{
			glDisable(GL_LIGHTING);
		}
	}

	if (lastSettings.useTexture != settings.useTexture)
	{
		glEnable(GL_TEXTURE_2D);
	}

	if (settings.useBlend)
		glDepthMask(GL_FALSE);

	// Keep in memory the used settings
	lastSettings.invertFaces = settings.invertFaces;
	lastSettings.useBlend = settings.useBlend;
	lastSettings.useDepth = settings.useDepth;
	lastSettings.useLighting = settings.useLighting;
	lastSettings.useTexture = settings.useTexture;

	/*glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);

	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_PRIMARY_COLOR);
	glTexEnvf(GL_TEXTURE_ENV, GL_RGB_SCALE, 2);*/

	const Vector4 colorMix = (material.GetColor() * subMesh.meshData->unifiedColor).GetRGBA().ToVector4();

	if (lastUsedColor != colorMix || (!Graphics::UseOpenGLFixedFunctions && lastShaderIdUsedColor != material.GetShader()->fileId))
	{
		lastUsedColor = colorMix;
		if constexpr (Graphics::UseOpenGLFixedFunctions)
		{
			glColor4f(colorMix.x, colorMix.y, colorMix.z, colorMix.w);
		}
		else 
		{
			lastShaderIdUsedColor = material.GetShader()->fileId;
			material.GetShader()->SetShaderAttribut("color", colorMix);
		}
	}

	//Bind all the data
	glBindVertexArray(subMesh.VAO);

	if (usedTexture != texture.GetTextureId())
	{
		usedTexture = texture.GetTextureId();
		BindTexture(texture);
	}

	if constexpr (Graphics::UseOpenGLFixedFunctions)
	{
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glTranslatef(material.GetOffset().x, material.GetOffset().y, 0);
		glScalef(material.GetTiling().x, material.GetTiling().y, 1.0f);
	}

	// Draw
	if (!subMesh.meshData->hasIndices)
	{
		glDrawArrays(GL_TRIANGLES, 0, subMesh.vertice_count);
	}
	else
	{
		glDrawElements(GL_TRIANGLES, subMesh.index_count, GL_UNSIGNED_SHORT, 0);
	}
	glBindVertexArray(0); // Disable this?
	if (Graphics::usedCamera->IsEditor())
	{
		Performance::AddDrawTriangles(subMesh.vertice_count / 3);
		Performance::AddDrawCall();
	}

	glDepthMask(GL_TRUE);
}

// TODO : Improve this function, it's not optimized and using shaders
void RendererOpengl::DrawLine(const Vector3& a, const Vector3& b, const Color& color, RenderingSettings& settings)
{
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

	lastSettings.useBlend = true;
	lastSettings.useDepth = settings.useDepth;
	lastSettings.useLighting = false;
	lastSettings.useTexture = false;
	usedTexture = 0;

	VertexNoColorNoUv ver[2];
	ver[0].x = a.x;
	ver[0].y = a.y;
	ver[0].z = a.z;

	ver[1].x = b.x;
	ver[1].y = b.y;
	ver[1].z = b.z;
	int stride = sizeof(VertexNoColorNoUv);
	glVertexPointer(3, GL_FLOAT, stride, &ver[0].x);

	const RGBA& vec4Color = color.GetRGBA();
	Vector4 colorToUse = Vector4(vec4Color.r, vec4Color.g, vec4Color.b, vec4Color.a);
	glColor4f(vec4Color.r, vec4Color.g, vec4Color.b, vec4Color.a);
	lastUsedColor = Vector4(-1, -1, -1, -1);
	glDrawArrays(GL_LINES, 0, 2);
}

unsigned int RendererOpengl::CreateNewTexture()
{
	unsigned int textureId;
	glGenTextures(1, &textureId);
	return textureId;
}

void RendererOpengl::DeleteTexture(Texture& texture)
{
	unsigned int textureId = texture.GetTextureId();
	glDeleteTextures(1, &textureId);
}

void RendererOpengl::SetTextureData(const Texture& texture, unsigned int textureType, const unsigned char* buffer)
{
	glTexImage2D(GL_TEXTURE_2D, 0, textureType, texture.GetWidth(), texture.GetHeight(), 0, textureType, GL_UNSIGNED_BYTE, buffer);
	if (texture.GetIsUsingMipMap())
		glGenerateMipmap(GL_TEXTURE_2D);
}

void RendererOpengl::SetLight(const int lightIndex, const Light& light, const Vector3& lightPosition, const Vector3& lightDirection)
{
	// This won't compile on PsVita, and if the code compile, it would not work because the fixed pipeline is broken on vitagl
#if !defined(__vita__)
	if (lightIndex >= maxLightCount)
		return;

	float intensity = light.intensity;
	const Color& color = light.color;
	const LightType& type = light.type;

	glEnable(GL_LIGHT0 + lightIndex);

	float lightAttenuation[1] = { light.GetQuadraticValue() };
	float lightLinearAttenuation[1] = { light.GetLinearValue() };
	float lightConstAttenuation[1] = { 1 };
	if (type == LightType::Directional || type == LightType::Ambient)
	{
		lightAttenuation[0] = { 0 };
		lightLinearAttenuation[0] = { 0 };
		lightConstAttenuation[0] = { 1 };
	}

	glLightfv(GL_LIGHT0 + lightIndex, GL_QUADRATIC_ATTENUATION, lightAttenuation);
	glLightfv(GL_LIGHT0 + lightIndex, GL_LINEAR_ATTENUATION, lightLinearAttenuation);
	glLightfv(GL_LIGHT0 + lightIndex, GL_CONSTANT_ATTENUATION, lightConstAttenuation);

	// Adapt the intensity depending of the light type
	float typeIntensity = 1;
	if (type == LightType::Directional || type == LightType::Ambient)
		typeIntensity = 5;
	else if (type == LightType::Point)
		typeIntensity = 2;
	else if (type == LightType::Spot)
		typeIntensity = 2;

	if (intensity > 1)
		intensity = 1;

	const RGBA& rgba = color.GetRGBA();
	const float lightColor[] = { rgba.r * intensity * typeIntensity, rgba.g * intensity * typeIntensity, rgba.b * intensity * typeIntensity, 1.0f };
	const float zeroLight[] = { 0.0f, 0.0f, 0.0f, 1.0f };

	const float position[] = { -lightPosition.x, lightPosition.y, lightPosition.z, 1 };
	const float direction[] = { lightDirection.x, lightDirection.y, lightDirection.z, 1 };

	// Assign created components to GL_LIGHT0
	if (type == LightType::Directional)
	{
		glLightfv(GL_LIGHT0 + lightIndex, GL_AMBIENT, zeroLight);
		glLightfv(GL_LIGHT0 + lightIndex, GL_DIFFUSE, lightColor);
	}
	else if (type == LightType::Ambient)
	{
		glLightfv(GL_LIGHT0 + lightIndex, GL_AMBIENT, lightColor);
		glLightfv(GL_LIGHT0 + lightIndex, GL_DIFFUSE, zeroLight);
	}
	else
	{
		glLightfv(GL_LIGHT0 + lightIndex, GL_AMBIENT, zeroLight);
		glLightfv(GL_LIGHT0 + lightIndex, GL_DIFFUSE, lightColor);
	}
	if (type == LightType::Spot) 
	{
		float cutOff[1] = { light.GetSpotAngle() };
		// Fixed pipeline does not support more than 90 degrees
		if (cutOff[0] > 90)
			cutOff[0] = 90;
		glLightfv(GL_LIGHT0 + lightIndex, GL_SPOT_CUTOFF, cutOff);
		float exponent[1] = { light.GetSpotSmoothness() * 128 };
		glLightfv(GL_LIGHT0 + lightIndex, GL_SPOT_EXPONENT, exponent);

		glLightfv(GL_LIGHT0 + lightIndex, GL_SPOT_DIRECTION, direction);
	}
	else 
	{
		float zero[1] = { 0 };
		float defaultCutOff[1] = { 180 };
		glLightfv(GL_LIGHT0 + lightIndex, GL_SPOT_CUTOFF, defaultCutOff);
		glLightfv(GL_LIGHT0 + lightIndex, GL_SPOT_EXPONENT, zero);
		glLightfv(GL_LIGHT0 + lightIndex, GL_SPOT_DIRECTION, zeroLight);
	}

	glLightfv(GL_LIGHT0 + lightIndex, GL_SPECULAR, zeroLight);
	glLightfv(GL_LIGHT0 + lightIndex, GL_POSITION, position);
#endif
}

void RendererOpengl::DisableAllLight()
{
	for (int lightIndex = 0; lightIndex < maxLightCount; lightIndex++)
	{
		glDisable(GL_LIGHT0 + lightIndex);
	}
}

void RendererOpengl::Setlights(const Camera& camera)
{
	const std::shared_ptr<Transform> cameraTransform = camera.GetTransform();

	DisableAllLight();
	const int lightCount = AssetManager::GetLightCount();
	int usedLightCount = 0;
	for (int i = 0; i < lightCount; i++)
	{
		std::shared_ptr<Light> light = AssetManager::GetLight(i).lock();
		if (light && light->IsEnabled() && light->GetGameObject()->IsLocalActive())
		{
			if (light->type == LightType::Directional)
			{
				const Vector3& lightRotation = light->GetTransform()->GetRotation();
				const Vector3 dir = Math::Get3DDirectionFromAngles(lightRotation.y, -lightRotation.x) * 1000;
				SetLight(usedLightCount, *light,  Vector3(0, 0, 0) + dir, dir);
			}
			else if (light->type == LightType::Ambient)
			{
				SetLight(usedLightCount, *light, Vector3(0, 0, 0), Vector3(0, 0, 0));
			}
			else if (light->type == LightType::Spot)
			{
				const Vector3& lightRotation = light->GetTransform()->GetRotation();
				const Vector3 dir = Math::Get3DDirectionFromAngles(-lightRotation.y, -lightRotation.x + 180).Normalized();
				SetLight(usedLightCount, *light, light->GetTransform()->GetPosition(), dir);
			}
			else
			{
				SetLight(usedLightCount, *light, light->GetTransform()->GetPosition(), Vector3(0, 0, 0));
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
}

void RendererOpengl::SetFogValues(float start, float end, const Color& color)
{
	fogStart = start;
	fogEnd = end;
	fogColor = color;
	glFogi(GL_FOG_MODE, GL_LINEAR);
	//glFogi(GL_FOG_MODE, GL_EXP);
	//glFogi(GL_FOG_MODE, GL_EXP2);
	glFogf(GL_FOG_DENSITY, 1.0f);
	glFogf(GL_FOG_START, start);
	glFogf(GL_FOG_END, end);
	const RGBA& rgba = color.GetRGBA();
	float floatColor[] = { rgba.r, rgba.g, rgba.b, 1.0f };

	glFogfv(GL_FOG_COLOR, floatColor);

}

unsigned int RendererOpengl::CreateBuffer()
{
	unsigned int id = 0;
	glGenBuffers(1, &id);
	return id;
}

unsigned int RendererOpengl::CreateVertexArray()
{
	unsigned int id = 0;
	glGenVertexArrays(1, &id);
	return id;
}

void RendererOpengl::BindVertexArray(unsigned int bufferId)
{
	glBindVertexArray(bufferId);
}

void RendererOpengl::DeleteBuffer(unsigned int bufferId)
{
	glDeleteBuffers(1, &bufferId);
}

void RendererOpengl::DeleteVertexArray(unsigned int bufferId)
{
	glDeleteVertexArrays(1, &bufferId);
}

void RendererOpengl::DeleteSubMeshData(MeshData::SubMesh& subMesh)
{
	if (subMesh.VAO != 0)
		DeleteVertexArray(subMesh.VAO);
	if (subMesh.VBO != 0)
		DeleteBuffer(subMesh.VBO);
	if (subMesh.EBO != 0)
		DeleteBuffer(subMesh.EBO);
}

void RendererOpengl::UploadMeshData(const MeshData& meshData)
{
	for (int i = 0; i < meshData.subMeshCount; i++)
	{
		MeshData::SubMesh* newSubMesh = meshData.subMeshes[i];

		if (newSubMesh->VAO == 0)
			newSubMesh->VAO = CreateVertexArray();

		glBindVertexArray(newSubMesh->VAO);

		if (newSubMesh->VBO == 0)
			newSubMesh->VBO = CreateBuffer();

		glBindBuffer(GL_ARRAY_BUFFER, newSubMesh->VBO);

		if (!meshData.hasNormal)
		{
			if (!meshData.hasUv)
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
			if (!meshData.hasUv)
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
		if (!meshData.hasNormal)
		{
			if (!meshData.hasUv)
			{
				stride = sizeof(VertexNoColorNoUv);
				//const VertexNoColorNoUv* data = (VertexNoColorNoUv*)subMesh->data;
				//glVertexPointer(3, GL_FLOAT, stride, &data[0].x);

				glVertexPointer(3, GL_FLOAT, stride, (void*)offsetof(VertexNoColorNoUv, x));
				return;
			}
			else
			{
				if (meshData.hasColor)
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

					if constexpr (Graphics::UseOpenGLFixedFunctions)
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
			if (!meshData.hasUv)
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
				if constexpr (Graphics::UseOpenGLFixedFunctions)
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
}

unsigned int RendererOpengl::CreateShader(Shader::ShaderType type)
{
	const int compileType = GetShaderTypeEnum(type);
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
#if defined(__vita__)
	glBindAttribLocation(programId, 0, "position");
	glBindAttribLocation(programId, 1, "uv");
	glBindAttribLocation(programId, 2, "normal");
#endif
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

void RendererOpengl::SetShaderAttribut(unsigned int programId, unsigned int attributId, const Vector4& value)
{
	glUniform4f(attributId, value.x, value.y, value.z, value.w);
}

void RendererOpengl::SetShaderAttribut(unsigned int programId, unsigned int attributId, const Vector3& value)
{
	glUniform3f(attributId, value.x, value.y, value.z);
}

void RendererOpengl::SetShaderAttribut(unsigned int programId, unsigned int attributId, const Vector2& value)
{
	glUniform2f(attributId, value.x, value.y);
}

void RendererOpengl::SetShaderAttribut(unsigned int programId, unsigned int attributId, const float value)
{
	glUniform1f(attributId, value);
}

void RendererOpengl::SetShaderAttribut(unsigned int programId, unsigned int attributId, const int value)
{
	glUniform1i(attributId, value);
}

void RendererOpengl::SetShaderAttribut(unsigned int programId, unsigned int attributId, const glm::mat4& trans)
{
	glUniformMatrix4fv(attributId, 1, false, glm::value_ptr(trans));
}

void RendererOpengl::SetShaderAttribut(unsigned int programId, unsigned int attributId, const glm::mat3& trans)
{
	glUniformMatrix3fv(attributId, 1, false, glm::value_ptr(trans));
}

int RendererOpengl::GetBufferTypeEnum(BufferType bufferType)
{
	int type = GL_REPEAT;
	switch (bufferType)
	{
	case BufferType::Array_Buffer:
		type = GL_ARRAY_BUFFER;
		break;
	case BufferType::Element_Array_Buffer:
		type = GL_ELEMENT_ARRAY_BUFFER;
		break;
	}
	return type;

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

int RendererOpengl::GetWrapModeEnum(WrapMode wrapMode)
{
	int mode = GL_REPEAT;
	switch (wrapMode)
	{
	case WrapMode::ClampToEdge:
	case WrapMode::ClampToBorder:
#if defined(_WIN32) || defined(_WIN64)
		mode = GL_CLAMP_TO_EDGE;
#else
		mode = GL_CLAMP;
#endif
		break;
	case WrapMode::Repeat:
		mode = GL_REPEAT;
		break;

		// case WrapMode::ClampToEdge:
		// 	mode = GL_CLAMP_TO_EDGE;
		// 	break;
		// case WrapMode::ClampToBorder:
		// 	mode = GL_CLAMP_TO_BORDER;
		// 	break;
		// case WrapMode::MirroredRepeat:
		// 	mode = GL_MIRRORED_REPEAT;
		// 	break;
		// case WrapMode::Repeat:
		// 	mode = GL_REPEAT;
		// 	break;
		// case WrapMode::MirrorClampToEdge:
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
	int compileType = GL_VERTEX_SHADER;
	switch (shaderType)
	{
	case Shader::ShaderType::Vertex_Shader:
		compileType = GL_VERTEX_SHADER;
		break;
	case Shader::ShaderType::Fragment_Shader:
		compileType = GL_FRAGMENT_SHADER;
		break;
#if !defined(__vita__)
	case Shader::ShaderType::Tessellation_Control_Shader:
		compileType = GL_TESS_CONTROL_SHADER;
		break;
	case Shader::ShaderType::Tessellation_Evaluation_Shader:
		compileType = GL_TESS_EVALUATION_SHADER;
		break;
#endif
	}
	return compileType;
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
#endif