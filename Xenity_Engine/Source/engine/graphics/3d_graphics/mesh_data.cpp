// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "mesh_data.h"

#include <malloc.h>
#if defined(__PSP__)
#include <pspkernel.h>
#include <vram.h>
#include <pspgu.h>
#elif defined(__PS3__)
#include <rsx/rsx.h>
#endif

#include <engine/graphics/color/color.h>
#include <engine/debug/debug.h>
#include <engine/engine.h>
#if defined(EDITOR)
#include <engine/file_system/mesh_loader/wavefront_loader.h>
#include <editor/mesh_loaders/assimp_mesh_loader.h>
#endif
#include <engine/file_system/mesh_loader/binary_mesh_loader.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/graphics/renderer/renderer.h>
#include <engine/file_system/async_file_loading.h>
#if defined(_EE)
#include <engine/graphics/renderer/renderer_vu1.h>
#endif
#include <engine/debug/performance.h>
#include <engine/debug/memory_tracker.h>
#include <engine/debug/stack_debug_object.h>

MeshData::MeshData()
{
}

std::shared_ptr<MeshData> MeshData::MakeMeshData()
{
	std::shared_ptr<MeshData> newFileRef = std::make_shared<MeshData>();
	AssetManager::AddFileReference(newFileRef);
	return newFileRef;
}

ReflectiveData MeshData::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	return reflectedVariables;
}

ReflectiveData MeshData::GetMetaReflectiveData([[maybe_unused]] AssetPlatform platform)
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
void MeshData::AddVertex(float u, float v, const Color& color, float x, float y, float z, unsigned int vertexIndex, unsigned int subMeshIndex)
{
	XASSERT(vertexIndex < m_subMeshes[subMeshIndex]->vertice_count, "[MeshData::AddVertex] vertexIndex out of bound");
	XASSERT(subMeshIndex < m_subMeshCount, "[MeshData::AddVertex] subMeshIndex out of bound");

	AddUV(u, v, vertexIndex, subMeshIndex);
	AddPosition(x, y, z, vertexIndex, subMeshIndex);
	AddColor(color, vertexIndex, subMeshIndex);
}

void MeshData::AddVertex(float x, float y, float z, unsigned int vertexIndex, unsigned int subMeshIndex)
{
	XASSERT(vertexIndex < m_subMeshes[subMeshIndex]->vertice_count, "[MeshData::AddVertex] vertexIndex out of bound");
	XASSERT(subMeshIndex < m_subMeshCount, "[MeshData::AddVertex] subMeshIndex out of bound");

	AddPosition(x, y, z, vertexIndex, subMeshIndex);
}

void MeshData::AddVertex(float u, float v, float x, float y, float z, unsigned int vertexIndex, unsigned int subMeshIndex)
{
	XASSERT(vertexIndex < m_subMeshes[subMeshIndex]->vertice_count, "[MeshData::AddVertex] vertexIndex out of bound");
	XASSERT(subMeshIndex < m_subMeshCount, "[MeshData::AddVertex] subMeshIndex out of bound");

	AddUV(u, v, vertexIndex, subMeshIndex);
	AddPosition(x, y, z, vertexIndex, subMeshIndex);
}

void MeshData::AddVertex(float u, float v, float nx, float ny, float nz, float x, float y, float z, unsigned int vertexIndex, unsigned int subMeshIndex)
{
	XASSERT(vertexIndex < m_subMeshes[subMeshIndex]->vertice_count, "[MeshData::AddVertex] vertexIndex out of bound");
	XASSERT(subMeshIndex < m_subMeshCount, "[MeshData::AddVertex] subMeshIndex out of bound");

	AddUV(u, v, vertexIndex, subMeshIndex);
	AddNormal(nx, ny, nz, vertexIndex, subMeshIndex);
	AddPosition(x, y, z, vertexIndex, subMeshIndex);
}

void MeshData::AddVertex(float nx, float ny, float nz, float x, float y, float z, unsigned int vertexIndex, unsigned int subMeshIndex)
{
	XASSERT(vertexIndex < m_subMeshes[subMeshIndex]->vertice_count, "[MeshData::AddVertex] vertexIndex out of bound");
	XASSERT(subMeshIndex < m_subMeshCount, "[MeshData::AddVertex] subMeshIndex out of bound");

	AddNormal(nx, ny, nz, vertexIndex, subMeshIndex);
	AddPosition(x, y, z, vertexIndex, subMeshIndex);
}

void MeshData::AddPosition(float x, float y, float z, unsigned int vertexIndex, unsigned int subMeshIndex)
{
	XASSERT(vertexIndex < m_subMeshes[subMeshIndex]->vertice_count, "[MeshData::AddPosition] vertexIndex out of bound");
	XASSERT(subMeshIndex < m_subMeshCount, "[MeshData::AddPosition] subMeshIndex out of bound");

	SubMesh* subMesh = m_subMeshes[subMeshIndex].get();
	const VertexDescriptor& vertexDescriptor = subMesh->m_vertexDescriptor;
	char* data = ((char*)subMesh->data) + (vertexIndex * vertexDescriptor.m_vertexSize) + vertexDescriptor.m_vertexElementInfos[vertexDescriptor.m_positionIndex].offset;
	reinterpret_cast<float*>(data)[0] = x;
	reinterpret_cast<float*>(data)[1] = y;
	reinterpret_cast<float*>(data)[2] = z;
}

void MeshData::AddNormal(float nx, float ny, float nz, unsigned int vertexIndex, unsigned int subMeshIndex)
{
	XASSERT(vertexIndex < m_subMeshes[subMeshIndex]->vertice_count, "[MeshData::AddNormal] vertexIndex out of bound");
	XASSERT(subMeshIndex < m_subMeshCount, "[MeshData::AddNormal] subMeshIndex out of bound");

	SubMesh* subMesh = m_subMeshes[subMeshIndex].get();
	const VertexDescriptor& vertexDescriptor = subMesh->m_vertexDescriptor;
	char* data = ((char*)subMesh->data) + (vertexIndex * vertexDescriptor.m_vertexSize) + vertexDescriptor.m_vertexElementInfos[vertexDescriptor.m_normalIndex].offset;
	reinterpret_cast<float*>(data)[0] = nx;
	reinterpret_cast<float*>(data)[1] = ny;
	reinterpret_cast<float*>(data)[2] = nz;
}

void MeshData::AddUV(float u, float v, unsigned int vertexIndex, unsigned int subMeshIndex)
{
	XASSERT(vertexIndex < m_subMeshes[subMeshIndex]->vertice_count, "[MeshData::AddUV] Index out of bound");
	XASSERT(subMeshIndex < m_subMeshCount, "[MeshData::AddUV] subMeshIndex out of bound");

	SubMesh* subMesh = m_subMeshes[subMeshIndex].get();
	const VertexDescriptor& vertexDescriptor = subMesh->m_vertexDescriptor;
	char* data = ((char*)subMesh->data) + (vertexIndex * vertexDescriptor.m_vertexSize) + vertexDescriptor.m_vertexElementInfos[vertexDescriptor.m_uvIndex].offset;
	reinterpret_cast<float*>(data)[0] = u;
	reinterpret_cast<float*>(data)[1] = v;
}

void MeshData::AddColor(const Color& color, unsigned int vertexIndex, unsigned int subMeshIndex)
{
	XASSERT(vertexIndex < m_subMeshes[subMeshIndex]->vertice_count, "[MeshData::AddColor] Index out of bound");
	XASSERT(subMeshIndex < m_subMeshCount, "[MeshData::AddColor] subMeshIndex out of bound");

	Vector4 colorVector = color.GetRGBA().ToVector4();

	SubMesh* subMesh = m_subMeshes[subMeshIndex].get();
	const VertexDescriptor& vertexDescriptor = subMesh->m_vertexDescriptor;
	char* data = ((char*)subMesh->data) + (vertexIndex * vertexDescriptor.m_vertexSize) + vertexDescriptor.m_vertexElementInfos[vertexDescriptor.m_colorIndex].offset;
	reinterpret_cast<float*>(data)[0] = colorVector.x;
	reinterpret_cast<float*>(data)[1] = colorVector.y;
	reinterpret_cast<float*>(data)[2] = colorVector.z;
	reinterpret_cast<float*>(data)[3] = colorVector.w;
}

void MeshData::SendDataToGpu()
{
	Engine::GetRenderer().UploadMeshData(*this);
	//FreeMeshData(false);
}

void MeshData::ComputeBoundingBox()
{
	bool firstValue = true;
	for (uint32_t i = 0; i < m_subMeshCount; i++)
	{
		std::unique_ptr<SubMesh>& subMesh = m_subMeshes[i];

		const uint32_t verticesCount = subMesh->vertice_count;
		for (uint32_t vertexIndex = 0; vertexIndex < verticesCount; vertexIndex++)
		{
			Vector3 vert;
			float* vertexPtr = (float*)((char*)subMesh->data + subMesh->m_vertexDescriptor.m_vertexElementInfos[subMesh->m_vertexDescriptor.m_positionIndex].offset + vertexIndex * subMesh->m_vertexDescriptor.m_vertexSize);
			vert.x = vertexPtr[0];
			vert.y = vertexPtr[1];
			vert.z = vertexPtr[2];

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
	const Vector3 spherePosition = (m_minBoundingBox + m_maxBoundingBox) / 2.0f;

	const Vector3 halfDiagonal = (m_maxBoundingBox - m_minBoundingBox) / 2.0f;
	const float sphereRadius = sqrt(halfDiagonal.x * halfDiagonal.x + halfDiagonal.y * halfDiagonal.y + halfDiagonal.z * halfDiagonal.z);

	m_boundingSphere.position = spherePosition;
	m_boundingSphere.radius = sphereRadius;
}

void MeshData::Unload()
{
	FreeMeshData(true);
}

void MeshData::FreeMeshData(bool deleteSubMeshes)
{
	if (deleteSubMeshes)
	{
		m_subMeshes.clear();
		m_subMeshCount = 0;
	}
	else // Only free data, do not delete sub meshes
	{
		for (uint32_t i = 0; i < m_subMeshCount; i++)
		{
			const std::unique_ptr<SubMesh>& subMesh = m_subMeshes[i];
			if (subMesh)
			{
				subMesh->FreeData();
			}
		}
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

void MeshData::LoadFileReference(const LoadOptions& loadOptions)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	if (m_fileStatus == FileStatus::FileStatus_Not_Loaded)
	{
		m_fileStatus = FileStatus::FileStatus_Loading;
		m_isValid = false;
		bool result;
#if defined(EDITOR)
		AssimpMeshLoader::LoadingOptions options;
		if (loadOptions.platform == Platform::P_PSP)
		{
			options.forceNoIndices = true;
			result = AssimpMeshLoader::LoadMesh(*this, options);
		}
		else
		{
			options.forceNoIndices = false;
			if (loadOptions.platform == Platform::P_PsVita)
			{
				options.forceColors = true;
			}
			result = AssimpMeshLoader::LoadMesh(*this, options);
		}
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

void MeshData::CreateSubMesh(unsigned int vcount, unsigned int index_count, const VertexDescriptor& vertexDescriptorList)
{
	XASSERT(vcount != 0 || index_count != 0, "[MeshData::CreateSubMesh] vcount and index_count are 0");
	XASSERT(vertexDescriptorList.m_vertexSize != 0, "[MeshData::CreateSubMesh] Wrong vertexDescriptorList vertex size");
	XASSERT(vertexDescriptorList.m_vertexElementInfos.size() != 0, "[MeshData::CreateSubMesh] Wrong vertexDescriptorList size");
	XASSERT((m_hasIndices && index_count != 0) || (!m_hasIndices && index_count == 0), "[MeshData::CreateSubMesh] Wrong index count and hasIndice param");

	std::unique_ptr<MeshData::SubMesh> newSubMesh = std::make_unique<MeshData::SubMesh>();
	newSubMesh->m_vertexDescriptor = vertexDescriptorList;
	newSubMesh->meshData = this;
	if (index_count >= std::numeric_limits<unsigned short>::max())
	{
		newSubMesh->isShortIndices = false;
	}
	else
	{
		newSubMesh->isShortIndices = true;
	}

	if (index_count != 0 && m_hasIndices)
	{
		const size_t indexSize = newSubMesh->isShortIndices ? sizeof(unsigned short) : sizeof(unsigned int);
		newSubMesh->indexMemSize = indexSize * index_count;
#if defined(__PSP__)
		newSubMesh->indices = memalign(16, newSubMesh->indexMemSize);
#elif defined(__PS3__)
		newSubMesh->indices = rsxMemalign(128, newSubMesh->indexMemSize);
#else
		newSubMesh->indices = malloc(newSubMesh->indexMemSize);
#endif

#if defined (DEBUG)
		Performance::s_meshDataMemoryTracker->Allocate(newSubMesh->indexMemSize);
		newSubMesh->debugIndexMemSize = newSubMesh->indexMemSize;
#endif

		if (newSubMesh->indices == nullptr)
		{
			Debug::PrintError("[MeshData::CreateSubMesh] No memory for Indices", true);
			return;
		}
	}

	newSubMesh->vertexMemSize = vertexDescriptorList.m_vertexSize * vcount;

	// Allocate memory for mesh data
#if defined(__PSP__)
	newSubMesh->isOnVram = true;

	newSubMesh->data = (void*)vramalloc(newSubMesh->vertexMemSize);
	if (!newSubMesh->data)
	{
		newSubMesh->isOnVram = false;
		newSubMesh->data = (void*)memalign(16, newSubMesh->vertexMemSize);
	}

	// Prepare the draw parameters
	newSubMesh->pspDrawParam |= GU_TRANSFORM_3D;

	if (m_hasIndices)
	{
		newSubMesh->pspDrawParam |= GU_INDEX_16BIT;
	}

	if (vertexDescriptorList.m_uvIndex != -1)
	{
		VertexElements uvElement = vertexDescriptorList.GetElementFromIndex(vertexDescriptorList.m_uvIndex);
		if (uvElement == VertexElements::UV_32_BITS)
		{
			newSubMesh->pspDrawParam |= GU_TEXTURE_32BITF;
		}
		else if (uvElement == VertexElements::UV_16_BITS)
		{
			newSubMesh->pspDrawParam |= GU_TEXTURE_16BIT;
		}
		else if (uvElement == VertexElements::UV_8_BITS)
		{
			newSubMesh->pspDrawParam |= GU_TEXTURE_8BIT;
		}
	}

	if (vertexDescriptorList.m_normalIndex != -1)
	{
		VertexElements normalElement = vertexDescriptorList.GetElementFromIndex(vertexDescriptorList.m_normalIndex);
		if (normalElement == VertexElements::NORMAL_32_BITS)
		{
			newSubMesh->pspDrawParam |= GU_NORMAL_32BITF;
		}
		else if (normalElement == VertexElements::NORMAL_16_BITS)
		{
			newSubMesh->pspDrawParam |= GU_NORMAL_16BIT;
		}
		else if (normalElement == VertexElements::NORMAL_8_BITS)
		{
			newSubMesh->pspDrawParam |= GU_NORMAL_8BIT;
		}
	}

	if (vertexDescriptorList.m_positionIndex != -1)
	{
		VertexElements positionElement = vertexDescriptorList.GetElementFromIndex(vertexDescriptorList.m_positionIndex);
		if (positionElement == VertexElements::POSITION_32_BITS)
		{
			newSubMesh->pspDrawParam |= GU_VERTEX_32BITF;
		}
		else if (positionElement == VertexElements::POSITION_16_BITS)
		{
			newSubMesh->pspDrawParam |= GU_VERTEX_16BIT;
		}
		else if (positionElement == VertexElements::POSITION_8_BITS)
		{
			newSubMesh->pspDrawParam |= GU_VERTEX_8BIT;
		}
	}

	if (vertexDescriptorList.m_colorIndex != -1)
	{
		VertexElements colorElement = vertexDescriptorList.GetElementFromIndex(vertexDescriptorList.m_colorIndex);
		if (colorElement == VertexElements::COLOR_32_BITS_UINT) // Actually wrong
		{
			newSubMesh->pspDrawParam |= GU_COLOR_8888;
		}
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
#elif defined(__PS3__)
	newSubMesh->data = (void*)rsxMemalign(128, newSubMesh->vertexMemSize);
#else
	newSubMesh->data = (void*)malloc(newSubMesh->vertexMemSize);
#endif

#if !defined(_EE)
	if (newSubMesh->data == nullptr)
	{
		Debug::PrintWarning("[MeshData::CreateSubMesh] No memory for Vertex", true);
		return;
	}
#else
	if (newSubMesh->c_verts == nullptr || newSubMesh->c_colours == nullptr || newSubMesh->c_st == nullptr)
	{
		Debug::PrintWarning("[MeshData::CreateSubMesh] No ps2 memory for Vertex", true);
		return;
	}
#endif

#if defined (DEBUG)
	Performance::s_meshDataMemoryTracker->Allocate(newSubMesh->vertexMemSize);
	newSubMesh->debugVertexMemSize = newSubMesh->vertexMemSize;
#endif

	newSubMesh->index_count = index_count;
	newSubMesh->vertice_count = vcount;

#if defined(__PS3__)
	if (newSubMesh->isShortIndices)
	{
		rsxAddressToOffset(&((unsigned short*)newSubMesh->indices)[0], &newSubMesh->indicesOffset);
	}
	else
	{
		rsxAddressToOffset(&((unsigned int*)newSubMesh->indices)[0], &newSubMesh->indicesOffset);
	}

	rsxAddressToOffset((void*)((char*)newSubMesh->data + vertexDescriptorList.m_vertexElementInfos[vertexDescriptorList.m_positionIndex].offset), &newSubMesh->positionOffset);
	if (vertexDescriptorList.m_uvIndex != -1)
	{
		rsxAddressToOffset((void*)((char*)newSubMesh->data + vertexDescriptorList.m_vertexElementInfos[vertexDescriptorList.m_uvIndex].offset), &newSubMesh->uvOffset);
	}

	if (vertexDescriptorList.m_normalIndex != -1)
	{
		rsxAddressToOffset((void*)((char*)newSubMesh->data + vertexDescriptorList.m_vertexElementInfos[vertexDescriptorList.m_normalIndex].offset), &newSubMesh->normalOffset);
	}

	if (vertexDescriptorList.m_colorIndex != -1)
	{
		rsxAddressToOffset((void*)((char*)newSubMesh->data + vertexDescriptorList.m_vertexElementInfos[vertexDescriptorList.m_colorIndex].offset), &newSubMesh->colorOffset);
	}

#endif

	m_subMeshes.push_back(std::move(newSubMesh));
	m_subMeshCount++;
}

void MeshData::SubMesh::FreeData()
{
	//Debug::Print("[MeshData::SubMesh::FreeData] Freeing data");

#if !defined(_EE)
	if (data)
	{
#if defined(__PSP__)
		if (isOnVram)
			vfree(data);
		else
			free(data);
#elif defined(__PS3__)
		rsxFree(data);
#else
		free(data);
#endif
		data = nullptr;
	}

	if (indices)
	{
#if defined(__PS3__)
		rsxFree(indices);
#else
		free(indices);
#endif
		indices = nullptr;
	}
#else
	if (c_verts == nullptr)
	{
		free(c_verts);
	}
	if (c_colours == nullptr)
	{
		free(c_colours);
	}
	if (c_st == nullptr)
	{
		free(c_st);
	}
#endif

#if defined (DEBUG)
	Performance::s_meshDataMemoryTracker->Deallocate(debugVertexMemSize);
	Performance::s_meshDataMemoryTracker->Deallocate(debugIndexMemSize);
#endif
	if (Engine::IsRunning(true))
	{
		Engine::GetRenderer().DeleteSubMeshData(*this);
	}
}

MeshData::SubMesh::~SubMesh()
{
	FreeData();
}
