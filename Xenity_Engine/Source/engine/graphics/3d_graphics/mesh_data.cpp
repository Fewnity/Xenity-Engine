// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "mesh_data.h"

#include <malloc.h>
#if defined(__PSP__)
#include <pspkernel.h>
#include <vram.h>
#include <pspgu.h>
#endif

#include <engine/graphics/color/color.h>
#include <engine/debug/debug.h>
#include <engine/engine.h>
#include <engine/file_system/mesh_loader/wavefront_loader.h>
#include <engine/file_system/mesh_loader/binary_mesh_loader.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/graphics/renderer/renderer.h>
#include <engine/file_system/async_file_loading.h>
#if defined(_EE)
#include <engine/graphics/renderer/renderer_vu1.h>
#endif
#include <engine/debug/performance.h>
#include <engine/debug/memory_tracker.h>

MeshData::MeshData()
{
}

/**
 * @brief Constructor
 *
 * @param vcount
 * @param index_count
 */
MeshData::MeshData(unsigned int vcount, unsigned int index_count, bool useVertexColor, bool useNormals, bool useUV)
{
	XASSERT(vcount != 0 || index_count != 0, "[MeshData::MeshData] Wrong vertice/index count");

	m_hasUv = useUV;
	m_hasNormal = useNormals;
	m_hasColor = useVertexColor;

	m_vertexDescriptor = (VertexElements)((uint32_t)m_vertexDescriptor | (uint32_t)VertexElements::POSITION_32_BITS);
	if(useUV)
	{
		m_vertexDescriptor = (VertexElements)((uint32_t)m_vertexDescriptor | (uint32_t)VertexElements::UV_32_BITS);
	}
	if (useNormals)
	{
		m_vertexDescriptor = (VertexElements)((uint32_t)m_vertexDescriptor | (uint32_t)VertexElements::NORMAL_32_BITS);
	}
	if (useVertexColor)
	{
		m_vertexDescriptor = (VertexElements)((uint32_t)m_vertexDescriptor | (uint32_t)VertexElements::COLOR);
	}

	AllocSubMesh(vcount, index_count);
}

std::shared_ptr<MeshData> MeshData::MakeMeshData()
{
	std::shared_ptr<MeshData> newFileRef = std::make_shared<MeshData>();
	AssetManager::AddFileReference(newFileRef);
	return newFileRef;
}

std::shared_ptr<MeshData> MeshData::MakeMeshData(unsigned int vcount, unsigned int index_count, bool useVertexColor, bool useNormals, bool useUV)
{
	XASSERT(vcount != 0 || index_count != 0, "[MeshData::MeshData] Wrong vertice/index count");

	std::shared_ptr<MeshData> newFileRef = std::make_shared<MeshData>(vcount, index_count, useVertexColor, useNormals, useUV);
	AssetManager::AddFileReference(newFileRef);
	return newFileRef;
}

ReflectiveData MeshData::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	return reflectedVariables;
}

ReflectiveData MeshData::GetMetaReflectiveData(AssetPlatform platform)
{
	ReflectiveData reflectedVariables;
	return reflectedVariables;
}

/**
 * @brief Add a vertice to the mesh
 *
 * @param u Texture coords U
 * @param v Texture coords V
 * @param color Vertice
 * @param x X position
 * @param y Y position
 * @param z Z position
 * @param index Vertex index
 */
void MeshData::AddVertex(float u, float v, const Color& color, float x, float y, float z, unsigned int index, unsigned int subMeshIndex)
{
	XASSERT(index < m_subMeshes[subMeshIndex]->vertice_count, "[MeshData::AddVertex] Index out of bound");

	Vertex vert;
	vert.u = u;
	vert.v = v;
#if defined(__PSP__)
	vert.color = color.GetUnsignedIntABGR();
#else
	const RGBA& rgba = color.GetRGBA();
	vert.r = rgba.r;
	vert.g = rgba.g;
	vert.b = rgba.b;
	vert.a = rgba.a;
#endif
	vert.x = x;
	vert.y = y;
	vert.z = z;

#if defined(_EE)
	m_subMeshes[subMeshIndex]->c_verts[index][0] = x;
	m_subMeshes[subMeshIndex]->c_verts[index][1] = y;
	m_subMeshes[subMeshIndex]->c_verts[index][2] = z;
	m_subMeshes[subMeshIndex]->c_verts[index][3] = 1;
	m_subMeshes[subMeshIndex]->c_st[index][0] = u;
	m_subMeshes[subMeshIndex]->c_st[index][1] = v;
	m_subMeshes[subMeshIndex]->c_st[index][2] = 1.0f;
	m_subMeshes[subMeshIndex]->c_st[index][3] = 0.0f;
#else
	((Vertex*)m_subMeshes[subMeshIndex]->data)[index] = vert;
#endif
}

void MeshData::AddVertex(float x, float y, float z, unsigned int index, unsigned int subMeshIndex)
{
	XASSERT(index < m_subMeshes[subMeshIndex]->vertice_count, "[MeshData::AddVertex] Index out of bound");

	VertexNoColorNoUv vert;
	vert.x = x;
	vert.y = y;
	vert.z = z;
#if defined(_EE)
	m_subMeshes[subMeshIndex]->c_verts[index][0] = x;
	m_subMeshes[subMeshIndex]->c_verts[index][1] = y;
	m_subMeshes[subMeshIndex]->c_verts[index][2] = z;
	m_subMeshes[subMeshIndex]->c_verts[index][3] = 1;
#else
	((VertexNoColorNoUv*)m_subMeshes[subMeshIndex]->data)[index] = vert;
#endif
}

void MeshData::AddVertex(float u, float v, float x, float y, float z, unsigned int index, unsigned int subMeshIndex)
{
	XASSERT(index < m_subMeshes[subMeshIndex]->vertice_count, "[MeshData::AddVertex] Index out of bound");

	VertexNoColor vert;
	vert.u = u;
	vert.v = v;
	vert.x = x;
	vert.y = y;
	vert.z = z;
#if defined(_EE)
	m_subMeshes[subMeshIndex]->c_verts[index][0] = x;
	m_subMeshes[subMeshIndex]->c_verts[index][1] = y;
	m_subMeshes[subMeshIndex]->c_verts[index][2] = z;
	m_subMeshes[subMeshIndex]->c_verts[index][3] = 1;

	m_subMeshes[subMeshIndex]->c_st[index][0] = u;
	m_subMeshes[subMeshIndex]->c_st[index][1] = v;
	m_subMeshes[subMeshIndex]->c_st[index][2] = 1.0f;
	m_subMeshes[subMeshIndex]->c_st[index][3] = 0.0f;

	m_subMeshes[subMeshIndex]->c_colours[index][0] = 1.0f;
	m_subMeshes[subMeshIndex]->c_colours[index][1] = 1.0f;
	m_subMeshes[subMeshIndex]->c_colours[index][2] = 1.0f;
	m_subMeshes[subMeshIndex]->c_colours[index][3] = 1.0f;
#else
	((VertexNoColor*)m_subMeshes[subMeshIndex]->data)[index] = vert;
#endif
}

void MeshData::AddVertex(float u, float v, float nx, float ny, float nz, float x, float y, float z, unsigned int index, unsigned int subMeshIndex)
{
	XASSERT(index < m_subMeshes[subMeshIndex]->vertice_count, "[MeshData::AddVertex] Index out of bound");

	VertexNormalsNoColor vert;
	vert.u = u;
	vert.v = v;
	vert.normX = nx;
	vert.normY = ny;
	vert.normZ = nz;
	vert.x = x;
	vert.y = y;
	vert.z = z;
#if defined(_EE)
	m_subMeshes[subMeshIndex]->c_verts[index][0] = x;
	m_subMeshes[subMeshIndex]->c_verts[index][1] = y;
	m_subMeshes[subMeshIndex]->c_verts[index][2] = z;
	m_subMeshes[subMeshIndex]->c_verts[index][3] = 1;

	m_subMeshes[subMeshIndex]->c_st[index][0] = u;
	m_subMeshes[subMeshIndex]->c_st[index][1] = v;
	m_subMeshes[subMeshIndex]->c_st[index][2] = 1.0f;
	m_subMeshes[subMeshIndex]->c_st[index][3] = 0.0f;

	m_subMeshes[subMeshIndex]->c_colours[index][0] = 1.0f;
	m_subMeshes[subMeshIndex]->c_colours[index][1] = 1.0f;
	m_subMeshes[subMeshIndex]->c_colours[index][2] = 1.0f;
	m_subMeshes[subMeshIndex]->c_colours[index][3] = 1.0f;
#else
	((VertexNormalsNoColor*)m_subMeshes[subMeshIndex]->data)[index] = vert;
#endif
}

void MeshData::AddVertex(float nx, float ny, float nz, float x, float y, float z, unsigned int index, unsigned int subMeshIndex)
{
	XASSERT(index < m_subMeshes[subMeshIndex]->vertice_count, "[MeshData::AddVertex] Index out of bound");

	VertexNormalsNoColorNoUv vert;
	vert.normX = nx;
	vert.normY = ny;
	vert.normZ = nz;
	vert.x = x;
	vert.y = y;
	vert.z = z;

#if defined(_EE)
	m_subMeshes[subMeshIndex]->c_verts[index][0] = x;
	m_subMeshes[subMeshIndex]->c_verts[index][1] = y;
	m_subMeshes[subMeshIndex]->c_verts[index][2] = z;
	m_subMeshes[subMeshIndex]->c_verts[index][3] = 1;

	m_subMeshes[subMeshIndex]->c_st[index][2] = 1.0f;
	m_subMeshes[subMeshIndex]->c_st[index][3] = 0.0f;
#else
	((VertexNormalsNoColorNoUv*)m_subMeshes[subMeshIndex]->data)[index] = vert;
#endif
}

void MeshData::SendDataToGpu()
{
	Engine::GetRenderer().UploadMeshData(*this);
	//FreeMeshData(false);
}

void MeshData::ComputeBoundingBox()
{
	bool firstValue = true;
	for (int i = 0; i < m_subMeshCount; i++)
	{
		SubMesh* subMesh = m_subMeshes[i];

		int verticesCount = subMesh->vertice_count;
		for (int vertexIndex = 0; vertexIndex < verticesCount; vertexIndex++)
		{
			Vector3 vert;
			if ((uint32_t)m_vertexDescriptor & (uint32_t)VertexElements::NORMAL_32_BITS)
			{
				if ((uint32_t)m_vertexDescriptor & (uint32_t)VertexElements::UV_32_BITS)
				{
					VertexNormalsNoColor& vertex = ((VertexNormalsNoColor*)subMesh->data)[vertexIndex];
					vert = Vector3(vertex.x, vertex.y, vertex.z);
				}
				else 
				{
					VertexNormalsNoColorNoUv& vertex = ((VertexNormalsNoColorNoUv*)subMesh->data)[vertexIndex];
					vert = Vector3(vertex.x, vertex.y, vertex.z);
				}
			}
			else 
			{
				if ((uint32_t)m_vertexDescriptor & (uint32_t)VertexElements::UV_32_BITS)
				{
					VertexNoColor& vertex = ((VertexNoColor*)subMesh->data)[vertexIndex];
					vert = Vector3(vertex.x, vertex.y, vertex.z);
				}
				else
				{
					VertexNoColorNoUv& vertex = ((VertexNoColorNoUv*)subMesh->data)[vertexIndex];
					vert = Vector3(vertex.x, vertex.y, vertex.z);
				}
			}
			
			if (firstValue)
			{
				m_minBoundingBox.x = vert.x;
				m_minBoundingBox.y = vert.y;
				m_minBoundingBox.z = vert.z;

				m_maxBoundingBox.x = vert.x;
				m_maxBoundingBox.y = vert.y;
				m_maxBoundingBox.z = vert.z;
				firstValue = false;
			}
			else
			{
				m_minBoundingBox.x = std::min(m_minBoundingBox.x, vert.x);
				m_minBoundingBox.y = std::min(m_minBoundingBox.y, vert.y);
				m_minBoundingBox.z = std::min(m_minBoundingBox.z, vert.z);

				m_maxBoundingBox.x = std::max(m_maxBoundingBox.x, vert.x);
				m_maxBoundingBox.y = std::max(m_maxBoundingBox.y, vert.y);
				m_maxBoundingBox.z = std::max(m_maxBoundingBox.z, vert.z);
			}
		}
	}
}

void MeshData::ComputeBoundingSphere()
{
	Vector3 spherePosition = (m_minBoundingBox + m_maxBoundingBox) / 2.0f;

	const Vector3 halfDiagonal = (m_maxBoundingBox - m_minBoundingBox) / 2.0f;
	const float sphereRadius = sqrt(halfDiagonal.x * halfDiagonal.x + halfDiagonal.y * halfDiagonal.y + halfDiagonal.z * halfDiagonal.z);

	boundingSphere.position = spherePosition;
	boundingSphere.radius = sphereRadius;
}

void MeshData::Unload()
{
	FreeMeshData(true);
}

void MeshData::FreeMeshData(bool deleteSubMeshes)
{
	for (int i = 0; i < m_subMeshCount; i++)
	{
		SubMesh* subMesh = m_subMeshes[i];
		if (subMesh)
		{
			if (subMesh->data)
			{
#if defined(__PSP__)
				if(isOnVram)
					vfree(subMesh->data);
				else
					free(subMesh->data);
#else
				free(subMesh->data);
#endif
				subMesh->data = nullptr;
			}

			if (subMesh->indices)
			{
				free(subMesh->indices);
				subMesh->indices = nullptr;
			}
#if defined (DEBUG)
			Performance::s_meshDataMemoryTracker->Deallocate(subMesh->debugVertexMemSize);
			Performance::s_meshDataMemoryTracker->Deallocate(subMesh->debugIndexMemSize);
#endif
			if (deleteSubMeshes && Engine::IsRunning(true))
			{
				Engine::GetRenderer().DeleteSubMeshData(*subMesh);
				delete subMesh;
			}
		}
	}

	if (deleteSubMeshes)
	{
		m_subMeshes.clear();
		m_subMeshCount = 0;
	}
}

/**
 * @brief Destructor
 *
 */
MeshData::~MeshData()
{
	Unload();
}

void MeshData::LoadFileReference()
{
	if (m_fileStatus == FileStatus::FileStatus_Not_Loaded)
	{
		m_fileStatus = FileStatus::FileStatus_Loading;
		m_isValid = false;
		bool result;
#if defined(EDITOR)
		result = WavefrontLoader::LoadFromRawData(*this);
#else
		result = BinaryMeshLoader::LoadMesh(*this);
#endif
		if (result) 
		{
			m_fileStatus = FileStatus::FileStatus_Loaded;
		}
		else
		{
			m_fileStatus = FileStatus::FileStatus_Failed;
		}
		OnLoadFileReferenceFinished();
//#if defined(EDITOR)
//		isLoading = true;
//
//		AsyncFileLoading::AddFile(shared_from_this());
//
//		std::thread threadLoading = std::thread(WavefrontLoader::LoadFromRawData, std::dynamic_pointer_cast<MeshData>(shared_from_this()));
//		threadLoading.detach();
//#else
//#endif
	}
}

void MeshData::OnLoadFileReferenceFinished()
{
#if defined(__PSP__)
	if (m_hasIndices)
	{
		pspDrawParam |= GU_INDEX_16BIT;
	}
	pspDrawParam |= GU_TEXTURE_32BITF;
	if (m_hasColor)
	{
		pspDrawParam |= GU_COLOR_8888;
	}
	if ((uint32_t)m_vertexDescriptor & (uint32_t)VertexElements::NORMAL_32_BITS)
	{
		pspDrawParam |= GU_NORMAL_32BITF;
	}
	pspDrawParam |= GU_VERTEX_32BITF;
	pspDrawParam |= GU_TRANSFORM_3D;
#endif

#if defined(__vita__) || defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
	SendDataToGpu();
#endif
	ComputeBoundingBox();
	ComputeBoundingSphere();
	m_isValid = true;
}

void MeshData::UnloadFileReference()
{
	if (Engine::IsRunning(true))
	{
		if (m_fileStatus == FileStatus::FileStatus_Loaded)
		{
			m_fileStatus = FileStatus::FileStatus_Not_Loaded;
			m_isValid = false;
			Unload();
		}
	}
}

void MeshData::UpdatePS2Packets(int index, std::shared_ptr<Texture> texture)
{
#if defined(_EE)
	SubMesh* subMesh = m_subMeshes[index];
	// if (subMesh->meshPacket)
	// packet2_free(subMesh->meshPacket);
	if (!subMesh->meshPacket)
	{
		subMesh->meshPacket = packet2_create(11, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);
		packet2_add_float(subMesh->meshPacket, 2048.0F);				   // scale
		packet2_add_float(subMesh->meshPacket, 2048.0F);				   // scale
		packet2_add_float(subMesh->meshPacket, ((float)0xFFFFFF) / 32.0F); // scale
		packet2_add_s32(subMesh->meshPacket, 36);						   // vertex count
		packet2_utils_gif_add_set(subMesh->meshPacket, 1);
		packet2_utils_gs_add_lod(subMesh->meshPacket, &((RendererVU1&)Engine::GetRenderer()).lod);
		packet2_utils_gs_add_texbuff_clut(subMesh->meshPacket, &texture->texbuff, &((RendererVU1&)Engine::GetRenderer()).clut);
		packet2_utils_gs_add_prim_giftag(subMesh->meshPacket, &((RendererVU1&)Engine::GetRenderer()).prim, 36, DRAW_STQ2_REGLIST, 3, 0);
		// RGBA
		packet2_add_u32(subMesh->meshPacket, 128);
		packet2_add_u32(subMesh->meshPacket, 128);
		packet2_add_u32(subMesh->meshPacket, 128);
		packet2_add_u32(subMesh->meshPacket, 128);
	}
#endif
}

void MeshData::AllocSubMesh(unsigned int vcount, unsigned int index_count)
{
	XASSERT(vcount != 0 || index_count != 0, "[MeshData::AllocSubMesh] vcount and index_count are 0");

	MeshData::SubMesh* newSubMesh = new MeshData::SubMesh();
	newSubMesh->meshData = this;
	if (index_count != 0 && m_hasIndices) 
	{
		newSubMesh->indexMemSize = sizeof(unsigned short) * index_count;
#if defined(__PSP__)
		newSubMesh->indices = (unsigned short*)memalign(16, newSubMesh->indexMemSize);
#else
		newSubMesh->indices = (unsigned short*)malloc(newSubMesh->indexMemSize);
#endif

#if defined (DEBUG)
		Performance::s_meshDataMemoryTracker->Allocate(newSubMesh->indexMemSize);
		newSubMesh->debugIndexMemSize = newSubMesh->indexMemSize;
#endif

		if (newSubMesh->indices == nullptr)
		{
			Debug::PrintError("[MeshData::AllocSubMesh] No memory for Indices", true);
			delete newSubMesh;
			return;
		}
	}

	// Add position size
	if ((uint32_t)m_vertexDescriptor & (uint32_t)VertexElements::POSITION_32_BITS)
	{
		newSubMesh->vertexMemSize += sizeof(float[3]);
	}
	else if ((uint32_t)m_vertexDescriptor & (uint32_t)VertexElements::POSITION_16_BITS)
	{
		newSubMesh->vertexMemSize += sizeof(short[3]);
	}

	// Add normal size
	if ((uint32_t)m_vertexDescriptor & (uint32_t)VertexElements::NORMAL_32_BITS)
	{
		newSubMesh->vertexMemSize += sizeof(float[3]);
	}
	else if ((uint32_t)m_vertexDescriptor & (uint32_t)VertexElements::NORMAL_16_BITS)
	{
		newSubMesh->vertexMemSize += sizeof(short[3]);
	}
	else if ((uint32_t)m_vertexDescriptor & (uint32_t)VertexElements::NORMAL_8_BITS)
	{
		newSubMesh->vertexMemSize += sizeof(char[3]);
	}

	// Add UV size
	if ((uint32_t)m_vertexDescriptor & (uint32_t)VertexElements::UV_32_BITS)
	{
		newSubMesh->vertexMemSize += sizeof(float[2]);
	}
	else if ((uint32_t)m_vertexDescriptor & (uint32_t)VertexElements::UV_16_BITS)
	{
		newSubMesh->vertexMemSize += sizeof(short[2]);
	}

	if ((uint32_t)m_vertexDescriptor & (uint32_t)VertexElements::COLOR)
	{
		newSubMesh->vertexMemSize += sizeof(float[4]);
	}

	newSubMesh->vertexMemSize *= vcount;

	// Allocate memory for mesh data
#if defined(__PSP__)
	isOnVram = true;

	newSubMesh->data = (void*)vramalloc(newSubMesh->vertexMemSize);
	if (!newSubMesh->data)
	{
		isOnVram = false;
		newSubMesh->data = (void*)memalign(16, newSubMesh->vertexMemSize);
	}
#elif defined(_EE)
	newSubMesh->c_verts = (VECTOR*)memalign(128, sizeof(VECTOR) * vcount);
	newSubMesh->c_colours = (VECTOR*)memalign(128, sizeof(VECTOR) * vcount);
	newSubMesh->c_st = (VECTOR*)memalign(128, sizeof(VECTOR) * vcount);
	// RendererVU1((RendererVU1 &)Engine::GetRenderer())
	// newSubMesh->meshPacket = packet2_create(10, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);

	// newSubMesh->meshPacket = packet2_create(11, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);
	// packet2_add_float(newSubMesh->meshPacket, 2048.0F);					  // scale
	// packet2_add_float(newSubMesh->meshPacket, 2048.0F);					  // scale
	// packet2_add_float(newSubMesh->meshPacket, ((float)0xFFFFFF) / 32.0F); // scale
	// packet2_add_s32(newSubMesh->meshPacket, 255);						  // vertex count
	// packet2_utils_gif_add_set(newSubMesh->meshPacket, 1);
	// packet2_utils_gs_add_lod(newSubMesh->meshPacket, &((RendererVU1 &)Engine::GetRenderer()).lod);
	// packet2_utils_gs_add_texbuff_clut(newSubMesh->meshPacket, &((RendererVU1 &)Engine::GetRenderer()).texbuff, &((RendererVU1 &)Engine::GetRenderer()).clut);
	// packet2_utils_gs_add_prim_giftag(newSubMesh->meshPacket, &((RendererVU1 &)Engine::GetRenderer()).prim, 255, DRAW_STQ2_REGLIST, 3, 0);
	// // RGBA
	// packet2_add_u32(newSubMesh->meshPacket, 128);
	// packet2_add_u32(newSubMesh->meshPacket, 128);
	// packet2_add_u32(newSubMesh->meshPacket, 128);
	// packet2_add_u32(newSubMesh->meshPacket, 128);

#else
	newSubMesh->data = (void*)malloc(newSubMesh->vertexMemSize);
#endif

#if defined (DEBUG)
	Performance::s_meshDataMemoryTracker->Allocate(newSubMesh->vertexMemSize);
	newSubMesh->debugVertexMemSize = newSubMesh->vertexMemSize;
#endif

#if !defined(_EE)
	if (newSubMesh->data == nullptr)
	{
		Debug::PrintWarning("[MeshData::AllocSubMesh] No memory for Vertex", true);
		free(newSubMesh->indices);
		delete newSubMesh;
		return;
	}
#else
	if (newSubMesh->c_verts == nullptr || newSubMesh->c_colours == nullptr || newSubMesh->c_st == nullptr)
	{
		Debug::PrintWarning("[MeshData::AllocSubMesh] No ps2 memory for Vertex", true);
		free(newSubMesh->c_verts);
		free(newSubMesh->c_colours);
		free(newSubMesh->c_st);
		delete newSubMesh;
		return;
	}
#endif
	newSubMesh->index_count = index_count;
	newSubMesh->vertice_count = vcount;

	m_subMeshes.push_back(newSubMesh);
	m_subMeshCount++;
}
