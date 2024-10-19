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
#include <engine/ui/window.h>
#include <engine/graphics/camera.h>
#include <engine/game_elements/gameobject.h>
#include <engine/game_elements/transform.h>
#include <engine/ui/screen.h>

#include <memory>
#include <glm/gtc/type_ptr.hpp>
#include <rsx/rsx.h>
#include <io/pad.h> 
#include <sysutil/video.h>
#include <rsxdebugfontrenderer.h>
#include <sysutil/sysutil.h>

#include "diffuse_specular_shader_vpo.h"
#include "diffuse_specular_shader_fpo.h"

#define DEFUALT_CB_SIZE						0x80000		// 512Kb default command buffer size
#define HOST_STATE_CB_SIZE					0x10000		// 64Kb state command buffer size (used for resetting certain default states)
#define HOST_ADDR_ALIGNMENT					(1024*1024)
#define HOSTBUFFER_SIZE				        (128*1024*1024)
#define GCM_LABEL_INDEX		255
#define FRAME_BUFFER_COUNT					2

uint32_t sLabelVal = 1;
uint32_t running = 0;
RSXDebugFontRenderer* debugFontRenderer = nullptr;
gcmContextData* context = nullptr;

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
	VIDEO_RESOLUTION_1080,
	VIDEO_RESOLUTION_720,
	VIDEO_RESOLUTION_480,
	VIDEO_RESOLUTION_576
};
static size_t RESOLUTION_ID_COUNT = sizeof(sResolutionIds) / sizeof(uint32_t);

u32 vp_offset;

u32 fp_offset;
u32* fp_buffer;

u32* texture_buffer;
u32 texture_offset;

// vertex shader
rsxProgramConst* projMatrix;
rsxProgramConst* mvMatrix;

// fragment shader
rsxProgramAttrib* textureUnit;
rsxProgramConst* eyePosition;
rsxProgramConst* globalAmbient;
rsxProgramConst* litPosition;
rsxProgramConst* litColor;
rsxProgramConst* Kd;
rsxProgramConst* Ks;
rsxProgramConst* spec;

glm::vec3 eye_pos = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 eye_dir = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 up_vec = glm::vec3(0.0f, 1.0f, 0.0f);

void* vp_ucode = NULL;
rsxVertexProgram* vpo = (rsxVertexProgram*)diffuse_specular_shader_vpo;

void* fp_ucode = NULL;
rsxFragmentProgram* fpo = (rsxFragmentProgram*)diffuse_specular_shader_fpo;

glm::mat4 transformationMatrix = glm::mat4(1);
glm::mat4 projectionMatrix;

extern "C"
{

	static void program_exit_callback()
	{
		gcmSetWaitFlip(context);
		rsxFinish(context, 1);
	}

	static void sysutil_exit_callback(u64 status, u64 param, void* usrdata)
	{
		switch (status) {
		case SYSUTIL_EXIT_GAME:
			running = 0;
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

void init_shader()
{
	uint32_t fpsize = 0;
	uint32_t vpsize = 0;

	rsxVertexProgramGetUCode(vpo, &vp_ucode, &vpsize);
	printf("vpsize: %d\n", vpsize);

	projMatrix = rsxVertexProgramGetConst(vpo, "projMatrix");
	mvMatrix = rsxVertexProgramGetConst(vpo, "modelViewMatrix");

	rsxFragmentProgramGetUCode(fpo, &fp_ucode, &fpsize);
	printf("fpsize: %d\n", fpsize);

	fp_buffer = (uint32_t*)rsxMemalign(64, fpsize);
	memcpy(fp_buffer, fp_ucode, fpsize);
	rsxAddressToOffset(fp_buffer, &fp_offset);

	textureUnit = rsxFragmentProgramGetAttrib(fpo, "texture");
	eyePosition = rsxFragmentProgramGetConst(fpo, "eyePosition");
	globalAmbient = rsxFragmentProgramGetConst(fpo, "globalAmbient");
	litPosition = rsxFragmentProgramGetConst(fpo, "lightPosition");
	litColor = rsxFragmentProgramGetConst(fpo, "lightColor");
	spec = rsxFragmentProgramGetConst(fpo, "shininess");
	Ks = rsxFragmentProgramGetConst(fpo, "Ks");
	Kd = rsxFragmentProgramGetConst(fpo, "Kd");
}

void RendererRSX::drawFrame()
{
	uint32_t color = 0;
	setDrawEnv();

	rsxSetClearColor(context, color);
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

	debugFontRenderer = new RSXDebugFontRenderer(context);
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
	init_shader();

	Window::SetResolution(resolution.x, resolution.y);

	DebugFont::init();
	DebugFont::setScreenRes(resolution.x, resolution.y);

	atexit(program_exit_callback);
	sysUtilRegisterCallback(0, sysutil_exit_callback, NULL);

	projectionMatrix = glm::transpose(glm::perspective(glm::radians(45.0f), aspect_ratio, 1.0f, 3000.0f));
	setDrawEnv();
	setRenderTarget(curr_fb);

	return result;
}

void RendererRSX::Setup()
{
}

void RendererRSX::Stop()
{
	//sceGuTerm();
}

void RendererRSX::NewFrame()
{
	sysUtilCheckCallback();
	drawFrame();

	return;

	for (int i = 0; i < maxLightCount; i++)
	{
		lastUpdatedLights[i] = nullptr;
	}
	lastUsedColor = 0x00000000;
	lastUsedColor2 = 0xFFFFFFFF;
}

void RendererRSX::EndFrame()
{
	usedTexture = nullptr;

	// DebugFont::setPosition(10, 10);
	// DebugFont::setColor(1.0f, 0.0f, 0.0f, 1.0f);
	//DebugFont::print("");

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
	//sceGuClearColor(color.GetUnsignedIntABGR());
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
	//projectionMatrix = glm::perspective(glm::radians(fov), aspect, nearClippingPlane, farClippingPlane);
	// projectionMatrix = glm::transpose(glm::perspective(glm::radians(fov), aspect, nearClippingPlane, farClippingPlane));
	projectionMatrix = glm::transpose(glm::perspective(glm::radians(fov), aspect, nearClippingPlane, farClippingPlane));
	//projectionMatrix = glm::transpose(glm::perspective(glm::radians(fov), aspect, nearClippingPlane, farClippingPlane));
	// sceGumMatrixMode(GU_PROJECTION);
	// sceGumLoadIdentity();
	// sceGumPerspective(fov, Window::GetAspectRatio(), nearClippingPlane, farClippingPlane);
}

void RendererRSX::ResetView()
{
	// sceGumMatrixMode(GU_VIEW);
	// sceGumLoadIdentity();
	// sceGumRotateY(3.14159f);
}

Vector3 camPos;
glm::mat4 cameraViewMatrix;

void RendererRSX::SetCameraPosition(const Camera& camera)
{
	cameraViewMatrix = camera.m_cameraTransformMatrix;
	// sceGumMatrixMode(GU_VIEW);
	// sceGumLoadMatrix((ScePspFMatrix4*)&camera.m_cameraTransformMatrix);
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
	transformationMatrix = mat;
	// sceGuSetMatrix(GU_MODEL, (ScePspFMatrix4*)&mat);
}

void RendererRSX::BindTexture(const Texture& texture)
{
	if (!texture.m_ps3buffer)
	{
		return;
	}

	uint32_t offset;
	rsxAddressToOffset(texture.m_ps3buffer, &offset);

	rsxInvalidateTextureCache(context, GCM_INVALIDATE_TEXTURE);

	uint32_t pitch = (texture.GetWidth() * 4);
	gcmTexture gcmTexture;

	gcmTexture.format = (GCM_TEXTURE_FORMAT_A8R8G8B8 | GCM_TEXTURE_FORMAT_LIN);
	gcmTexture.mipmap = 1;
	gcmTexture.dimension = GCM_TEXTURE_DIMS_2D;
	gcmTexture.cubemap = GCM_FALSE;
	gcmTexture.remap = ((GCM_TEXTURE_REMAP_TYPE_REMAP << GCM_TEXTURE_REMAP_TYPE_B_SHIFT) |
		(GCM_TEXTURE_REMAP_TYPE_REMAP << GCM_TEXTURE_REMAP_TYPE_G_SHIFT) |
		(GCM_TEXTURE_REMAP_TYPE_REMAP << GCM_TEXTURE_REMAP_TYPE_R_SHIFT) |
		(GCM_TEXTURE_REMAP_TYPE_REMAP << GCM_TEXTURE_REMAP_TYPE_A_SHIFT) |
		(GCM_TEXTURE_REMAP_COLOR_B << GCM_TEXTURE_REMAP_COLOR_B_SHIFT) |
		(GCM_TEXTURE_REMAP_COLOR_G << GCM_TEXTURE_REMAP_COLOR_G_SHIFT) |
		(GCM_TEXTURE_REMAP_COLOR_R << GCM_TEXTURE_REMAP_COLOR_R_SHIFT) |
		(GCM_TEXTURE_REMAP_COLOR_A << GCM_TEXTURE_REMAP_COLOR_A_SHIFT));
	gcmTexture.width = texture.GetWidth();
	gcmTexture.height = texture.GetHeight();
	gcmTexture.depth = 1;
	gcmTexture.location = GCM_LOCATION_RSX;
	gcmTexture.pitch = pitch;
	gcmTexture.offset = offset;
	rsxLoadTexture(context, textureUnit->index, &gcmTexture);
	rsxTextureControl(context, textureUnit->index, GCM_TRUE, 0 << 8, 12 << 8, GCM_TEXTURE_MAX_ANISO_1);
	int minFilterValue = GCM_TEXTURE_LINEAR;
	int magfilterValue = GCM_TEXTURE_LINEAR;
	if (texture.GetFilter() == Filter::Point)
	{
		minFilterValue = GCM_TEXTURE_NEAREST;
		magfilterValue = GCM_TEXTURE_NEAREST;
	}
	rsxTextureFilter(context, textureUnit->index, 0, minFilterValue, magfilterValue, GCM_TEXTURE_CONVOLUTION_QUINCUNX);
	const int wrap = GetWrapModeEnum(texture.GetWrapMode());

	rsxTextureWrapMode(context, textureUnit->index, wrap, wrap, wrap, 0, GCM_TEXTURE_ZFUNC_LESS, 0);
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
	uint32_t i, offset, color = 0;
	glm::mat4 rotX, rotY;
	glm::vec4 objEyePos, objLightPos;
	glm::mat4 modelMatrixIT, modelViewMatrix;
	glm::vec4 lightPos = glm::vec4(250.0f, 150.0f, 150.0f, 1);
	f32 globalAmbientColor[3] = { 0.8f, 0.7f, 0.7f };
	f32 lightColor[3] = { 0.95f, 0.95f, 0.95f };
	f32 materialColorDiffuse[3] = { 0.5f, 0.5f, 0.5f };
	f32 materialColorSpecular[3] = { 0.7f, 0.6f, 0.6f };
	f32 shininess = 17.8954f;

	eye_pos = glm::vec3(camPos.x, camPos.y, camPos.z);

	modelMatrixIT = glm::inverse(transformationMatrix);
	modelViewMatrix = glm::transpose(cameraViewMatrix * transformationMatrix);

	objEyePos = modelMatrixIT * glm::vec4(eye_pos.x, eye_pos.y, eye_pos.z, 1);
	objLightPos = modelMatrixIT * lightPos;

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

	if (settings.renderingMode == MaterialRenderingModes::Transparent)
	{
		rsxSetDepthWriteEnable(context, GCM_FALSE);
	}

	//Keep in memory the used settings
	lastSettings.invertFaces = settings.invertFaces;
	lastSettings.renderingMode = settings.renderingMode;
	lastSettings.useDepth = settings.useDepth;
	lastSettings.useLighting = settings.useLighting;
	lastSettings.useTexture = settings.useTexture;

	if (usedTexture != texture.m_ps3buffer)
	{
		usedTexture = texture.m_ps3buffer;
		BindTexture(texture);
	}

	// Set vertex array attributes
	{
		rsxAddressToOffset(&((VertexNormalsNoColor*)subMesh.data)[0].normX, &offset);
		rsxBindVertexArrayAttrib(context, GCM_VERTEX_ATTRIB_NORMAL, 0, offset, sizeof(VertexNormalsNoColor), 3, GCM_VERTEX_DATA_TYPE_F32, GCM_LOCATION_RSX);

		rsxAddressToOffset(&((VertexNormalsNoColor*)subMesh.data)[0].u, &offset);
		rsxBindVertexArrayAttrib(context, GCM_VERTEX_ATTRIB_TEX0, 0, offset, sizeof(VertexNormalsNoColor), 2, GCM_VERTEX_DATA_TYPE_F32, GCM_LOCATION_RSX);

		rsxAddressToOffset(&((VertexNormalsNoColor*)subMesh.data)[0].x, &offset);
		rsxBindVertexArrayAttrib(context, GCM_VERTEX_ATTRIB_POS, 0, offset, sizeof(VertexNormalsNoColor), 3, GCM_VERTEX_DATA_TYPE_F32, GCM_LOCATION_RSX);
	}

	// Update shaders
	{
		rsxLoadVertexProgram(context, vpo, vp_ucode);
		rsxLoadFragmentProgramLocation(context, fpo, fp_offset, GCM_LOCATION_RSX);

		rsxSetVertexProgramParameter(context, vpo, projMatrix, (float*)&projectionMatrix);
		rsxSetVertexProgramParameter(context, vpo, mvMatrix, (float*)&modelViewMatrix);

		// rsxSetFragmentProgramParameter(context, fpo, eyePosition, (float*)&objEyePos, fp_offset, GCM_LOCATION_RSX);
		rsxSetFragmentProgramParameter(context, fpo, globalAmbient, globalAmbientColor, fp_offset, GCM_LOCATION_RSX);
		// rsxSetFragmentProgramParameter(context, fpo, litPosition, (float*)&objLightPos, fp_offset, GCM_LOCATION_RSX);
		// rsxSetFragmentProgramParameter(context, fpo, litColor, lightColor, fp_offset, GCM_LOCATION_RSX);
		// rsxSetFragmentProgramParameter(context, fpo, spec, &shininess, fp_offset, GCM_LOCATION_RSX);

		// rsxSetFragmentProgramParameter(context, fpo, Kd, materialColorDiffuse, fp_offset, GCM_LOCATION_RSX);
		// rsxSetFragmentProgramParameter(context, fpo, Ks, materialColorSpecular, fp_offset, GCM_LOCATION_RSX);

	}

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
	// if (lastSettings.invertFaces != settings.invertFaces)
	// {
	// 	if (settings.invertFaces)
	// 	{
	// 		sceGuFrontFace(GU_CW);
	// 	}
	// 	else
	// 	{
	// 		sceGuFrontFace(GU_CCW);
	// 	}
	// }

	// if (lastSettings.useDepth != settings.useDepth)
	// {
	// 	if (settings.useDepth)
	// 	{
	// 		sceGuEnable(GU_DEPTH_TEST);
	// 	}
	// 	else
	// 	{
	// 		sceGuDisable(GU_DEPTH_TEST);
	// 	}
	// }

	// if (lastSettings.renderingMode != settings.renderingMode)
	// {
	// 	if (settings.renderingMode == MaterialRenderingModes::Opaque)
	// 	{
	// 		sceGuDisable(GU_BLEND);
	// 		sceGuDisable(GU_ALPHA_TEST);
	// 	}
	// 	else if (settings.renderingMode == MaterialRenderingModes::Cutout)
	// 	{
	// 		sceGuDisable(GU_BLEND);
	// 		sceGuEnable(GU_ALPHA_TEST);
	// 		sceGuAlphaFunc(GU_GEQUAL, static_cast<int>(material.GetAlphaCutoff() * 255), 0xff);
	// 	}
	// 	else
	// 	{
	// 		sceGuEnable(GU_BLEND);
	// 		sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
	// 		sceGuDisable(GU_ALPHA_TEST);
	// 	}
	// }

	// if (lastSettings.useLighting != settings.useLighting)
	// {
	// 	if (settings.useLighting)
	// 	{
	// 		sceGuEnable(GU_LIGHTING);
	// 	}
	// 	else
	// 	{
	// 		sceGuDisable(GU_LIGHTING);
	// 	}
	// }

	// if (lastSettings.useTexture != settings.useTexture)
	// {
	// 	sceGuEnable(GU_TEXTURE_2D);
	// }

	// // glDepthMask needs GL_FALSE here, pspsdk is doing this wrong, may change in a sdk update
	// if (settings.renderingMode == MaterialRenderingModes::Transparent)
	// {
	// 	sceGuDepthMask(GU_TRUE);
	// }

	// // Keep in memory the used settings
	// lastSettings.invertFaces = settings.invertFaces;
	// lastSettings.renderingMode = settings.renderingMode;
	// lastSettings.useDepth = settings.useDepth;
	// lastSettings.useLighting = settings.useLighting;
	// lastSettings.useTexture = settings.useTexture;

	// if (lastUsedColor != material.GetColor().GetUnsignedIntABGR() || lastUsedColor2 != subMesh.meshData->unifiedColor.GetUnsignedIntABGR())
	// {
	// 	lastUsedColor = material.GetColor().GetUnsignedIntABGR();
	// 	lastUsedColor2 = subMesh.meshData->unifiedColor.GetUnsignedIntABGR();
	// 	sceGuColor((material.GetColor() * subMesh.meshData->unifiedColor).GetUnsignedIntABGR());
	// }

	// // Bind texture
	// if (usedTexture != texture.data[0])
	// {
	// 	usedTexture = texture.data[0];
	// 	BindTexture(texture);
	// }
	// sceGuTexOffset(material.GetOffset().x, material.GetOffset().y);
	// sceGuTexScale(material.GetTiling().x, material.GetTiling().y);

	// // Draw
	// if (!subMesh.meshData->m_hasIndices)
	// {
	// 	sceGumDrawArray(GU_TRIANGLES, subMesh.meshData->pspDrawParam, subMesh.vertice_count, 0, subMesh.data);
	// }
	// else
	// {
	// 	sceGumDrawArray(GU_TRIANGLES, subMesh.meshData->pspDrawParam, subMesh.index_count, subMesh.indices, subMesh.data);
	// }
	// //Performance::AddDrawCall();


	// // glDepthMask needs GL_TRUE here, pspsdk is doing this wrong, may change in a sdk update
	// sceGuDepthMask(GU_FALSE);
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
	// const int textureLevelCount = texture.inVram.size();
	// for (int i = 0; i < textureLevelCount; i++)
	// {
	// 	if (texture.inVram[i])
	// 		vfree(texture.data[i]);
	// 	else
	// 		free(texture.data[i]);
	// }
}

void RendererRSX::SetTextureData(const Texture& texture, unsigned int textureType, const unsigned char* buffer)
{
	//Debug::Print("SetTextureData");

	// texture->m_ps3buffer = (uint32_t*)rsxMemalign(128, (texture.GetWidth() * texture.GetHeight() * 4));
	// if(!texture->m_ps3buffer) 
	//     return;

	// uint32_t texture_offset;
	// rsxAddressToOffset(texture_buffer, &texture_offset);

	// unsigned char* upBuffer = (u8*)texture->m_ps3buffer;
	// for(int i=0; i< texture.GetWidth() * texture.GetHeight() * 4; i+=4)
	// {
	// 	upBuffer[i + 1] = *buffer++;
	// 	upBuffer[i + 2] = *buffer++;
	// 	upBuffer[i + 3] = *buffer++;
	// 	upBuffer[i + 0] = *buffer++;
	// }
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

void RendererRSX::Setlights(const LightsIndices& lightsIndices, const Camera& camera)
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

void RendererRSX::UploadMeshData(const MeshData& meshData)
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

		// case WrapMode::ClampToEdge:
		// 	mode = GU_CLAMP_TO_EDGE;
		// 	break;
		// case WrapMode::ClampToBorder:
		// 	mode = GU_CLAMP_TO_BORDER;
		// 	break;
		// case WrapMode::MirroredRepeat:
		// 	mode = GU_MIRRORED_REPEAT;
		// 	break;
		// case WrapMode::Repeat:
		// 	mode = GU_REPEAT;
		// 	break;
		// case WrapMode::MirrorClampToEdge:
		// 	mode = GU_MIRROR_CLAMP_TO_EDGE;
		// 	break;
	}
	return mode;
}
#endif