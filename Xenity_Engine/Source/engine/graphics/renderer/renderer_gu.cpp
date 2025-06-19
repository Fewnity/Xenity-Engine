// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#if defined(__PSP__)
#include "renderer_gu.h"

#include <memory>

#include <glm/gtc/type_ptr.hpp>
#include <pspkernel.h>
#include <vram.h>
#include <pspdisplay.h>
#include <pspgu.h>
#include <pspgum.h>

#include <engine/graphics/3d_graphics/mesh_data.h>
#include <engine/tools/profiler_benchmark.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/tools/internal_math.h>
#include <engine/debug/performance.h>
#include <engine/graphics/graphics.h>
#include <engine/graphics/material.h>
#include <engine/ui/window.h>
#include <engine/graphics/camera.h>
#include <engine/game_elements/gameobject.h>
#include <engine/game_elements/transform.h>
#include <engine/ui/screen.h>
#include <engine/graphics/texture/texture_psp.h>

static unsigned int __attribute__((aligned(16))) list[262144];

#define PSP_BUF_WIDTH 512
#define PSP_SCR_WIDTH 480
#define PSP_SCR_HEIGHT 272

RendererGU::RendererGU()
{
}

int RendererGU::Init()
{
	int result = 0;

	int displayBufferByteCount = 2;
	int displayColorMode = GU_PSM_5650;
	if (useHighQualityColor)
	{
		displayBufferByteCount = 4;
		displayColorMode = GU_PSM_8888;
	}

	void* fbp0 = vramalloc(PSP_BUF_WIDTH * PSP_SCR_HEIGHT * displayBufferByteCount);
	void* fbp1 = vramalloc(PSP_BUF_WIDTH * PSP_SCR_HEIGHT * displayBufferByteCount);
	void* zbp = vramalloc(PSP_BUF_WIDTH * PSP_SCR_HEIGHT * 2);

	m_frameBuffer = fbp0;
	sceGuInit();
	gumInit();

	sceGuStart(GU_DIRECT, list);
	sceGuDrawBuffer(displayColorMode, vrelptr(fbp0), PSP_BUF_WIDTH);
	sceGuDispBuffer(PSP_SCR_WIDTH, PSP_SCR_HEIGHT, vrelptr(fbp1), PSP_BUF_WIDTH);
	sceGuDepthBuffer(vrelptr(zbp), PSP_BUF_WIDTH);
	sceGuOffset(2048 - (PSP_SCR_WIDTH / 2), 2048 - (PSP_SCR_HEIGHT / 2));
	sceGuViewport(2048, 2048, PSP_SCR_WIDTH, PSP_SCR_HEIGHT);

	sceGuDepthRange(100, 65535);
	// This function has changed in the new PSP SDK, so we may need to adjust it later
	sceGuScissor(0, 0, PSP_SCR_WIDTH, PSP_SCR_HEIGHT);
	sceGuEnable(GU_SCISSOR_TEST);

	sceGuDepthFunc(GU_LEQUAL);
	sceGuEnable(GU_DEPTH_TEST);
	sceGuFrontFace(GU_CCW);
	sceGuShadeModel(GU_SMOOTH);
	sceGuEnable(GU_CULL_FACE);
	sceGuEnable(GU_TEXTURE_2D);
	sceGuEnable(GU_CLIP_PLANES);

	sceGuAmbient(0xff000000);

	sceGuFinish();
	sceGuSync(0, 0);

	sceDisplayWaitVblankStart();
	sceGuDisplay(GU_TRUE);

	maxLightCount = 4;

	Window::SetResolution(PSP_SCR_WIDTH, PSP_SCR_HEIGHT);

	lastSettings.invertFaces = false;
	lastSettings.renderingMode = MaterialRenderingMode::Opaque;
	lastSettings.useDepth = true;
	lastSettings.useLighting = false;
	lastSettings.useTexture = true;

	return result;
}

void RendererGU::Setup()
{
}

void RendererGU::Stop()
{
	sceGuTerm();
}

void RendererGU::NewFrame()
{
	sceGuStart(GU_DIRECT, list);
	for (int i = 0; i < maxLightCount; i++)
	{
		lastUpdatedLights[i] = nullptr;
	}
	lastUsedColor = 0x00000000;
	lastUsedColor2 = 0xFFFFFFFF;
}

void RendererGU::EndFrame()
{
	SCOPED_PROFILER("RendererGU::EndFrame", scopeBenchmark);

	usedTexture = nullptr;

	/*if (!dialog)
	{*/
	sceGuFinish();
	sceGuSync(0, 0);
	//}

	if (Screen::IsVSyncEnabled())
	{
		sceDisplayWaitVblankStart();
	}

	sceGuSwapBuffers();
}

void RendererGU::SetViewport(int x, int y, int width, int height)
{
	sceGuViewport(x, y, width, height);
}

void RendererGU::SetClearColor(const Color& color)
{
	sceGuClearColor(color.GetUnsignedIntABGR());
}

void RendererGU::SetProjection2D(float projectionSize, float nearClippingPlane, float farClippingPlane)
{
	sceGumMatrixMode(GU_PROJECTION);
	sceGumLoadIdentity();
	const float halfRatio = Graphics::usedCamera->GetAspectRatio() / 2.0f * 10 * (projectionSize / 5.0f);
	const float halfOne = 0.5f * 10 * (projectionSize / 5.0f);
	sceGumOrtho(-halfRatio, halfRatio, -halfOne, halfOne, nearClippingPlane, farClippingPlane);
}

void RendererGU::SetProjection3D(float fov, float nearClippingPlane, float farClippingPlane, float aspect)
{
	sceGumMatrixMode(GU_PROJECTION);
	sceGumLoadIdentity();
	sceGumPerspective(fov, Window::GetAspectRatio(), nearClippingPlane, farClippingPlane);
}

void RendererGU::ResetView()
{
	sceGumMatrixMode(GU_VIEW);
	sceGumLoadIdentity();
	sceGumRotateY(3.14159f);
}

void RendererGU::SetCameraPosition(const Camera& camera)
{
	sceGumMatrixMode(GU_VIEW);
	sceGumLoadMatrix((ScePspFMatrix4*)&camera.m_cameraTransformMatrix);
}

void RendererGU::SetCameraPosition(const Vector3& position, const Vector3& rotation)
{
	sceGumMatrixMode(GU_VIEW);
	sceGumLoadIdentity();

	sceGumRotateZ((-rotation.z) / 180.0f * 3.14159f);
	sceGumRotateX(rotation.x / 180.0f * 3.14159f);
	sceGumRotateY((rotation.y + 180) / 180.0f * 3.14159f);

	ScePspFVector3 v = { position.x, -position.y, -position.z };
	sceGumTranslate(&v);
}

void RendererGU::ResetTransform()
{
	sceGumMatrixMode(GU_MODEL);
	sceGumLoadIdentity();
}

void RendererGU::SetTransform(const Vector3& position, const Vector3& rotation, const Vector3& scale, bool resetTransform)
{
	sceGumMatrixMode(GU_MODEL);
	if (resetTransform)
		sceGumLoadIdentity();

	ScePspFVector3 vt = { -position.x, position.y, position.z };
	sceGumTranslate(&vt);

	sceGumRotateY(-rotation.y * 3.14159265359 / 180.0f);
	sceGumRotateX(rotation.x * 3.14159265359 / 180.0f);
	sceGumRotateZ(-rotation.z * 3.14159265359 / 180.0f);

	ScePspFVector3 vs = { scale.x, scale.y, scale.z };
	sceGumScale(&vs);
}

void RendererGU::SetTransform(const glm::mat4& mat)
{
	sceGuSetMatrix(GU_MODEL, (ScePspFMatrix4*)&mat);
}

void RendererGU::BindTexture(const Texture& texture)
{
}

void RendererGU::ApplyTextureFilters(const Texture& texture)
{
}

void RendererGU::DrawSubMesh(const MeshData::SubMesh& subMesh, const Material& material, RenderingSettings& settings)
{
	DrawSubMesh(subMesh, material, *material.GetTexture(), settings);
}

void RendererGU::DrawSubMesh(const MeshData::SubMesh& subMesh, const Material& material, const Texture& texture, RenderingSettings& settings)
{
	//float material_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };  /* default value */
	//float material_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };  /* default value */
	//float material_specular[] = { 0.0f, 0.0f, 0.0f, 1.0f }; /* NOT default value */
	//float material_emission[] = { 0.0f, 0.0f, 0.0f, 1.0f }; /* default value */
	// glMaterial(GU_DIFFUSE, 0xFFFFFFFF);
	//  glMaterialfv(GU_FRONT, GU_AMBIENT, material_ambient);
	//  glMaterialfv(GU_FRONT, GU_DIFFUSE, material_diffuse);
	//  glMaterialfv(GU_FRONT, GU_SPECULAR, material_specular);
	//  glMaterialfv(GU_FRONT, GU_EMISSION, material_emission);
	//  glMaterialf(GU_FRONT, GU_SHININESS, 10.0);               /* NOT default value   */

	// Apply rendering settings
	if (lastSettings.invertFaces != settings.invertFaces)
	{
		if (settings.invertFaces)
		{
			sceGuFrontFace(GU_CW);
		}
		else
		{
			sceGuFrontFace(GU_CCW);
		}
	}

	if (lastSettings.useDepth != settings.useDepth)
	{
		if (settings.useDepth)
		{
			sceGuEnable(GU_DEPTH_TEST);
		}
		else
		{
			sceGuDisable(GU_DEPTH_TEST);
		}
	}

	if (lastSettings.renderingMode != settings.renderingMode)
	{
		if (settings.renderingMode == MaterialRenderingMode::Opaque)
		{
			sceGuDisable(GU_BLEND);
			sceGuDisable(GU_ALPHA_TEST);
		}
		else if (settings.renderingMode == MaterialRenderingMode::Cutout)
		{
			sceGuDisable(GU_BLEND);
			sceGuEnable(GU_ALPHA_TEST);
			sceGuAlphaFunc(GU_GEQUAL, static_cast<int>(material.GetAlphaCutoff() * 255), 0xff);
		}
		else
		{
			sceGuEnable(GU_BLEND);
			sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
			sceGuDisable(GU_ALPHA_TEST);
		}
	}

	if (lastSettings.useLighting != settings.useLighting)
	{
		if (settings.useLighting)
		{
			sceGuEnable(GU_LIGHTING);
		}
		else
		{
			sceGuDisable(GU_LIGHTING);
		}
	}

	if (lastSettings.useTexture != settings.useTexture)
	{
		sceGuEnable(GU_TEXTURE_2D);
	}

	// glDepthMask needs GL_FALSE here, pspsdk is doing this wrong, may change in a sdk update
	if (settings.renderingMode == MaterialRenderingMode::Transparent || settings.max_depth)
	{
		sceGuDepthMask(GU_TRUE);
	}

	if (lastSettings.max_depth != settings.max_depth)
	{
		if (settings.max_depth)
		{
			sceGuDepthRange(65534, 65535);
		}
		else
		{
			sceGuDepthRange(100, 65535);
		}
	}

	// Keep in memory the used settings
	lastSettings.invertFaces = settings.invertFaces;
	lastSettings.renderingMode = settings.renderingMode;
	lastSettings.useDepth = settings.useDepth;
	lastSettings.useLighting = settings.useLighting;
	lastSettings.useTexture = settings.useTexture;
	lastSettings.max_depth = settings.max_depth;

	if (lastUsedColor != material.GetColor().GetUnsignedIntABGR() || lastUsedColor2 != subMesh.m_meshData->unifiedColor.GetUnsignedIntABGR())
	{
		lastUsedColor = material.GetColor().GetUnsignedIntABGR();
		lastUsedColor2 = subMesh.m_meshData->unifiedColor.GetUnsignedIntABGR();
		sceGuColor((material.GetColor() * subMesh.m_meshData->unifiedColor).GetUnsignedIntABGR());
	}

	// Bind texture
	const TexturePSP& guTexture = dynamic_cast<const TexturePSP&>(texture);
	if (usedTexture != guTexture.data[0])
	{
		usedTexture = guTexture.data[0];
		texture.Bind();
	}
	sceGuTexOffset(material.GetOffset().x, material.GetOffset().y);
	sceGuTexScale(material.GetTiling().x, material.GetTiling().y);

	// Draw
	if (subMesh.m_index_count == 0)
	{
		sceGumDrawArray(GU_TRIANGLES, subMesh.pspDrawParam, subMesh.m_vertice_count, 0, subMesh.m_data);
	}
	else
	{
		sceGumDrawArray(GU_TRIANGLES, subMesh.pspDrawParam, subMesh.m_index_count, subMesh.GetIndices(), subMesh.m_data);
	}
	//Performance::AddDrawCall();


	// glDepthMask needs GL_TRUE here, pspsdk is doing this wrong, may change in a sdk update
	sceGuDepthMask(GU_FALSE);
}

void RendererGU::DrawLine(const Vector3& a, const Vector3& b, const Color& color, RenderingSettings& settings)
{
}

unsigned int RendererGU::CreateNewTexture()
{
	return 0;
}

void RendererGU::DeleteTexture(Texture& texture)
{
}

void RendererGU::SetTextureData(const Texture& texture, unsigned int textureType, const unsigned char* buffer)
{
}

void RendererGU::SetLight(const int lightIndex, const Light& light, const Vector3& lightPosition, const Vector3& lightDirection)
{
	if (lightIndex >= maxLightCount)
	{
		return;
	}

	float intensity = light.m_intensity;
	const Color& color = light.color;
	const LightType& type = light.m_type;
	const RGBA& rgba = color.GetRGBA();

	sceGuEnable(GU_LIGHT0 + lightIndex);

	// Do not reupdate values if this light has been already updated in the same frame
	if (lastUpdatedLights[lightIndex] == &light)
	{
		return;
	}

	lastUpdatedLights[lightIndex] = &light;

	if (intensity > 1)
		intensity = 1;

	float typeIntensity = 1;
	if (type == LightType::Directional)
		typeIntensity = 2;

	const Color fixedColor = Color::CreateFromRGBAFloat(rgba.r * intensity * typeIntensity, rgba.g * intensity * typeIntensity, rgba.b * intensity * typeIntensity, 1);
	//color.SetFromRGBAfloat(rgba.r * intensity, rgba.g * intensity, rgba.b * intensity, 1);
	ScePspFVector3 pos = { -lightPosition.x, lightPosition.y, lightPosition.z };
	ScePspFVector3 rot = { lightDirection.x, lightDirection.y, lightDirection.z };
	if (type == LightType::Directional)
	{
		sceGuLight(lightIndex, GU_POINTLIGHT, GU_AMBIENT_AND_DIFFUSE, &pos);
		sceGuLightColor(lightIndex, GU_AMBIENT, 0x00000000);
		sceGuLightColor(lightIndex, GU_DIFFUSE, fixedColor.GetUnsignedIntABGR());
	}
	else if (type == LightType::Ambient)
	{
		sceGuLight(lightIndex, GU_POINTLIGHT, GU_AMBIENT_AND_DIFFUSE, &pos);
		sceGuLightColor(lightIndex, GU_AMBIENT, fixedColor.GetUnsignedIntABGR());
		sceGuLightColor(lightIndex, GU_DIFFUSE, 0x00000000);
	}
	else if (type == LightType::Spot)
	{
		sceGuLight(lightIndex, GU_SPOTLIGHT, GU_AMBIENT_AND_DIFFUSE, &pos);
		sceGuLightColor(lightIndex, GU_AMBIENT, 0x00000000);
		sceGuLightColor(lightIndex, GU_DIFFUSE, fixedColor.GetUnsignedIntABGR());
		
		sceGuLightSpot(lightIndex, &rot, light.GetSpotSmoothness() * 5, 1 - (light.GetSpotAngle() * light.GetSpotAngle()) / 8100);
	}
	else
	{
		sceGuLight(lightIndex, GU_POINTLIGHT, GU_AMBIENT_AND_DIFFUSE, &pos);
		sceGuLightColor(lightIndex, GU_DIFFUSE, fixedColor.GetUnsignedIntABGR());
		sceGuLightColor(lightIndex, GU_AMBIENT, 0x00000000);
	}
	sceGuLightColor(lightIndex, GU_SPECULAR, 0x00000000);
	
	float quadraticAttenuation = light.GetQuadraticValue();
	float linearAttenuation = light.GetLinearValue();
	float constAttenuation = 1;
	if (type == LightType::Directional || type == LightType::Ambient)
	{
		quadraticAttenuation = 0;
		linearAttenuation = 0;
		constAttenuation = 0;
	}
	sceGuLightAtt(lightIndex, constAttenuation, linearAttenuation, quadraticAttenuation);
}

void RendererGU::DisableAllLight()
{
	for (int lightIndex = 0; lightIndex < maxLightCount; lightIndex++)
	{
		sceGuDisable(GU_LIGHT0 + lightIndex);
	}
}

void RendererGU::Setlights(const LightsIndices& lightsIndices)
{
	DisableAllLight();
	const int lightCount = AssetManager::GetLightCount();

	int usedLightCount = 0;
	static const Vector3 zero = Vector3(0, 0, 0);

	for (int i = 0; i < lightCount; i++)
	{
		const Light* light = AssetManager::GetLight(i);
		if (light->m_type == LightType::Ambient && light->IsEnabled() && light->GetGameObjectRaw()->IsLocalActive())
		{
			SetLight(usedLightCount, *light, zero, zero);
			usedLightCount++;
			if (usedLightCount == maxLightCount)
				break;
		}
	}
	if (usedLightCount != maxLightCount)
	{
		for (size_t i = 0; i < lightsIndices.usedDirectionalLightCount; i++)
		{
			const Light* light = AssetManager::GetLight(lightsIndices.directionalLightIndices[i].x - 1);
			const Vector3 dir = light->GetTransformRaw()->GetBackward() * 1000;
			SetLight(usedLightCount, *light, dir, dir);

			usedLightCount++;
			if (usedLightCount == maxLightCount)
				break;
		}
	}
	if (usedLightCount != maxLightCount)
	{
		for (size_t i = 0; i < lightsIndices.usedPointLightCount; i++)
		{
			const Light* light = AssetManager::GetLight(lightsIndices.pointLightIndices[i].x - 1);
			SetLight(usedLightCount, *light, light->GetTransformRaw()->GetPosition(), zero);
			usedLightCount++;
			if (usedLightCount == maxLightCount)
				break;
		}
	}
	if (usedLightCount != maxLightCount)
	{
		for (size_t i = 0; i < lightsIndices.usedSpotLightCount; i++)
		{
			const Light* light = AssetManager::GetLight(lightsIndices.spotLightIndices[i].x - 1);
			Vector3 fwd = light->GetTransformRaw()->GetForward();
			fwd.x = -fwd.x;
			SetLight(usedLightCount, *light, light->GetTransformRaw()->GetPosition(), fwd);
			usedLightCount++;
			if (usedLightCount == maxLightCount)
				break;
		}
	}
}

const uint8_t* RendererGU::GetFrameBuffer() const
{
	return static_cast<const uint8_t*>(m_frameBuffer);
}

bool RendererGU::UseHighQualityColor() const
{
	return useHighQualityColor;
}

void RendererGU::Clear(ClearMode mode)
{
	if (mode == ClearMode::Color_Depth)
	{
		sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT);
	}
	else if (mode == ClearMode::Color)
	{
		sceGuClear(GU_COLOR_BUFFER_BIT);
	}
	else if (mode == ClearMode::Depth)
	{
		sceGuClear(GU_DEPTH_BUFFER_BIT/*| GU_STENCIL_BUFFER_BIT*/);
	}
}

void RendererGU::SetFog(bool m_active)
{
	if (m_active)
		sceGuEnable(GU_FOG);
	else
		sceGuDisable(GU_FOG);

	if (m_active)
		sceGuFog(fogStart, fogEnd, fogColor.GetUnsignedIntABGR());
}

void RendererGU::SetFogValues(float start, float end, const Color& color)
{
	fogStart = start;
	fogEnd = end;
	fogColor = color;
	sceGuFog(fogStart, fogEnd, fogColor.GetUnsignedIntABGR());
}

void RendererGU::DeleteSubMeshData(MeshData::SubMesh& subMesh)
{
}

void RendererGU::UploadMeshData(MeshData& meshData)
{
}

int RendererGU::GetWrapModeEnum(WrapMode wrapMode)
{
	int mode = GU_REPEAT;
	switch (wrapMode)
	{
	case WrapMode::ClampToEdge:
		mode = GU_CLAMP;
		break;
	case WrapMode::Repeat:
		mode = GU_REPEAT;
		break;
	}
	return mode;
}
#endif