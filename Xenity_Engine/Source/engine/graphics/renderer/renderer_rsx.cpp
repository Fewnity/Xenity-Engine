// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#if defined(__PS3__)
#include "renderer_rsx.h"

#include <engine/graphics/3d_graphics/mesh_data.h>
#include <engine/tools/profiler_benchmark.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/tools/math.h>
#include <engine/debug/performance.h>
#include <engine/debug/debug.h>
#include <engine/graphics/graphics.h>
#include <engine/graphics/material.h>
#include <engine/graphics/shader_rsx.h>
#include <engine/ui/window.h>
#include <engine/graphics/camera.h>
#include <engine/engine.h>
#include <engine/game_elements/gameobject.h>
#include <engine/game_elements/transform.h>
#include <engine/ui/screen.h>

#include <memory>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <io/pad.h> 
#include <sysutil/video.h>
#include <sysutil/sysutil.h>
#include <engine/graphics/texture_ps3.h>
#include <malloc.h>
#include <sysutil/sysutil.h>
#include <sys/process.h>

#include <unistd.h>

#define DEFUALT_CB_SIZE						0x80000		// 512Kb default command buffer size
#define HOST_STATE_CB_SIZE					0x10000		// 64Kb state command buffer size (used for resetting certain default states)
#define HOST_ADDR_ALIGNMENT					(1024*1024)
#define HOSTBUFFER_SIZE				        (128*1024*1024)
#define GCM_LABEL_INDEX		255
#define FRAME_BUFFER_COUNT					2

gcmContextData* RendererRSX::context = nullptr;

uint32_t sLabelVal = 1;
uint32_t running = 0;

uint32_t curr_fb = 0;
uint32_t first_fb = 1;

uint32_t depth_pitch;
uint32_t depth_offset;
uint32_t* depth_buffer = nullptr;

uint32_t color_pitch;
uint32_t color_offset[FRAME_BUFFER_COUNT];
uint32_t* color_buffer[FRAME_BUFFER_COUNT];
f32 aspect_ratio;
videoResolution vResolution;
static uint32_t sResolutionIds[] = {
	//VIDEO_RESOLUTION_1080, // Disable because it's too slow with lighting (too much fragments)
	VIDEO_RESOLUTION_720,
	VIDEO_RESOLUTION_480,
	VIDEO_RESOLUTION_576
};
static size_t RESOLUTION_ID_COUNT = sizeof(sResolutionIds) / sizeof(uint32_t);

// fragment shader
rsxProgramAttrib* RendererRSX::textureUnit = nullptr;


extern "C"
{

	void program_exit_callback()
	{
		gcmSetWaitFlip(RendererRSX::context);
		rsxFinish(RendererRSX::context, 1);
	}

	static void sysutil_exit_callback(u64 status, u64 param, void* usrdata)
	{
		switch (status) {
		case SYSUTIL_EXIT_GAME:
			running = 0;
			Engine::Quit();
			break;
		case SYSUTIL_DRAW_BEGIN:
		case SYSUTIL_DRAW_END:
			break;
		default:
			break;
		}
	}

}

void RendererRSX::setDrawEnv()
{
	rsxSetColorMask(context, GCM_COLOR_MASK_B |
		GCM_COLOR_MASK_G |
		GCM_COLOR_MASK_R |
		GCM_COLOR_MASK_A);

	rsxSetColorMaskMrt(context, 0);

	u16 x, y, w, h;
	f32 min, max;
	f32 scale[4], offset[4];

	x = 0;
	y = 0;
	w = resolution.x;
	h = resolution.y;
	min = 0.0f;
	max = 1.0f;
	scale[0] = w * 0.5f;
	scale[1] = h * -0.5f;
	scale[2] = (max - min) * 0.5f;
	scale[3] = 0.0f;
	offset[0] = x + w * 0.5f;
	offset[1] = y + h * 0.5f;
	offset[2] = (max + min) * 0.5f;
	offset[3] = 0.0f;

	rsxSetViewport(context, x, y, w, h, min, max, scale, offset);
	rsxSetScissor(context, x, y, w, h);

	rsxSetDepthTestEnable(context, GCM_TRUE);
	rsxSetDepthFunc(context, GCM_LESS);
	rsxSetShadeModel(context, GCM_SHADE_MODEL_SMOOTH);
	rsxSetDepthWriteEnable(context, 1);

	rsxSetFrontFace(context, GCM_FRONTFACE_CW);
	rsxSetCullFaceEnable(context, GCM_TRUE);
	rsxSetCullFace(context, GCM_CULL_FRONT);
}

void RendererRSX::drawFrame()
{
	setDrawEnv();
	rsxSetClearColor(context, clearColor.GetUnsignedIntARGB());
	rsxSetClearDepthStencil(context, 0xffffff00);
	rsxClearSurface(context, GCM_CLEAR_R |
		GCM_CLEAR_G |
		GCM_CLEAR_B |
		GCM_CLEAR_A |
		GCM_CLEAR_S |
		GCM_CLEAR_Z);

	rsxSetZMinMaxControl(context, GCM_FALSE, GCM_TRUE, GCM_FALSE);

	for (int i = 0; i < 8; i++)
		rsxSetViewportClip(context, i, resolution.x, resolution.y);
}

void RendererRSX::waitFinish()
{
	rsxSetWriteBackendLabel(context, GCM_LABEL_INDEX, sLabelVal);

	rsxFlushBuffer(context);

	// vu32 = volatile uint32_t
	while (*(vu32*)gcmGetLabelAddress(GCM_LABEL_INDEX) != sLabelVal)
		usleep(30);

	++sLabelVal;
}

void RendererRSX::waitRSXIdle()
{
	rsxSetWriteBackendLabel(context, GCM_LABEL_INDEX, sLabelVal);
	rsxSetWaitLabel(context, GCM_LABEL_INDEX, sLabelVal);

	++sLabelVal;

	waitFinish();
}

void RendererRSX::initVideoConfiguration()
{
	s32 rval = 0;
	s32 resId = 0;

	for (size_t i = 0; i < RESOLUTION_ID_COUNT; i++)
	{
		rval = videoGetResolutionAvailability(VIDEO_PRIMARY, sResolutionIds[i], VIDEO_ASPECT_AUTO, 0);
		if (rval != 1)
			continue;

		resId = sResolutionIds[i];

		rval = videoGetResolution(resId, &vResolution);

		if (!rval)
			break;
	}

	if (rval)
	{
		Debug::Print("RSX: videoGetResolutionAvailability failed. No usable resolution.");

		// Try to force a resolution
		resId = 1;
		rval = videoGetResolution(resId, &vResolution);
	}


	videoConfiguration config = {
		(u8)resId,
		VIDEO_BUFFER_FORMAT_XRGB,
		VIDEO_ASPECT_AUTO,
		{0, 0, 0, 0, 0, 0, 0, 0, 0},
		(uint32_t)vResolution.width * 4
	};

	rval = videoConfigure(VIDEO_PRIMARY, &config, NULL, 0);
	if (rval)
	{
		Debug::Print("RSX: videoConfigure failed.");
		exit(1);
	}

	videoState state;

	rval = videoGetState(VIDEO_PRIMARY, 0, &state);

	// Better to replace this by aspect_ratio = vResolution.width/vResolution.height; ?
	switch (state.displayMode.aspect) {
	case VIDEO_ASPECT_4_3:
		aspect_ratio = 4.0f / 3.0f;
		break;
	case VIDEO_ASPECT_16_9:
		aspect_ratio = 16.0f / 9.0f;
		break;
	default:
		printf("unknown aspect ratio %x\n", state.displayMode.aspect);
		aspect_ratio = 16.0f / 9.0f;
		break;
	}

	resolution.x = vResolution.width;
	resolution.y = vResolution.height;
}

void RendererRSX::setRenderTarget(uint32_t index)
{
	gcmSurface sf;

	sf.colorFormat = GCM_SURFACE_X8R8G8B8;
	sf.colorTarget = GCM_SURFACE_TARGET_0;
	sf.colorLocation[0] = GCM_LOCATION_RSX;
	sf.colorOffset[0] = color_offset[index];
	sf.colorPitch[0] = color_pitch;

	sf.colorLocation[1] = GCM_LOCATION_RSX;
	sf.colorLocation[2] = GCM_LOCATION_RSX;
	sf.colorLocation[3] = GCM_LOCATION_RSX;
	sf.colorOffset[1] = 0;
	sf.colorOffset[2] = 0;
	sf.colorOffset[3] = 0;
	sf.colorPitch[1] = 64;
	sf.colorPitch[2] = 64;
	sf.colorPitch[3] = 64;

	sf.depthFormat = GCM_SURFACE_ZETA_Z24S8;
	sf.depthLocation = GCM_LOCATION_RSX;
	sf.depthOffset = depth_offset;
	sf.depthPitch = depth_pitch;

	sf.type = GCM_SURFACE_TYPE_LINEAR;
	sf.antiAlias = GCM_SURFACE_CENTER_1;

	sf.width = resolution.x;
	sf.height = resolution.y;
	sf.x = 0;
	sf.y = 0;

	rsxSetSurface(context, &sf);
}

void RendererRSX::init_screen(void* host_addr, uint32_t size)
{
	uint32_t zs_depth = 4;
	uint32_t color_depth = 4;

	rsxInit(&context, DEFUALT_CB_SIZE, size, host_addr);

	initVideoConfiguration();

	waitRSXIdle();

	gcmSetFlipMode(GCM_FLIP_VSYNC);

	color_pitch = resolution.x * color_depth;
	depth_pitch = resolution.x * zs_depth;

	for (uint32_t i = 0; i < FRAME_BUFFER_COUNT; i++)
	{
		color_buffer[i] = (uint32_t*)rsxMemalign(64, (resolution.y * color_pitch));
		rsxAddressToOffset(color_buffer[i], &color_offset[i]);
		gcmSetDisplayBuffer(i, color_offset[i], color_pitch, resolution.x, resolution.y);
	}

	depth_buffer = (uint32_t*)rsxMemalign(64, resolution.y * depth_pitch);
	rsxAddressToOffset(depth_buffer, &depth_offset);

	//debugFontRenderer = new RSXDebugFontRenderer(context);
}

void RendererRSX::waitflip()
{
	while (gcmGetFlipStatus() != 0)
		usleep(200);

	gcmResetFlipStatus();
}

void RendererRSX::flip()
{
	if (!first_fb)
		waitflip();
	else
		gcmResetFlipStatus();

	gcmSetFlip(context, curr_fb);
	rsxFlushBuffer(context);

	gcmSetWaitFlip(context);

	curr_fb ^= 1;
	setRenderTarget(curr_fb);

	first_fb = 0;
}

RendererRSX::RendererRSX()
{
}

int RendererRSX::Init()
{
	int result = 0;

	//maxLightCount = 4;
	void* host_addr = memalign(HOST_ADDR_ALIGNMENT, HOSTBUFFER_SIZE);
	init_screen(host_addr, HOSTBUFFER_SIZE);

	Window::SetResolution(resolution.x, resolution.y);

	atexit(program_exit_callback);
	sysUtilRegisterCallback(0, sysutil_exit_callback, NULL);

	setDrawEnv();
	setRenderTarget(curr_fb);

	return result;
}

void RendererRSX::Setup()
{
	lastSettings.invertFaces = false;
	lastSettings.renderingMode = MaterialRenderingModes::Opaque;
	lastSettings.useDepth = true;
	lastSettings.useLighting = false;
	lastSettings.useTexture = true;
	lastSettings.max_depth = false;
}

void RendererRSX::Stop()
{
	//sceGuTerm();
}

void RendererRSX::NewFrame()
{
	sysUtilCheckCallback();
	drawFrame();

	// return;

	// for (int i = 0; i < maxLightCount; i++)
	// {
	// 	lastUpdatedLights[i] = nullptr;
	// }
	lastUsedColor = 0x00000000;
	lastUsedColor2 = 0xFFFFFFFF;
}

void RendererRSX::EndFrame()
{
	SCOPED_PROFILER("RendererRSX::EndFrame", scopeBenchmark);

	usedTexture = nullptr;

	if (Screen::IsVSyncEnabled())
	{
		//sceDisplayWaitVblankStart();
	}
	flip();
}

void RendererRSX::SetViewport(int x, int y, int width, int height)
{
	//sceGuViewport(x, y, width, height);
}

void RendererRSX::SetClearColor(const Color& color)
{
	clearColor = color;
}

void RendererRSX::SetProjection2D(float projectionSize, float nearClippingPlane, float farClippingPlane)
{
	// sceGumMatrixMode(GU_PROJECTION);
	// sceGumLoadIdentity();
	// const float halfRatio = Graphics::usedCamera->GetAspectRatio() / 2.0f * 10 * (projectionSize / 5.0f);
	// const float halfOne = 0.5f * 10 * (projectionSize / 5.0f);
	// sceGumOrtho(-halfRatio, halfRatio, -halfOne, halfOne, nearClippingPlane, farClippingPlane);
}

void RendererRSX::SetProjection3D(float fov, float nearClippingPlane, float farClippingPlane, float aspect)
{
}

void RendererRSX::ResetView()
{
	// sceGumMatrixMode(GU_VIEW);
	// sceGumLoadIdentity();
	// sceGumRotateY(3.14159f);
}

void RendererRSX::SetCameraPosition(const Camera& camera)
{
}

void RendererRSX::SetCameraPosition(const Vector3& position, const Vector3& rotation)
{
	// sceGumMatrixMode(GU_VIEW);
	// sceGumLoadIdentity();

	// sceGumRotateZ((-rotation.z) / 180.0f * 3.14159f);
	// sceGumRotateX(rotation.x / 180.0f * 3.14159f);
	// sceGumRotateY((rotation.y + 180) / 180.0f * 3.14159f);

	// ScePspFVector3 v = { position.x, -position.y, -position.z };
	// sceGumTranslate(&v);
}

void RendererRSX::ResetTransform()
{
	// sceGumMatrixMode(GU_MODEL);
	// sceGumLoadIdentity();
}

void RendererRSX::SetTransform(const Vector3& position, const Vector3& rotation, const Vector3& scale, bool resetTransform)
{
	// sceGumMatrixMode(GU_MODEL);
	// if (resetTransform)
	// 	sceGumLoadIdentity();

	// ScePspFVector3 vt = { -position.x, position.y, position.z };
	// sceGumTranslate(&vt);

	// sceGumRotateY(-rotation.y * 3.14159265359 / 180.0f);
	// sceGumRotateX(rotation.x * 3.14159265359 / 180.0f);
	// sceGumRotateZ(-rotation.z * 3.14159265359 / 180.0f);

	// ScePspFVector3 vs = { scale.x, scale.y, scale.z };
	// sceGumScale(&vs);
}

void RendererRSX::SetTransform(const glm::mat4& mat)
{
}

void RendererRSX::BindTexture(const Texture& texture)
{
}

void RendererRSX::ApplyTextureFilters(const Texture& texture)
{
}

void RendererRSX::DrawSubMesh(const MeshData::SubMesh& subMesh, const Material& material, RenderingSettings& settings)
{
	DrawSubMesh(subMesh, material, *material.GetTexture(), settings);
}

void RendererRSX::DrawSubMesh(const MeshData::SubMesh& subMesh, const Material& material, const Texture& texture, RenderingSettings& settings)
{
	ShaderRSX& rsxShader = dynamic_cast<ShaderRSX&>(*Graphics::s_currentShader);

	uint32_t offset= 0;
	f32 globalAmbientColor[3] = { 0.8f, 0.7f, 0.7f };

	if (lastSettings.useDepth != settings.useDepth)
	{
		if (settings.useDepth)
		{
			rsxSetDepthTestEnable(context, GCM_TRUE);
		}
		else
		{
			rsxSetDepthTestEnable(context, GCM_FALSE);
		}
	}

	if (lastSettings.renderingMode != settings.renderingMode)
	{
		if (settings.renderingMode == MaterialRenderingModes::Opaque)
		{
			rsxSetBlendEnable(context, GCM_FALSE);
			rsxSetAlphaTestEnable(context, GCM_FALSE);
		}
		else if (settings.renderingMode == MaterialRenderingModes::Cutout)
		{
			rsxSetBlendEnable(context, GCM_FALSE);
			rsxSetAlphaTestEnable(context, GCM_TRUE);
			rsxSetAlphaFunc(context, GCM_GEQUAL, static_cast<int>(material.GetAlphaCutoff() * 255));
		}
		else
		{
			rsxSetBlendEnable(context, GCM_TRUE);
			rsxSetBlendFunc(context, GCM_SRC_ALPHA, GCM_ONE_MINUS_SRC_ALPHA, GCM_SRC_ALPHA, GCM_ONE_MINUS_SRC_ALPHA);
			rsxSetBlendEquation(context, GCM_FUNC_ADD, GCM_FUNC_ADD);
		}
	}

	if (settings.renderingMode == MaterialRenderingModes::Transparent || settings.max_depth)
	{
		rsxSetDepthWriteEnable(context, GCM_FALSE);
	}

	if (lastSettings.max_depth != settings.max_depth)
	{
		if (settings.max_depth)
		{
			rsxSetDepthBounds(context, 0.9999f, 1);
			rsxSetDepthBoundsTestEnable(context, GCM_TRUE);
		}
		else
		{
			rsxSetDepthBounds(context, 0, 1);
			rsxSetDepthBoundsTestEnable(context, GCM_FALSE);
		}
	}

	//Keep in memory the used settings
	lastSettings.invertFaces = settings.invertFaces;
	lastSettings.renderingMode = settings.renderingMode;
	lastSettings.useDepth = settings.useDepth;
	lastSettings.useLighting = settings.useLighting;
	lastSettings.useTexture = settings.useTexture;
	lastSettings.max_depth = settings.max_depth;

	const TexturePS3& ps3Texture = dynamic_cast<const TexturePS3&>(texture);
	if (usedTexture != ps3Texture.m_ps3buffer)
	{
		usedTexture = ps3Texture.m_ps3buffer;
		texture.Bind();
	}

	// Set vertex array attributes
	{
		if((int)subMesh.meshData->GetVertexDescriptor() & (int)VertexElements::NORMAL_32_BITS)
		{
			rsxAddressToOffset(&((VertexNormalsNoColor*)subMesh.data)[0].normX, &offset);
			rsxBindVertexArrayAttrib(context, GCM_VERTEX_ATTRIB_NORMAL, 0, offset, sizeof(VertexNormalsNoColor), 3, GCM_VERTEX_DATA_TYPE_F32, GCM_LOCATION_RSX);

			rsxAddressToOffset(&((VertexNormalsNoColor*)subMesh.data)[0].u, &offset);
			rsxBindVertexArrayAttrib(context, GCM_VERTEX_ATTRIB_TEX0, 0, offset, sizeof(VertexNormalsNoColor), 2, GCM_VERTEX_DATA_TYPE_F32, GCM_LOCATION_RSX);

			rsxAddressToOffset(&((VertexNormalsNoColor*)subMesh.data)[0].x, &offset);
			rsxBindVertexArrayAttrib(context, GCM_VERTEX_ATTRIB_POS, 0, offset, sizeof(VertexNormalsNoColor), 3, GCM_VERTEX_DATA_TYPE_F32, GCM_LOCATION_RSX);
		}
		else
		{
			rsxAddressToOffset(&((VertexNoColor*)subMesh.data)[0].u, &offset);
			rsxBindVertexArrayAttrib(context, GCM_VERTEX_ATTRIB_TEX0, 0, offset, sizeof(VertexNoColor), 2, GCM_VERTEX_DATA_TYPE_F32, GCM_LOCATION_RSX);

			rsxAddressToOffset(&((VertexNoColor*)subMesh.data)[0].x, &offset);
			rsxBindVertexArrayAttrib(context, GCM_VERTEX_ATTRIB_POS, 0, offset, sizeof(VertexNoColor), 3, GCM_VERTEX_DATA_TYPE_F32, GCM_LOCATION_RSX);
		}
	}

	if (lastUsedColor != material.GetColor().GetUnsignedIntRGBA() || lastUsedColor2 != subMesh.meshData->unifiedColor.GetUnsignedIntRGBA() || (!Graphics::s_UseOpenGLFixedFunctions && lastShaderIdUsedColor != material.GetShader()->m_fileId))
	{
		lastUsedColor = material.GetColor().GetUnsignedIntRGBA();
		lastUsedColor2 = subMesh.meshData->unifiedColor.GetUnsignedIntRGBA();
		const Vector4 colorMix = (material.GetColor() * subMesh.meshData->unifiedColor).GetRGBA().ToVector4();

		lastShaderIdUsedColor = material.GetShader()->m_fileId;
		rsxSetFragmentProgramParameter(context, rsxShader.m_fragmentProgram, rsxShader.m_color, (float*)&colorMix.x, rsxShader.m_fp_offset, GCM_LOCATION_RSX);
	}

	// While rsxSetUpdateFragmentProgramParameter is missing, we have to set the fragment shader to apply rsxSetFragmentProgramParameter calls
	rsxShader.Use();

	rsxSetUserClipPlaneControl(context, GCM_USER_CLIP_PLANE_DISABLE,
		GCM_USER_CLIP_PLANE_DISABLE,
		GCM_USER_CLIP_PLANE_DISABLE,
		GCM_USER_CLIP_PLANE_DISABLE,
		GCM_USER_CLIP_PLANE_DISABLE,
		GCM_USER_CLIP_PLANE_DISABLE);

	rsxAddressToOffset(&subMesh.indices[0], &offset);
	rsxInvalidateVertexCache(context);
	rsxDrawIndexArray(context, GCM_TYPE_TRIANGLES, offset, subMesh.index_count, GCM_INDEX_TYPE_16B, GCM_LOCATION_RSX);
	rsxSetDepthWriteEnable(context, GCM_TRUE);
}

void RendererRSX::DrawLine(const Vector3& a, const Vector3& b, const Color& color, RenderingSettings& settings)
{
}

unsigned int RendererRSX::CreateNewTexture()
{
	return 0;
}

void RendererRSX::DeleteTexture(Texture& texture)
{
}

void RendererRSX::SetTextureData(const Texture& texture, unsigned int textureType, const unsigned char* buffer)
{
}

void RendererRSX::SetLight(const int lightIndex, const Light& light, const Vector3& lightPosition, const Vector3& lightDirection)
{
	// if (lightIndex >= maxLightCount)
	// 	return;

	// float intensity = light.m_intensity;
	// const Color& color = light.color;
	// const LightType& type = light.m_type;
	// const RGBA& rgba = color.GetRGBA();

	// sceGuEnable(GU_LIGHT0 + lightIndex);

	// if (lastUpdatedLights[lightIndex] == &light)
	// 	return;

	// lastUpdatedLights[lightIndex] = &light;

	// if (intensity > 1)
	// 	intensity = 1;

	// float typeIntensity = 1;
	// if (type == LightType::Directional)
	// 	typeIntensity = 2;

	// const Color fixedColor = Color::CreateFromRGBAFloat(rgba.r * intensity * typeIntensity, rgba.g * intensity * typeIntensity, rgba.b * intensity * typeIntensity, 1);
	// //color.SetFromRGBAfloat(rgba.r * intensity, rgba.g * intensity, rgba.b * intensity, 1);
	// ScePspFVector3 pos = { -lightPosition.x, lightPosition.y, lightPosition.z };
	// ScePspFVector3 rot = { lightDirection.x, lightDirection.y, lightDirection.z };
	// if (type == LightType::Directional)
	// {
	// 	sceGuLight(lightIndex, GU_POINTLIGHT, GU_AMBIENT_AND_DIFFUSE, &pos);
	// 	sceGuLightColor(lightIndex, GU_AMBIENT, 0x00000000);
	// 	sceGuLightColor(lightIndex, GU_DIFFUSE, fixedColor.GetUnsignedIntABGR());
	// }
	// else if (type == LightType::Ambient)
	// {
	// 	sceGuLight(lightIndex, GU_POINTLIGHT, GU_AMBIENT_AND_DIFFUSE, &pos);
	// 	sceGuLightColor(lightIndex, GU_AMBIENT, fixedColor.GetUnsignedIntABGR());
	// 	sceGuLightColor(lightIndex, GU_DIFFUSE, 0x00000000);
	// }
	// else if (type == LightType::Spot)
	// {
	// 	sceGuLight(lightIndex, GU_SPOTLIGHT, GU_AMBIENT_AND_DIFFUSE, &pos);
	// 	sceGuLightColor(lightIndex, GU_AMBIENT, 0x00000000);
	// 	sceGuLightColor(lightIndex, GU_DIFFUSE, fixedColor.GetUnsignedIntABGR());

	// 	sceGuLightSpot(lightIndex, &rot, light.GetSpotSmoothness() * 5, 1 - (light.GetSpotAngle() * light.GetSpotAngle()) / 8100);
	// }
	// else
	// {
	// 	sceGuLight(lightIndex, GU_POINTLIGHT, GU_AMBIENT_AND_DIFFUSE, &pos);
	// 	sceGuLightColor(lightIndex, GU_DIFFUSE, fixedColor.GetUnsignedIntABGR());
	// 	sceGuLightColor(lightIndex, GU_AMBIENT, 0x00000000);
	// }
	// sceGuLightColor(lightIndex, GU_SPECULAR, 0x00000000);

	// float quadraticAttenuation = light.GetQuadraticValue();
	// float linearAttenuation = light.GetLinearValue();
	// float constAttenuation = 1;
	// if (type == LightType::Directional || type == LightType::Ambient)
	// {
	// 	quadraticAttenuation = 0;
	// 	linearAttenuation = 0;
	// 	constAttenuation = 0;
	// }
	// sceGuLightAtt(lightIndex, constAttenuation, linearAttenuation, quadraticAttenuation);
}

void RendererRSX::DisableAllLight()
{
	// for (int lightIndex = 0; lightIndex < maxLightCount; lightIndex++)
	// {
	// 	sceGuDisable(GU_LIGHT0 + lightIndex);
	// }
}

void RendererRSX::Setlights(const LightsIndices& lightsIndices)
{
	// DisableAllLight();
	// const int lightCount = AssetManager::GetLightCount();

	// int usedLightCount = 0;
	// static const Vector3 zero = Vector3(0, 0, 0);

	// for (int i = 0; i < lightCount; i++)
	// {
	// 	const Light* light = AssetManager::GetLight(i);
	// 	if (light->m_type == LightType::Ambient && light->IsEnabled() && light->GetGameObjectRaw()->IsLocalActive())
	// 	{
	// 		SetLight(usedLightCount, *light, zero, zero);
	// 		usedLightCount++;
	// 		if (usedLightCount == maxLightCount)
	// 			break;
	// 	}
	// }
	// if (usedLightCount != maxLightCount)
	// {
	// 	for (size_t i = 0; i < lightsIndices.usedDirectionalLightCount; i++)
	// 	{
	// 		const Light* light = AssetManager::GetLight(lightsIndices.directionalLightIndices[i].x - 1);
	// 		const Vector3 dir = light->GetTransformRaw()->GetBackward() * 1000;
	// 		SetLight(usedLightCount, *light, dir, dir);

	// 		usedLightCount++;
	// 		if (usedLightCount == maxLightCount)
	// 			break;
	// 	}
	// }
	// if (usedLightCount != maxLightCount)
	// {
	// 	for (size_t i = 0; i < lightsIndices.usedPointLightCount; i++)
	// 	{
	// 		const Light* light = AssetManager::GetLight(lightsIndices.pointLightIndices[i].x - 1);
	// 		SetLight(usedLightCount, *light, light->GetTransformRaw()->GetPosition(), zero);
	// 		usedLightCount++;
	// 		if (usedLightCount == maxLightCount)
	// 			break;
	// 	}
	// }
	// if (usedLightCount != maxLightCount)
	// {
	// 	for (size_t i = 0; i < lightsIndices.usedSpotLightCount; i++)
	// 	{
	// 		const Light* light = AssetManager::GetLight(lightsIndices.spotLightIndices[i].x - 1);
	// 		Vector3 fwd = light->GetTransformRaw()->GetForward();
	// 		fwd.x = -fwd.x;
	// 		SetLight(usedLightCount, *light, light->GetTransformRaw()->GetPosition(), fwd);
	// 		usedLightCount++;
	// 		if (usedLightCount == maxLightCount)
	// 			break;
	// 	}
	// }
}

void RendererRSX::Clear()
{
	// sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT /*| GU_STENCIL_BUFFER_BIT*/);
}

void RendererRSX::SetFog(bool m_active)
{
	// if (m_active)
	// 	sceGuEnable(GU_FOG);
	// else
	// 	sceGuDisable(GU_FOG);

	// if (m_active)
	// 	sceGuFog(fogStart, fogEnd, fogColor.GetUnsignedIntABGR());
}

void RendererRSX::SetFogValues(float start, float end, const Color& color)
{
	// fogStart = start;
	// fogEnd = end;
	// fogColor = color;
	// sceGuFog(fogStart, fogEnd, fogColor.GetUnsignedIntABGR());
}

void RendererRSX::DeleteSubMeshData(MeshData::SubMesh& subMesh)
{
}

void RendererRSX::UploadMeshData(MeshData& meshData)
{
}

int RendererRSX::GetWrapModeEnum(WrapMode wrapMode)
{
	int mode = 0;
	switch (wrapMode)
	{
	case WrapMode::ClampToEdge:
	case WrapMode::ClampToBorder:
		mode = GCM_TEXTURE_CLAMP_TO_EDGE;
		break;
	case WrapMode::Repeat:
		mode = GCM_TEXTURE_REPEAT;
		break;
	}
	return mode;
}
#endif