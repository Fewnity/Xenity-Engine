// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#if defined(_EE2)
#include "renderer_gskit.h"

#include <engine/graphics/3d_graphics/mesh_data.h>
#include <engine/tools/profiler_benchmark.h>

#include <math3d.h>

#include <gsKit.h>
#include <gsInline.h>
#include <dmaKit.h>
#include <gsTexture.h>
#include <gsToolkit.h>

#include <draw.h>
#include <draw3d.h>

#include <memory>
#include <malloc.h>
#include <glm/gtc/type_ptr.hpp>

#include "cube_data.c"

int drawCount = 0;
int drawCount2 = 0;
VECTOR position = {0.00f, 0.00f, 0.00f, 1.00f};
VECTOR rotation = {0.00f, 0.00f, 0.00f, 1.00f};
glm::mat4 transformationMatrix = glm::mat4(1);

RendererGsKit::RendererGsKit()
{
}

int RendererGsKit::Init()
{
	int result = 0;

	gsGlobal = gsKit_init_global();

	gsGlobal->PSM = GS_PSM_CT16;
	gsGlobal->PSMZ = GS_PSMZ_16S;
	gsGlobal->PrimAlphaEnable = GS_SETTING_ON;
	gsGlobal->DoubleBuffering = GS_SETTING_OFF;
	//    gsGlobal->PrimAAEnable = GS_SETTING_ON;

	dmaKit_init(D_CTRL_RELE_OFF, D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC, D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);
	dmaKit_chan_init(DMA_CHANNEL_GIF);
	printf("gsGlobal->CurrentPointer: %d\n", gsGlobal->CurrentPointer);
	gsKit_init_screen(gsGlobal);
	printf("gsGlobal->CurrentPointer: %d\n", gsGlobal->CurrentPointer);
	gsKit_set_clamp(gsGlobal, GS_CMODE_REPEAT);

	gsKit_mode_switch(gsGlobal, GS_ONESHOT);
	gsKit_vram_clear(gsGlobal);
	gsKit_TexManager_init(gsGlobal);

	return result;
}

void RendererGsKit::Setup()
{
	gsKit_set_test(gsGlobal, GS_ZTEST_ON);
	gsGlobal->PrimAAEnable = GS_SETTING_ON;
}

void RendererGsKit::Stop()
{
}

void RendererGsKit::NewFrame()
{
}

void RendererGsKit::EndFrame()
{
	gsKit_queue_exec(gsGlobal);
	gsKit_sync_flip(gsGlobal);
	gsKit_TexManager_nextFrame(gsGlobal);
	drawCount2 = 0;
}

void RendererGsKit::SetViewport(int x, int y, int width, int height)
{
}

void RendererGsKit::SetClearColor(const Color &color)
{
}

void RendererGsKit::SetProjection2D(float projectionSize, float nearClippingPlane, float farClippingPlane)
{
}

void RendererGsKit::SetProjection3D(float fov, float nearClippingPlane, float farClippingPlane, float aspect)
{
}

void RendererGsKit::ResetView()
{
}

void RendererGsKit::SetCameraPosition(const std::shared_ptr<Camera> &camera)
{
	float zNear = camera->GetNearClippingPlane();
	float zFar = camera->GetFarClippingPlane();
	float fH = tan(float(camera->GetFov() / 360.0f * 3.14159f)) * zNear;
	float fW = fH * (4.0f / 3.0f);
	create_view_screen(view_screen, 4.0f / 3.0f, -0.5f, 0.5f, -0.5f, 0.5f, 0.30f, 1000.00f);
	// create_view_screen(view_screen, 4.0f / 3.0f, -fW, fW, -fH, fH, zFar, zNear);

	Vector3 camPos = camera->GetTransform()->GetPosition();
	Vector3 camRot = camera->GetTransform()->GetEulerAngles();

	// VECTOR camera_position = { camPos.x, camPos.y, camPos.z, 1.00f };
	// VECTOR camera_rotation = { camRot.x * 3.14159265359 / 180.0f, camRot.y * 3.14159265359 / 180.0f, camRot.z * 3.14159265359 / 180.0f, 1.00f };
	VECTOR camera_position = {0.00f, 4.00f, 3.00f, 1.00f};
	VECTOR camera_rotation = {0.00f, 0.00f, 180.00f * 3.14159265359 / 180.0f, 1.00f};
	create_world_view(world_view, camera_position, camera_rotation);
}

void RendererGsKit::ResetTransform()
{
}

void RendererGsKit::SetTransform(const Vector3 &position, const Vector3 &rotation, const Vector3 &scale, bool resetTransform)
{
	// transformationMatrix = glm::translate(transformationMatrix, glm::vec3(position.x, position.y, position.z));
	// transformationMatrix = glm::rotate(transformationMatrix, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	// transformationMatrix = glm::rotate(transformationMatrix, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	// transformationMatrix = glm::rotate(transformationMatrix, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
}

void RendererGsKit::SetTransform(const glm::mat4 &mat)
{
	transformationMatrix = mat;
}

void RendererGsKit::BindTexture(const std::shared_ptr<Texture> &texture)
{
}

void RendererGsKit::ApplyTextureFilters(const std::shared_ptr<Texture> &texture)
{
}

int gsKit_convert_xyz(vertex_f_t *output, GSGLOBAL *gsGlobal, int count, vertex_f_t *vertices)
{
	int z;
	unsigned int max_z;

	switch (gsGlobal->PSMZ)
	{
	case GS_PSMZ_32:
		z = 32;
		break;

	case GS_PSMZ_24:
		z = 24;
		break;

	case GS_PSMZ_16:
	case GS_PSMZ_16S:
		z = 16;
		break;

	default:
		return -1;
	}

	float center_x = gsGlobal->Width / 2;
	float center_y = gsGlobal->Height / 2;
	max_z = 1 << (z - 1);

	for (int i = 0; i < count; i++)
	{
		output[i].x = ((vertices[i].x + 1.0f) * center_x);
		output[i].y = ((vertices[i].y + 1.0f) * center_y);
		output[i].z = (unsigned int)((vertices[i].z + 1.0f) * max_z);
	}

	return 0;
}

void calculate_vertices_clipped(VECTOR *output, int count, VECTOR *vertices, MATRIX local_screen)
{
	asm __volatile__(
		"lqc2		$vf1, 0x00(%3)	    \n" // set local_screen matrix[0]
		"lqc2		$vf2, 0x10(%3)	    \n" // set local_screen matrix[1]
		"lqc2		$vf3, 0x20(%3)	    \n" // set local_screen matrix[2]
		"lqc2		$vf4, 0x30(%3)	    \n" // set local_screen matrix[3]
		// GS CLIP
		"li      $2,0x4580				\n"
		"dsll    $2, 16					\n"
		"ori     $2, 0x4580				\n"
		"dsll    $2, 16					\n"
		"qmtc2   $2,	$vf29			\n"
		"li      $2,	0x8000			\n"
		"ctc2    $2,	$vi1			\n"
		"li      $9,	0x00			\n"

		"calcvertices_loop:	            \n"
		"lqc2		$vf6, 0x00(%2)	    \n" // load XYZ
		"vmulaw		$ACC, $vf4, $vf0	\n"
		"vmaddax	$ACC, $vf1, $vf6	\n"
		"vmadday	$ACC, $vf2, $vf6	\n"
		"vmaddz		$vf7, $vf3, $vf6	\n"
		"vdiv    	$Q,	$vf0w,	$vf7w		\n"
		"vwaitq								\n"
		"vmulq.xyz		$vf7,	$vf7, $Q	\n"
		"vftoi4.xyzw	$vf8,	$vf7		\n"
		// GS CLIP
		"vnop							 \n"
		"vnop							 \n"
		"ctc2    $0,	$vi16            \n" // clear status flag
		"vsub.xyw  $vf0, $vf7,	$vf0     \n" //(z,ZBz,PPy,PPx)-(1,0,0,0);
		"vsub.xy   $vf0, $vf29,	$vf7  	 \n" //(Zmax,0,Ymax,Xmax) -(z,ZBz,PPy,PPx)
		"vnop                            \n"
		"vnop                            \n"
		"vnop							 \n"
		"vnop							 \n"
		"sll     $9,	1				 \n"
		"andi    $9,	$9, 6			 \n"
		"cfc2    $2,	$vi16            \n" // read status flag
		"andi    $2,	$2,0xc0			 \n"
		"beqz    $2,	calcvertices_skip1\n"
		"ori     $9,	$9,1			 \n"

		"calcvertices_skip1:			 \n"
		"beqz    $9,calcvertices_skip2	 \n"
		"vmfir.w $vf8,	$vi1			 \n"

		"calcvertices_skip2:		     \n"
		"sqc2		$vf7, 0x00(%0)	     \n" // Store XYZ
		"addi		%0, 0x10	         \n"
		"addi		%2, 0x10	         \n"
		"addi		%1, -1		         \n"
		"bne		$0, %1, calcvertices_loop	\n"
		: : "r"(output), "r"(count), "r"(vertices), "r"(local_screen) : "$10", "memory");
}

void RendererGsKit::DrawMeshData(const std::shared_ptr<MeshData> &meshData, const std::vector<std::shared_ptr<Texture>> &textures, RenderingSettings &settings)
{
	int subMeshCount = meshData->m_subMeshCount;
	size_t textureCount = textures.size();

	MeshData::SubMesh *subMesh = nullptr;

	MATRIX local_world;
	MATRIX local_screen;

	for (size_t i = 0; i < subMeshCount; i++)
	{
		subMesh = meshData->m_subMeshes[i];

		// Do not continue if there are more submeshes than textures
		if (i == textureCount)
			break;

		// Do not draw the submesh if the texture is null
		if (textures[i] == nullptr)
			continue;

		// Do not draw the submesh if the vertice count is 0
		if (subMesh->vertice_count == 0)
			continue;

		VECTOR *temp_vertices = (VECTOR *)memalign(128, sizeof(VECTOR) * subMesh->vertice_count);
		VECTOR *transformed_verts = (VECTOR *)memalign(128, sizeof(VECTOR) * subMesh->vertice_count);
		color_t *transformed_colors = (color_t *)memalign(128, sizeof(color_t) * subMesh->vertice_count);

		for (int i = 0; i < 16; i++)
		{
			local_world[i] = transformationMatrix[i / 4][i % 4];
		}

		// create_local_world(local_world, position, rotation);
		create_local_screen(local_screen, local_world, world_view, view_screen);

		// calculate_vertices(temp_vertices, subMesh->vertice_count, subMesh->c_verts, local_screen);
		calculate_vertices_clipped(temp_vertices, subMesh->vertice_count, subMesh->c_verts, local_screen);

		gsKit_convert_xyz((vertex_f_t *)transformed_verts, gsGlobal, subMesh->vertice_count, (vertex_f_t *)temp_vertices);

		draw_convert_rgbq(transformed_colors, subMesh->vertice_count, (vertex_f_t *)temp_vertices, (color_f_t *)subMesh->c_colours, 0x80);

		GSPRIMUVPOINT *transformed_vertices = (GSPRIMUVPOINT *)memalign(128, sizeof(GSPRIMUVPOINT) * subMesh->vertice_count);

		// GSTEXTURE* ps2Tex = &textures[i]->ps2Tex;

		// ---- Fill vertices
		for (unsigned int i2 = 0; i2 < subMesh->vertice_count; i2++)
		{
			transformed_vertices[i2].rgbaq = color_to_RGBAQ(transformed_colors[i2].r, transformed_colors[i2].g, transformed_colors[i2].b, transformed_colors[i2].a, 0.0f);

			// transformed_vertices[i].rgbaq = color_to_RGBAQ(0, 0, 0, 0, 0.0f);

			transformed_vertices[i2].xyz2 = vertex_to_XYZ2(gsGlobal, transformed_verts[i2][0], transformed_verts[i2][1], transformed_verts[i2][2]);
			// transformed_vertices[i].xyz2 = vertex_to_XYZ2(gsGlobal, 0, 0, 0);
			transformed_vertices[i2].uv = vertex_to_UV(&textures[i]->ps2Tex, subMesh->c_st[i2][0] * textures[i]->ps2Tex.Width, subMesh->c_st[i2][1] * textures[i]->ps2Tex.Height);
			// transformed_vertices[i].uv = vertex_to_UV(ps2Tex, 0 * ps2Tex->Width, 0 * ps2Tex->Height);
		}

		gsKit_prim_list_triangle_goraud_texture_uv_3d(gsGlobal, &textures[i]->ps2Tex, subMesh->vertice_count, transformed_vertices); // With texture

		free(transformed_vertices);
		free(temp_vertices);
		free(transformed_verts);
		free(transformed_colors);
	}
}

void RendererGsKit::DrawLine(const Vector3 &a, const Vector3 &b, const Color &color, RenderingSettings &settings)
{
}

unsigned int RendererGsKit::CreateNewTexture()
{
	return 0;
}

void RendererGsKit::DeleteTexture(Texture *texture)
{
}

void RendererGsKit::SetTextureData(const std::shared_ptr<Texture> &texture, unsigned int textureType, const unsigned char *buffer)
{
}

void RendererGsKit::SetLight(int lightIndex, const Vector3 &lightPosition, float intensity, Color color, LightType type, float attenuation)
{
}

void RendererGsKit::DisableAllLight()
{
}

void RendererGsKit::Setlights(const std::shared_ptr<Camera> &camera)
{
}

void RendererGsKit::Clear()
{
	u64 CLEAR_CLOR = GS_SETREG_RGBAQ(189, 13, 136, 0x00, 0x00);
	gsKit_clear(gsGlobal, CLEAR_CLOR);
}

void RendererGsKit::SetFog(bool m_active)
{
}

void RendererGsKit::SetFogValues(float start, float end, const Color &color)
{
}

void RendererGsKit::DeleteSubMeshData(MeshData::SubMesh *subMesh)
{
}

void RendererGsKit::UploadMeshData(const std::shared_ptr<MeshData> &meshData)
{
}

#endif