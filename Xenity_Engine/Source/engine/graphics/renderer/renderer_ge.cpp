#if defined(__PSP__)
#include "renderer_ge.h"

#include <engine/graphics/3d_graphics/mesh_data.h>
#include <engine/tools/profiler_benchmark.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/tools/math.h>
#include <engine/debug/performance.h>
#include <engine/graphics/graphics.h>
#include <engine/ui/window.h>
#include <engine/graphics/camera.h>
#include <engine/game_elements/gameobject.h>
#include <engine/game_elements/transform.h>

static unsigned int __attribute__((aligned(16))) list[262144];
#include <pspkernel.h>
#define GUGL_IMPLEMENTATION
#include <psp/gu2gl.h>

#include <memory>
#include <glm/gtc/type_ptr.hpp>
#include <vram.h>
#include <pspdisplay.h>
#include <pspgu.h>
#include <pspgum.h>

RendererGE::RendererGE()
{
}

int RendererGE::Init()
{
	int result = 0;

	int displayBufferByteCount = 2;
	bool highQualityColor = false;
	int displayColorMode = GU_PSM_5650;
	if (highQualityColor)
	{
		displayBufferByteCount = 4;
		displayColorMode = GU_PSM_8888;
	}

	void* fbp0 = vramalloc(PSP_BUF_WIDTH * PSP_SCR_HEIGHT * displayBufferByteCount);
	void* fbp1 = vramalloc(PSP_BUF_WIDTH * PSP_SCR_HEIGHT * displayBufferByteCount);
	void* zbp = vramalloc(PSP_BUF_WIDTH * PSP_SCR_HEIGHT * 2);

	sceGuInit();

	sceGuStart(GU_DIRECT, list);
	sceGuDrawBuffer(displayColorMode, vrelptr(fbp0), PSP_BUF_WIDTH);
	sceGuDispBuffer(PSP_SCR_WIDTH, PSP_SCR_HEIGHT, vrelptr(fbp1), PSP_BUF_WIDTH);
	sceGuDepthBuffer(vrelptr(zbp), PSP_BUF_WIDTH);
	sceGuOffset(2048 - (PSP_SCR_WIDTH / 2), 2048 - (PSP_SCR_HEIGHT / 2));
	sceGuViewport(2048, 2048, PSP_SCR_WIDTH, PSP_SCR_HEIGHT);

	sceGuDepthRange(100, 65535);
	sceGuScissor(0, 0, PSP_SCR_WIDTH, PSP_SCR_HEIGHT);
	sceGuEnable(GU_SCISSOR_TEST);

	sceGuDepthFunc(GU_LEQUAL);
	sceGuEnable(GU_DEPTH_TEST);
	sceGuFrontFace(GU_CCW);
	sceGuShadeModel(GU_SMOOTH);
	sceGuEnable(GU_CULL_FACE);
	sceGuEnable(GU_TEXTURE_2D);
	sceGuEnable(GU_CLIP_PLANES);

	sceGuFinish();
	sceGuSync(0, 0);

	sceDisplayWaitVblankStart();
	sceGuDisplay(GU_TRUE);

	maxLightCount = 4;
	return result;
}

void RendererGE::Setup()
{
}

void RendererGE::Stop()
{
	guglTerm();
}

void RendererGE::NewFrame()
{
	guglStartFrame(list, GL_FALSE);
}

void RendererGE::EndFrame()
{
	usedTexture.reset();
	guglSwapBuffers(GL_FALSE, GL_FALSE);
}

void RendererGE::SetViewport(int x, int y, int width, int height)
{
	glViewport(x, y, width, height);
}

void RendererGE::SetClearColor(const Color &color)
{
	glClearColor(color.GetUnsignedIntABGR());
}

void RendererGE::SetProjection2D(float projectionSize, float nearClippingPlane, float farClippingPlane)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float halfRatio = Graphics::usedCamera.lock()->GetAspectRatio() / 2.0f * 10 * (projectionSize / 5.0f);
	float halfOne = 0.5f * 10 * (projectionSize / 5.0f);
	glOrtho(-halfRatio, halfRatio, -halfOne, halfOne, nearClippingPlane, farClippingPlane);
}

void RendererGE::SetProjection3D(float fov, float nearClippingPlane, float farClippingPlane, float aspect)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glPerspective(fov, Window::GetAspectRatio(), nearClippingPlane, farClippingPlane);
}

void RendererGE::ResetView()
{
	glMatrixMode(GL_VIEW);
	glLoadIdentity();
	gluRotateY(180 / 180.0f * 3.14159f);
}

void RendererGE::SetCameraPosition(const std::shared_ptr<Camera> &camera)
{
	auto transform = camera->GetTransform();
	Vector3 position = transform->GetPosition();
	Vector3 rotation = transform->GetRotation();
	glMatrixMode(GL_VIEW);
	glLoadIdentity();

	gluRotateZ((-rotation.z) / 180.0f * 3.14159f);
	gluRotateX(rotation.x / 180.0f * 3.14159f);
	gluRotateY((rotation.y + 180) / 180.0f * 3.14159f);

	glTranslatef(position.x, -position.y, -position.z);
}

void RendererGE::ResetTransform()
{
	glMatrixMode(GL_MODEL);
	glLoadIdentity();
}

void RendererGE::SetTransform(const Vector3 &position, const Vector3 &rotation, const Vector3 &scale, bool resetTransform)
{
	glMatrixMode(GL_MODEL);
	if (resetTransform)
		glLoadIdentity();
	glTranslatef(-position.x, position.y, position.z);
	gluRotateY(-rotation.y * 3.14159265359 / 180.0f);
	gluRotateX(rotation.x * 3.14159265359 / 180.0f);
	gluRotateZ(-rotation.z * 3.14159265359 / 180.0f);
	glScalef(scale.x, scale.y, scale.z);
}

void RendererGE::SetTransform(const glm::mat4 &mat)
{
	sceGuSetMatrix(GU_MODEL, (ScePspFMatrix4*)&mat);
}

void RendererGE::BindTexture(const std::shared_ptr<Texture> &texture)
{
	glTexMode(texture->type, texture->mipmaplevelCount, 0, 1);
	glTexFunc(GL_TFX_MODULATE, GL_TCC_RGBA);
	// Set mipmap behavior
	if (texture->useMipMap)
		sceGuTexLevelMode(GU_TEXTURE_AUTO, -1); // Greater is lower quality
	// sceGuTexLevelMode(GL_TEXTURE_CONST, 1); // Set mipmap level to use
	// sceGuTexLevelMode(GL_TEXTURE_SLOPE, 2); //??? has no effect

	glTexImage(0, texture->pW, texture->pH, texture->pW, texture->data[0]);
	// Send mipmap data
	if (texture->useMipMap)
	{
		glTexImage(1, texture->pW / 2, texture->pH / 2, texture->pW / 2, texture->data[1]);
		// glTexImage(2, texture->pW / 4, texture->pH / 4, texture->pW / 4, texture->data[2]);
		// glTexImage(3, texture->pW / 8, texture->pH / 8, texture->pW / 8, texture->data[3]);
	}
	ApplyTextureFilters(texture);
}

void RendererGE::ApplyTextureFilters(const std::shared_ptr<Texture> &texture)
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

	// Apply filters
	glTexFilter(minFilterValue, magfilterValue);
	glTexWrap(wrap, wrap);

}

void RendererGE::DrawMeshData(const std::shared_ptr<MeshData> &meshData, const std::vector<std::shared_ptr<Texture>> &textures, RenderingSettings &settings)
{
	if (!meshData->isValid)
		return;

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
		if (settings.useLighting && !settings.useBlend)
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

	// Keep in memory the used settings
	lastSettings.invertFaces = settings.invertFaces;
	lastSettings.useBlend = settings.useBlend;
	lastSettings.useDepth = settings.useDepth;
	lastSettings.useLighting = settings.useLighting;
	lastSettings.useTexture = settings.useTexture;

	int subMeshCount = meshData->subMeshCount;
	size_t textureCount = textures.size();

	int params = 0;

	// Get the parameters to use dependings of the mesh data format
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

	MeshData::SubMesh* subMesh = nullptr;

	for (int i = 0; i < subMeshCount; i++)
	{
		subMesh = meshData->subMeshes[i];

		// Do not continue if there are more submeshes than textures
		if (i == textureCount)
			break;

		// Do not draw the submesh if the texture is null
		if (textures[i] == nullptr)
			continue;

		// Bind texture
		if (usedTexture != textures[i])
		{
			usedTexture = textures[i];
			BindTexture(textures[i]);
		}

		// Draw
		if (!meshData->hasIndices)
		{
			glDrawElements(GL_TRIANGLES, params, subMesh->vertice_count, 0, subMesh->data);
		}
		else
		{
			glDrawElements(GL_TRIANGLES, params, subMesh->index_count, subMesh->indices, subMesh->data);
		}
	}
	Performance::AddDrawCall();
}

void RendererGE::DrawLine(const Vector3 &a, const Vector3 &b, const Color &color, RenderingSettings &settings)
{
}

unsigned int RendererGE::CreateNewTexture()
{
	return 0;
}

void RendererGE::DeleteTexture(Texture *texture)
{
}

void RendererGE::SetTextureData(const std::shared_ptr<Texture> &texture, unsigned int textureType, const unsigned char *buffer)
{
}

void RendererGE::SetLight(int lightIndex, const Vector3 &lightPosition, float intensity, Color color, Light::LightType type, float attenuation)
{
	if (lightIndex >= maxLightCount)
		return;

	RGBA rgba = color.GetRGBA();

	glEnable(GL_LIGHT0 + lightIndex);

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
}

void RendererGE::DisableAllLight()
{
	for (int lightIndex = 0; lightIndex < maxLightCount; lightIndex++)
	{
		glDisable(GL_LIGHT0 + lightIndex);
	}
}

void RendererGE::Setlights(const std::shared_ptr<Camera> &camera)
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

void RendererGE::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT /*| GL_STENCIL_BUFFER_BIT*/);
}

void RendererGE::SetFog(bool active)
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

void RendererGE::SetFogValues(float start, float end, const Color &color)
{
	fogStart = start;
	fogEnd = end;
	fogColor = color;
	sceGuFog(fogStart, fogEnd, fogColor.GetUnsignedIntABGR());
}

void RendererGE::DeleteSubMeshData(MeshData::SubMesh *subMesh)
{
}

void RendererGE::UploadMeshData(const std::shared_ptr<MeshData> &meshData)
{
}

int RendererGE::GetWrapModeEnum(Texture::WrapMode wrapMode)
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
#endif