// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#if defined(_EE)
#include "renderer_vu1.h"

#include <engine/graphics/3d_graphics/mesh_data.h>
#include <engine/tools/profiler_benchmark.h>
#include <engine/debug/debug.h>
#include <engine/graphics/graphics.h>
#include <engine/ui/window.h>
#include <kernel.h>
#include <malloc.h>
#include <tamtypes.h>
#include <gs_psm.h>
#include <dma.h>
#include <packet2.h>
#include <packet2_utils.h>
#include <graph.h>
#include <draw.h>

#include <unistd.h>

#include <memory>
#include <malloc.h>
#include <glm/gtc/type_ptr.hpp>
#include <engine/graphics/camera.h>
#include <engine/game_elements/transform.h>
#include "mesh_data.c"
#include "zbyszek.c"

extern unsigned char zbyszek[];

// #include "cube_data.c"

int drawCount = 0;
int drawCount2 = 0;
glm::mat4 transformationMatrix = glm::mat4(1);
framebuffer_t frame;
zbuffer_t z;
packet2_t *zbyszek_packet;

extern u32 VU1Draw3D_CodeStart __attribute__((section(".vudata")));
extern u32 VU1Draw3D_CodeEnd __attribute__((section(".vudata")));
VECTOR *c_verts __attribute__((aligned(128))), *c_sts __attribute__((aligned(128)));
packet2_t *vif_packets[2] __attribute__((aligned(64)));
packet2_t *curr_vif_packet = nullptr;

VECTOR object_position;
VECTOR object_rotation = {0.00f, 0.00f, 0.00f, 1.00f};
VECTOR camera_position = {0.00f, 0.00f, 5.00f, 1.00f}; // VECTOR camera_position = {240.00f, 240.00f, 40.00f, 1.00f};
VECTOR camera_rotation = {0.00f, 0.00f, 0.00f, 1.00f};
MATRIX local_world, world_view, view_screen, local_screen;

u8 context = 0;

RendererVU1::RendererVU1()
{
}

void RendererVU1::calculate_cube(texbuffer_t *t_texbuff)
{
	packet2_add_float(zbyszek_packet, 2048.0F);					  // scale
	packet2_add_float(zbyszek_packet, 2048.0F);					  // scale
	packet2_add_float(zbyszek_packet, ((float)0xFFFFFF) / 32.0F); // scale
	packet2_add_s32(zbyszek_packet, faces_count);				  // vertex count
	packet2_utils_gif_add_set(zbyszek_packet, 1);
	packet2_utils_gs_add_lod(zbyszek_packet, &lod);
	packet2_utils_gs_add_texbuff_clut(zbyszek_packet, t_texbuff, &clut);
	packet2_utils_gs_add_prim_giftag(zbyszek_packet, &prim, faces_count, DRAW_STQ2_REGLIST, 3, 0);
	u8 j = 0; // RGBA
			  // for (j = 0; j < 4; j++)
	// packet2_add_u32(zbyszek_packet, 128);
	packet2_add_u32(zbyszek_packet, 128);
	packet2_add_u32(zbyszek_packet, 128);
	packet2_add_u32(zbyszek_packet, 128);
	packet2_add_u32(zbyszek_packet, 128);
}

void RendererVU1::vu1_set_double_buffer_settings()
{
	packet2_t *packet2 = packet2_create(1, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);
	packet2_utils_vu_add_double_buffer(packet2, 8, 496);
	packet2_utils_vu_add_end_tag(packet2);
	dma_channel_send_packet2(packet2, DMA_CHANNEL_VIF1, 1);
	dma_channel_wait(DMA_CHANNEL_VIF1, 0);
	packet2_free(packet2);
}

void RendererVU1::vu1_upload_micro_program()
{
	u32 packet_size = packet2_utils_get_packet_size_for_program(&VU1Draw3D_CodeStart, &VU1Draw3D_CodeEnd) + 1; // + 1 for end tag
	packet2_t *packet2 = packet2_create(packet_size, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);
	packet2_vif_add_micro_program(packet2, 0, &VU1Draw3D_CodeStart, &VU1Draw3D_CodeEnd);
	packet2_utils_vu_add_end_tag(packet2);
	dma_channel_send_packet2(packet2, DMA_CHANNEL_VIF1, 1);
	FlushCache(0);
	dma_channel_wait(DMA_CHANNEL_VIF1, 0);
	packet2_free(packet2);
}

/** Send texture data to GS. */
void RendererVU1::send_texture(texbuffer_t *texbuf, int id)
{
	packet2_t *packet2 = packet2_create(50, P2_TYPE_NORMAL, P2_MODE_CHAIN, 0);
	// if (id == 0)
	packet2_update(packet2, draw_texture_transfer(packet2->next, zbyszek, 128, 128, GS_PSM_24, texbuf->address, texbuf->width));
	// else
	// packet2_update(packet2, draw_texture_transfer(packet2->next, zbyszek2, 128, 128, GS_PSM_24, texbuf->address, texbuf->width));
	packet2_update(packet2, draw_texture_flush(packet2->next));
	FlushCache(0);
	dma_channel_send_packet2(packet2, DMA_CHANNEL_GIF, 1);
	dma_wait_fast();
	packet2_free(packet2);
}

/** Send packet which will clear our screen. */
void RendererVU1::clear_screen(framebuffer_t *frame, zbuffer_t *z)
{
	packet2_t *clear = packet2_create(35, P2_TYPE_NORMAL, P2_MODE_NORMAL, 0);

	// Clear framebuffer but don't update zbuffer.
	packet2_update(clear, draw_disable_tests(clear->next, 0, z));
	// packet2_update(clear, draw_clear(clear->next, 0, 2048.0f - 320.0f, 2048.0f - 256.0f, frame->width, frame->height, 0x40, 0x40, 0x80));
	packet2_update(clear, draw_clear(clear->next, 0, 2048.0f - 320.0f, 2048.0f - 256.0f, frame->width, frame->height, rand() % 64, rand() % 64, 0x80));
	packet2_update(clear, draw_enable_tests(clear->next, 0, z));
	packet2_update(clear, draw_finish(clear->next));

	// Now send our current dma chain.
	dma_wait_fast();
	FlushCache(0);
	dma_channel_send_packet2(clear, DMA_CHANNEL_GIF, 1);

	packet2_free(clear);

	// Wait for scene to finish drawing
	draw_wait_finish();
}

void RendererVU1::set_lod_clut_prim_tex_buff()
{
	lod.calculation = LOD_USE_K;
	lod.max_level = 0;
	lod.mag_filter = LOD_MAG_NEAREST;
	lod.min_filter = LOD_MIN_NEAREST;
	lod.l = 0;
	lod.k = 0;

	clut.storage_mode = CLUT_STORAGE_MODE1;
	clut.start = 0;
	clut.psm = 0;
	clut.load_method = CLUT_NO_LOAD;
	clut.address = 0;

	// Define the triangle primitive we want to use.
	prim.type = PRIM_TRIANGLE;
	prim.shading = PRIM_SHADE_GOURAUD;
	prim.mapping = DRAW_ENABLE;
	prim.fogging = DRAW_DISABLE;
	prim.blending = DRAW_ENABLE;
	prim.antialiasing = DRAW_DISABLE;
	prim.mapping_type = PRIM_MAP_ST;
	prim.colorfix = PRIM_UNFIXED;

	// texbuff.info.width = draw_log2(128);
	// texbuff.info.height = draw_log2(128);
	// texbuff.info.components = TEXTURE_COMPONENTS_RGB;
	// texbuff.info.function = TEXTURE_FUNCTION_MODULATE;
}

int RendererVU1::Init()
{
	int result = 0;

	dma_channel_initialize(DMA_CHANNEL_GIF, NULL, 0);
	dma_channel_initialize(DMA_CHANNEL_VIF1, NULL, 0);
	dma_channel_fast_waits(DMA_CHANNEL_GIF);
	dma_channel_fast_waits(DMA_CHANNEL_VIF1);

	// Initialize vif packets
	zbyszek_packet = packet2_create(10, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);
	// zbyszek_packet2 = packet2_create(10, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);
	vif_packets[0] = packet2_create(11, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);
	vif_packets[1] = packet2_create(11, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);

	vu1_upload_micro_program();
	vu1_set_double_buffer_settings();

	frame.width = 640;
	frame.height = 512;
	frame.mask = 0;
	frame.psm = GS_PSM_32;
	frame.address = graph_vram_allocate(frame.width, frame.height, frame.psm, GRAPH_ALIGN_PAGE);

	// Enable the zbuffer.
	z.enable = DRAW_ENABLE;
	z.mask = 0;
	z.method = ZTEST_METHOD_GREATER_EQUAL;
	z.zsm = GS_ZBUF_32;
	z.address = graph_vram_allocate(frame.width, frame.height, z.zsm, GRAPH_ALIGN_PAGE);

	graph_initialize(frame.address, frame.width, frame.height, frame.psm, 0, 0);

	packet2_t *packet2 = packet2_create(20, P2_TYPE_NORMAL, P2_MODE_NORMAL, 0);

	// This will setup a default drawing environment.
	packet2_update(packet2, draw_setup_environment(packet2->next, 0, &frame, &z));

	// Now reset the primitive origin to 2048-width/2,2048-height/2.
	packet2_update(packet2, draw_primitive_xyoffset(packet2->next, 0, (2048 - 320), (2048 - 256)));

	// Finish setting up the environment.
	packet2_update(packet2, draw_finish(packet2->next));

	// Now send the packet, no need to wait since it's the first.
	FlushCache(0);
	dma_channel_send_packet2(packet2, DMA_CHANNEL_GIF, 1);
	dma_wait_fast();

	packet2_free(packet2);

	set_lod_clut_prim_tex_buff();

	// texbuff.width = 128;
	// texbuff.psm = GS_PSM_24;
	// texbuff.address = graph_vram_allocate(128, 128, GS_PSM_24, GRAPH_ALIGN_BLOCK);
	// send_texture(&texbuff, 0);
	// calculate_cube(&texbuff);

	// c_verts = (VECTOR *)memalign(128, sizeof(VECTOR) * faces_count);
	// c_sts = (VECTOR *)memalign(128, sizeof(VECTOR) * faces_count);

	// for (int i = 0; i < faces_count; i++)
	// {
	// 	c_verts[i][0] = vertices[faces[i]][0];
	// 	c_verts[i][1] = vertices[faces[i]][1];
	// 	c_verts[i][2] = vertices[faces[i]][2];
	// 	c_verts[i][3] = vertices[faces[i]][3];

	// 	c_sts[i][0] = sts[faces[i]][0];
	// 	c_sts[i][1] = sts[faces[i]][1];
	// 	c_sts[i][2] = sts[faces[i]][2];
	// 	c_sts[i][3] = sts[faces[i]][3];
	// }

	Window::SetResolution(640, 512);

	return result;
}

void RendererVU1::Setup()
{
}

void RendererVU1::Stop()
{
	packet2_free(vif_packets[0]);
	packet2_free(vif_packets[1]);
	packet2_free(zbyszek_packet);
	// packet2_free(zbyszek_packet2);
}

void RendererVU1::NewFrame()
{
}

void RendererVU1::EndFrame()
{
	graph_wait_vsync();
	drawCount2 = 0;
}

void RendererVU1::SetViewport(int x, int y, int width, int height)
{
}

void RendererVU1::SetClearColor(const Color &color)
{
}

void RendererVU1::SetProjection2D(float projectionSize, float nearClippingPlane, float farClippingPlane)
{
}

void RendererVU1::SetProjection3D(float fov, float nearClippingPlane, float farClippingPlane, float aspect)
{
}

void RendererVU1::ResetView()
{
}

void RendererVU1::SetCameraPosition(const std::shared_ptr<Camera> &camera)
{
	// float zNear = camera->GetNearClippingPlane();
	// float zFar = camera->GetFarClippingPlane();
	// float fH = tan(float(camera->GetFov() / 360.0f * 3.14159f)) * zNear;
	// float fW = fH * (4.0f / 3.0f);
	// create_view_screen(view_screen, 4.0f / 3.0f, -0.5f, 0.5f, -0.5f, 0.5f, 0.30f, 1000.00f);
	// // create_view_screen(view_screen, 4.0f / 3.0f, -fW, fW, -fH, fH, zFar, zNear);

	Vector3 camPos = camera->GetTransform()->GetPosition();
	Vector3 camRot = camera->GetTransform()->GetEulerAngles();

	// camera_position[0] = camPos.x;
	// camera_position[1] = camPos.y;
	// camera_position[2] = camPos.z;
	// camera_position[3] = 1.00f;

	// camera_rotation[0] = camRot.x * 3.14159265359 / 180.0f;
	// camera_rotation[1] = camRot.y * 3.14159265359 / 180.0f;
	// camera_rotation[2] = camRot.z * 3.14159265359 / 180.0f;
	// camera_rotation[0] = 10 * 3.14159265359 / 180.0f;
	// camera_rotation[1] = 0 * 3.14159265359 / 180.0f;
	// camera_rotation[2] = 0 * 3.14159265359 / 180.0f;
	// camera_rotation[3] = 1.0f;

	// camera_rotation = {camRot.x * 3.14159265359 / 180.0f, camRot.y * 3.14159265359 / 180.0f, camRot.z * 3.14159265359 / 180.0f, 1.00f};
	// Debug::Print("Salut");
	//   VECTOR camera_position = {0.00f, 4.00f, 3.00f, 1.00f};
	//   VECTOR camera_rotation = {0.00f, 0.00f, 180.00f * 3.14159265359 / 180.0f, 1.00f};
	//   create_world_view(world_view, camera_position, camera_rotation);

	VECTOR camera_position2 = {-camPos.x, camPos.y, camPos.z, 1.00f};																								  //- + +
	VECTOR camera_rotation2 = {camRot.x * 3.14159265359 / 180.0f, (180.0f + camRot.y) * 3.14159265359 / 180.0f, (180.0f - camRot.z) * 3.14159265359 / 180.0f, 1.00f}; // + + -

	// VECTOR camera_position2 = {0, 2, -4, 1.00f};																								 //- + +
	// VECTOR camera_rotation2 = {0 * 3.14159265359 / 180.0f, (180.0f + 0) * 3.14159265359 / 180.0f, (180.0f - 0) * 3.14159265359 / 180.0f, 1.00f}; // + + -
	create_world_view(world_view, camera_position2, camera_rotation2);
}

void RendererVU1::ResetTransform()
{
}

void RendererVU1::SetTransform(const Vector3 &position, const Vector3 &rotation, const Vector3 &scale, bool resetTransform)
{
	// transformationMatrix = glm::translate(transformationMatrix, glm::vec3(position.x, position.y, position.z));
	// transformationMatrix = glm::rotate(transformationMatrix, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	// transformationMatrix = glm::rotate(transformationMatrix, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	// transformationMatrix = glm::rotate(transformationMatrix, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
}

void RendererVU1::SetTransform(const glm::mat4 &mat)
{
	transformationMatrix = mat;
}

void RendererVU1::BindTexture(const std::shared_ptr<Texture> &texture)
{
}

void RendererVU1::ApplyTextureFilters(const std::shared_ptr<Texture> &texture)
{
}
int t = 0;
void RendererVU1::DrawMeshData(const std::shared_ptr<MeshData> &meshData, const std::vector<std::shared_ptr<Texture>> &textures, RenderingSettings &settings)
{
	// create_view_screen(view_screen, graph_aspect_ratio(), -3.00f, 3.00f, -3.00f, 3.00f, 1.00f, 2000.00f);
	float t;
	create_view_screen(view_screen, graph_aspect_ratio(), 0.098f, -0.098f, -0.14f, 0.14f, 0.03f, 1000.00f);
	// glm::mat4 projection = glm::perspective(glm::radians(60.0f), 640.0f / 448.0f, 1.0f, 2000.0f);

	// for (int i = 0; i < 16; i++)
	// {
	// 	view_screen[i] = projection[i / 4][i % 4];
	// 	// world_view[i] = Graphics::usedCamera.lock()->GetTransform()->transformationMatrix[i / 4][i % 4];
	// }

	t++;
	// if (t == 100)
	// {
	// 	t = 0;
	// 	object_rotation[0] += 0.008f;
	// 	while (object_rotation[0] > 3.14f)
	// 	{
	// 		object_rotation[0] -= 6.28f;
	// 	}
	// 	object_rotation[1] += 0.012f;
	// 	while (object_rotation[1] > 3.14f)
	// 	{
	// 		object_rotation[1] -= 6.28f;
	// 	}

	// 	// camera_position[2] += 1.5F;
	// 	// camera_rotation[2] += 0.002f;
	// 	// if (camera_position[2] >= 800.0F)
	// 	// {
	// 	// 	camera_position[2] = 40.0F;
	// 	// 	camera_rotation[2] = 0.00f;
	// 	// }
	// }
	// VECTOR camera_position2 = {1.00f, 1.00f, -2.00f, 1.00f};																						 //- + +
	// VECTOR camera_rotation2 = {20.0f * 3.14159265359 / 180.0f, (180.0f + 0) * 3.14159265359 / 180.0f, (180.0f - 0) * 3.14159265359 / 180.0f, 1.00f}; // ? + -
	// create_world_view(world_view, camera_position2, camera_rotation2);
	// glm::mat4 transformationMatrixC = glm::mat4(1);
	// transformationMatrixC = glm::translate(transformationMatrixC, glm::vec3(0, 0, 0));
	// // transformationMatrixC = glm::rotate(transformationMatrixC, (t * 0.005f) * 3.14159265359f / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	// // transformationMatrixC = glm::rotate(transformationMatrixC, (0.0f) * 3.14159265359f / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	// // transformationMatrixC = glm::rotate(transformationMatrixC, (180.0f + 0) * 3.14159265359f / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	// transformationMatrixC = glm::rotate(transformationMatrixC, 0 * 3.14159265359f / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	// transformationMatrixC = glm::rotate(transformationMatrixC, (0.0f) * 3.14159265359f / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	// transformationMatrixC = glm::rotate(transformationMatrixC, 0 * 3.14159265359f / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	// for (int i = 0; i < 16; i++)
	// {
	// 	// world_view[i] = transformationMatrixC[i / 4][i % 4];
	// 	//  world_view[i] = Graphics::usedCamera.lock()->GetTransform()->transformationMatrix[i / 4][i % 4];
	// }
	///
	//
	//

	// create_local_world(local_world, object_position, object_rotation);
	for (int i = 0; i < 16; i++)
	{
		local_world[i] = transformationMatrix[i / 4][i % 4];
	}
	create_local_screen(local_screen, local_world, world_view, view_screen);

	// return;
	int subMeshCount = meshData->m_subMeshCount;

	size_t textureCount = textures.size();

	MeshData::SubMesh *subMesh = nullptr;
	drawCount2++;
	// if (drawCount2 != 2)
	// 	return;
	// if (subMeshCount != 6)
	// 	return;

	if (drawCount2 >= 30)
		return;

	for (size_t i = 0; i < subMeshCount; i++)
	// for (size_t i = 0; i < 5; i++)
	// for (size_t i = 5; i < 6; i++)
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

		// if (subMesh->vertice_count != 756)
		// 	break;
		// if (drawCount2 != 2)
		// 	break;
		// Debug::Print("A");

		// if (i == 1)
		// 	break;

		meshData->UpdatePS2Packets(i, textures[i]);
		// printf("v%d i%d\n", subMesh->vertice_count, subMesh->index_count);
		//  Debug::Print("" + std::to_string(subMesh->vertice_count));

		// we don't wan't to unpack at 8 + beggining of buffer, but at
		// the beggining of the buffer

		// packet2_utils_vu_add_unpack_data(curr_vif_packet, vif_added_bytes, zbyszek_packet->base, packet2_get_qw_count(zbyszek_packet), 1);
		// vif_added_bytes += packet2_get_qw_count(zbyszek_packet);

		// packet2_utils_vu_add_unpack_data(curr_vif_packet, vif_added_bytes, c_verts, faces_count, 1);
		// vif_added_bytes += faces_count;

		// packet2_utils_vu_add_unpack_data(curr_vif_packet, vif_added_bytes, c_sts, faces_count, 1);
		// vif_added_bytes += faces_count;
		int verticesToDraw = subMesh->vertice_count;
		int verticesDrawn = 0;
		while (verticesToDraw > 0)
		// for (int ttt = 0; ttt < 2; ttt++)
		{
			int count = 36;
			if (verticesToDraw < 36)
				count = verticesToDraw;
			// Debug::Print("Draw " + std::to_string(count));
			// curr_vif_packet = packet2_create(2048, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);
			curr_vif_packet = vif_packets[context];

			packet2_reset(curr_vif_packet, 0);
			// Add matrix at the beggining of VU mem (skip TOP)
			packet2_utils_vu_add_unpack_data(curr_vif_packet, 0, &local_screen, 8, 0);
			u32 vif_added_bytes = 0; // zero because now we will use TOP register (double buffer)

			packet2_utils_vu_add_unpack_data(curr_vif_packet, vif_added_bytes, subMesh->meshPacket->base, packet2_get_qw_count(subMesh->meshPacket), 1);
			vif_added_bytes += packet2_get_qw_count(subMesh->meshPacket);
			packet2_utils_vu_add_unpack_data(curr_vif_packet, vif_added_bytes, subMesh->c_verts + verticesDrawn, count, 1);
			vif_added_bytes += 36;

			packet2_utils_vu_add_unpack_data(curr_vif_packet, vif_added_bytes, subMesh->c_st + verticesDrawn, count, 1);
			// vif_added_bytes += 36;

			packet2_utils_vu_add_start_program(curr_vif_packet, 0);
			packet2_utils_vu_add_end_tag(curr_vif_packet);

			dma_channel_wait(DMA_CHANNEL_VIF1, 0);
			FlushCache(0);
			dma_channel_send_packet2(curr_vif_packet, DMA_CHANNEL_VIF1, 1);
			// draw_wait_finish();
			//  packet2_free(curr_vif_packet);

			// sleep(1);
			//   Switch packet, so we can proceed during DMA transfer
			if (context == 0)
				context = 1;
			else
				context = 0;
			// context = !context;
			verticesToDraw -= count;
			verticesDrawn += count;
		}
	}
	// struct timespec req;
	// req.tv_sec = 0;
	// req.tv_nsec = 1000000; // sleep 5ms at most
	// nanosleep(&req, nullptr);
}

void RendererVU1::DrawLine(const Vector3 &a, const Vector3 &b, const Color &color, RenderingSettings &settings)
{
}

unsigned int RendererVU1::CreateNewTexture()
{
	return 0;
}

void RendererVU1::DeleteTexture(Texture *texture)
{
}

void RendererVU1::SetTextureData(const std::shared_ptr<Texture> &texture, unsigned int textureType, const unsigned char *buffer)
{
}

void RendererVU1::SetLight(int lightIndex, const Vector3 &lightPosition, float intensity, Color color, LightType type, float attenuation)
{
}

void RendererVU1::DisableAllLight()
{
}

void RendererVU1::Setlights(const std::shared_ptr<Camera> &camera)
{
}

void RendererVU1::Clear()
{
	// u64 CLEAR_CLOR = GS_SETREG_RGBAQ(189, 13, 136, 0x00, 0x00);
	// gsKit_clear(gsGlobal, CLEAR_CLOR);
	clear_screen(&frame, &z);
}

void RendererVU1::SetFog(bool m_active)
{
}

void RendererVU1::SetFogValues(float start, float end, const Color &color)
{
}

void RendererVU1::DeleteSubMeshData(MeshData::SubMesh *subMesh)
{
}

void RendererVU1::UploadMeshData(const std::shared_ptr<MeshData> &meshData)
{
}

#endif