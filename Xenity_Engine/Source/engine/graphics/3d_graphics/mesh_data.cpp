#include "mesh_data.h"
#include <malloc.h>
#include <engine/graphics/color/color.h>
#include <engine/debug/debug.h>
#include <engine/engine.h>

#if defined(__PSP__)
#include <pspkernel.h>
#include <vram.h>
#include <pspgu.h>
#endif

#include <engine/file_system/mesh_loader/wavefront_loader.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/graphics/renderer/renderer.h>
#include <engine/file_system/async_file_loading.h>

#if defined(_EE)
#include <engine/graphics/renderer/renderer_vu1.h>
#endif

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
	this->hasUv = useUV;
	this->hasNormal = useNormals;
	this->hasColor = useVertexColor;

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
	std::shared_ptr<MeshData> newFileRef = std::make_shared<MeshData>(vcount, index_count, useVertexColor, useNormals, useUV);
	AssetManager::AddFileReference(newFileRef);
	return newFileRef;
}

ReflectiveData MeshData::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	return reflectedVariables;
}

ReflectiveData MeshData::GetMetaReflectiveData()
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
void MeshData::AddVertex(float u, float v, const Color& color, float x, float y, float z, int index, int subMeshIndex)
{
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
	subMeshes[subMeshIndex]->c_verts[index][0] = x;
	subMeshes[subMeshIndex]->c_verts[index][1] = y;
	subMeshes[subMeshIndex]->c_verts[index][2] = z;
	subMeshes[subMeshIndex]->c_verts[index][3] = 1;
	subMeshes[subMeshIndex]->c_st[index][0] = u;
	subMeshes[subMeshIndex]->c_st[index][1] = v;
	subMeshes[subMeshIndex]->c_st[index][2] = 1.0f;
	subMeshes[subMeshIndex]->c_st[index][3] = 0.0f;

#else
	((Vertex*)subMeshes[subMeshIndex]->data)[index] = vert;
#endif
}

void MeshData::AddVertex(float x, float y, float z, int index, int subMeshIndex)
{
	VertexNoColorNoUv vert;
	vert.x = x;
	vert.y = y;
	vert.z = z;
#if defined(_EE)
	subMeshes[subMeshIndex]->c_verts[index][0] = x;
	subMeshes[subMeshIndex]->c_verts[index][1] = y;
	subMeshes[subMeshIndex]->c_verts[index][2] = z;
	subMeshes[subMeshIndex]->c_verts[index][3] = 1;
#else
	((VertexNoColorNoUv*)subMeshes[subMeshIndex]->data)[index] = vert;
#endif
}

void MeshData::AddVertex(float u, float v, float x, float y, float z, int index, int subMeshIndex)
{
	VertexNoColor vert;
	vert.u = u;
	vert.v = v;
	vert.x = x;
	vert.y = y;
	vert.z = z;
#if defined(_EE)
	subMeshes[subMeshIndex]->c_verts[index][0] = x;
	subMeshes[subMeshIndex]->c_verts[index][1] = y;
	subMeshes[subMeshIndex]->c_verts[index][2] = z;
	subMeshes[subMeshIndex]->c_verts[index][3] = 1;

	subMeshes[subMeshIndex]->c_st[index][0] = u;
	subMeshes[subMeshIndex]->c_st[index][1] = v;
	subMeshes[subMeshIndex]->c_st[index][2] = 1.0f;
	subMeshes[subMeshIndex]->c_st[index][3] = 0.0f;

	subMeshes[subMeshIndex]->c_colours[index][0] = 1.0f;
	subMeshes[subMeshIndex]->c_colours[index][1] = 1.0f;
	subMeshes[subMeshIndex]->c_colours[index][2] = 1.0f;
	subMeshes[subMeshIndex]->c_colours[index][3] = 1.0f;
#else
	((VertexNoColor*)subMeshes[subMeshIndex]->data)[index] = vert;
#endif
}

void MeshData::AddVertex(float u, float v, float nx, float ny, float nz, float x, float y, float z, int index, int subMeshIndex)
{
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
	subMeshes[subMeshIndex]->c_verts[index][0] = x;
	subMeshes[subMeshIndex]->c_verts[index][1] = y;
	subMeshes[subMeshIndex]->c_verts[index][2] = z;
	subMeshes[subMeshIndex]->c_verts[index][3] = 1;

	subMeshes[subMeshIndex]->c_st[index][0] = u;
	subMeshes[subMeshIndex]->c_st[index][1] = v;
	subMeshes[subMeshIndex]->c_st[index][2] = 1.0f;
	subMeshes[subMeshIndex]->c_st[index][3] = 0.0f;

	subMeshes[subMeshIndex]->c_colours[index][0] = 1.0f;
	subMeshes[subMeshIndex]->c_colours[index][1] = 1.0f;
	subMeshes[subMeshIndex]->c_colours[index][2] = 1.0f;
	subMeshes[subMeshIndex]->c_colours[index][3] = 1.0f;
#else
	((VertexNormalsNoColor*)subMeshes[subMeshIndex]->data)[index] = vert;
#endif
}

void MeshData::AddVertex(float nx, float ny, float nz, float x, float y, float z, int index, int subMeshIndex)
{
	VertexNormalsNoColorNoUv vert;
	vert.normX = nx;
	vert.normY = ny;
	vert.normZ = nz;
	vert.x = x;
	vert.y = y;
	vert.z = z;

#if defined(_EE)
	subMeshes[subMeshIndex]->c_verts[index][0] = x;
	subMeshes[subMeshIndex]->c_verts[index][1] = y;
	subMeshes[subMeshIndex]->c_verts[index][2] = z;
	subMeshes[subMeshIndex]->c_verts[index][3] = 1;

	subMeshes[subMeshIndex]->c_st[index][2] = 1.0f;
	subMeshes[subMeshIndex]->c_st[index][3] = 0.0f;
#else
	((VertexNormalsNoColorNoUv*)subMeshes[subMeshIndex]->data)[index] = vert;
#endif
}

void MeshData::SendDataToGpu()
{
	Engine::GetRenderer().UploadMeshData(std::dynamic_pointer_cast<MeshData>(shared_from_this()));
	//FreeMeshData(false);
}

void MeshData::ComputeBoundingBox()
{
	bool firstValue = true;
	for (int i = 0; i < subMeshCount; i++)
	{
		SubMesh* subMesh = subMeshes[i];

		int verticesCount = subMesh->vertice_count;
		for (int vertexIndex = 0; vertexIndex < verticesCount; vertexIndex++)
		{
			Vector3 vert;
			if (!hasNormal)
			{
				if (!hasUv)
				{
					VertexNoColorNoUv& vertex = ((VertexNoColorNoUv*)subMesh->data)[vertexIndex];
					vert = Vector3(vertex.x, vertex.y, vertex.z);
				}
				else
				{
					VertexNoColor& vertex = ((VertexNoColor*)subMesh->data)[vertexIndex];
					vert = Vector3(vertex.x, vertex.y, vertex.z);
				}
			}
			else
			{
				if (!hasUv)
				{
					VertexNormalsNoColorNoUv& vertex = ((VertexNormalsNoColorNoUv*)subMesh->data)[vertexIndex];
					vert = Vector3(vertex.x, vertex.y, vertex.z);
				}
				else
				{
					VertexNormalsNoColor& vertex = ((VertexNormalsNoColor*)subMesh->data)[vertexIndex];
					vert = Vector3(vertex.x, vertex.y, vertex.z);
				}
			}
			if (firstValue)
			{
				minBoundingBox.x = vert.x;
				minBoundingBox.y = vert.y;
				minBoundingBox.z = vert.z;

				maxBoundingBox.x = vert.x;
				maxBoundingBox.y = vert.y;
				maxBoundingBox.z = vert.z;
				firstValue = false;
			}
			else
			{
				minBoundingBox.x = std::min(minBoundingBox.x, vert.x);
				minBoundingBox.y = std::min(minBoundingBox.y, vert.y);
				minBoundingBox.z = std::min(minBoundingBox.z, vert.z);

				maxBoundingBox.x = std::max(maxBoundingBox.x, vert.x);
				maxBoundingBox.y = std::max(maxBoundingBox.y, vert.y);
				maxBoundingBox.z = std::max(maxBoundingBox.z, vert.z);
			}
		}
	}
}

void MeshData::Unload()
{
	FreeMeshData(true);
}

void MeshData::FreeMeshData(bool deleteSubMeshes)
{
	for (int i = 0; i < subMeshCount; i++)
	{
		SubMesh* subMesh = subMeshes[i];
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
			if (deleteSubMeshes && Engine::IsRunning(true))
			{
				Engine::GetRenderer().DeleteSubMeshData(subMesh);
				delete subMesh;
			}
		}
	}

	if (deleteSubMeshes)
	{
		subMeshes.clear();
		subMeshCount = 0;
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
	if (!isLoaded)
	{
		isLoaded = true;
		isValid = false;

		WavefrontLoader::LoadFromRawData(std::dynamic_pointer_cast<MeshData>(shared_from_this()));

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
	if (hasIndices)
	{
		pspDrawParam |= GU_INDEX_16BIT;
	}
	pspDrawParam |= GU_TEXTURE_32BITF;
	if (hasColor)
	{
		pspDrawParam |= GU_COLOR_8888;
	}
	if (hasNormal)
	{
		pspDrawParam |= GU_NORMAL_32BITF;
	}
	pspDrawParam |= GU_VERTEX_32BITF;
	pspDrawParam |= GU_TRANSFORM_3D;
#endif

#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
	SendDataToGpu();
#endif
	ComputeBoundingBox();
	isValid = true;
}

void MeshData::UnloadFileReference()
{
	if (Engine::IsRunning(true))
	{
		if (isLoaded)
		{
			isLoaded = false;
			isValid = false;
			Unload();
		}
	}
}

void MeshData::UpdatePS2Packets(int index, std::shared_ptr<Texture> texture)
{
#if defined(_EE)
	SubMesh* subMesh = subMeshes[index];
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
	DXASSERT(vcount != 0 || index_count != 0, "[MeshData::AllocSubMesh] vcount and index_count are 0")

	MeshData::SubMesh* newSubMesh = new MeshData::SubMesh();
	newSubMesh->meshData = this;
	if (index_count != 0 && hasIndices) 
	{
#if defined(__PSP__)
		newSubMesh->indices = (unsigned short*)memalign(16, sizeof(unsigned short) * index_count);
#else
		newSubMesh->indices = (unsigned short*)malloc(sizeof(unsigned short) * index_count);
#endif
		if (newSubMesh->indices == nullptr)
		{
			Debug::PrintError("[MeshData::AllocSubMesh] No memory for Indices", true);
			delete newSubMesh;
			return;
		}
	}
	// Allocate memory for mesh data
#if defined(__PSP__)
	isOnVram = true;
	if (!hasNormal)
	{
		if (!hasUv)
		{
			newSubMesh->data = (VertexNoColorNoUv*)vramalloc(vcount * sizeof(VertexNoColorNoUv));
			if(!newSubMesh->data)
			{
				isOnVram = false;
				newSubMesh->data = (VertexNoColorNoUv*)memalign(16, sizeof(VertexNoColorNoUv) * vcount);
			}
		}
		else
		{
			newSubMesh->data = (VertexNoColor*)vramalloc(vcount * sizeof(VertexNoColor));
			if (!newSubMesh->data)
			{
				isOnVram = false;
				newSubMesh->data = (VertexNoColor*)memalign(16, sizeof(VertexNoColor) * vcount);
			}
		}
	}
	else
	{
		if (!hasUv)
		{
			newSubMesh->data = (VertexNormalsNoColorNoUv*)vramalloc(vcount * sizeof(VertexNormalsNoColorNoUv));
			if (!newSubMesh->data)
			{
				isOnVram = false;
				newSubMesh->data = (VertexNormalsNoColorNoUv*)memalign(16, sizeof(VertexNormalsNoColorNoUv) * vcount);
			}
		}
		else
		{
			newSubMesh->data = (VertexNormalsNoColor*)vramalloc(vcount * sizeof(VertexNormalsNoColor));
			if (!newSubMesh->data)
			{
				isOnVram = false;
				newSubMesh->data = (VertexNormalsNoColor*)memalign(16, sizeof(VertexNormalsNoColor) * vcount);
			}
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
	// Debug::Print("A" + std::to_string(vcount));

	// if (!hasNormal)
	//{
	//	if (!hasUv)
	//		newSubMesh->data = (VertexNoColorNoUv*)memalign(16, sizeof(VertexNoColorNoUv) * vcount);
	//	else
	//		newSubMesh->data = (VertexNoColor*)memalign(16, sizeof(VertexNoColor) * vcount);
	// }
	// else
	//{
	//	if (!hasUv)
	//		newSubMesh->data = (VertexNormalsNoColorNoUv*)memalign(16, sizeof(VertexNormalsNoColorNoUv) * vcount);
	//	else
	//		newSubMesh->data = (VertexNormalsNoColor*)memalign(16, sizeof(VertexNormalsNoColor) * vcount);
	// }
#else
	if (!hasNormal)
	{
		if (!hasUv)
			newSubMesh->data = (VertexNoColorNoUv*)malloc(sizeof(VertexNoColorNoUv) * vcount);
		else
			newSubMesh->data = (VertexNoColor*)malloc(sizeof(VertexNoColor) * vcount);
	}
	else
	{
		if (!hasUv)
			newSubMesh->data = (VertexNormalsNoColorNoUv*)malloc(sizeof(VertexNormalsNoColorNoUv) * vcount);
		else
			newSubMesh->data = (VertexNormalsNoColor*)malloc(sizeof(VertexNormalsNoColor) * vcount);
	}
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

	subMeshes.push_back(newSubMesh);
	subMeshCount++;
}
