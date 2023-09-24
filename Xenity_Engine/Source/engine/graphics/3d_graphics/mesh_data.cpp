#include "mesh_data.h"
#include <malloc.h>
#include "../color/color.h"
#include "../../debug/debug.h"
#include "../../engine.h"

#ifdef __PSP__
#include <pspkernel.h>
#include <vram.h>
#endif
#include "../../file_system/mesh_loader/wavefront_loader.h"
#include "../../asset_management/asset_manager.h"
#include "../../graphics/renderer/renderer.h"

#include <glad/glad.h>

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

std::unordered_map<std::string, ReflectionEntry> MeshData::GetReflection()
{
	std::unordered_map<std::string, ReflectionEntry> reflectedVariables;
	Reflection::AddReflectionVariable(reflectedVariables, fileId, "fileId", true);
	return reflectedVariables;
}


std::unordered_map<std::string, ReflectionEntry> MeshData::GetMetaReflection()
{
	std::unordered_map<std::string, ReflectionEntry> reflectedVariables;
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
#ifdef __PSP__
	vert.color = color.GetUnsignedIntABGR();
#else
	RGBA rgba = color.GetRGBA();
	vert.r = rgba.r;
	vert.g = rgba.g;
	vert.b = rgba.b;
	vert.a = rgba.a;
#endif
	vert.x = x;
	vert.y = y;
	vert.z = z;

	((Vertex*)subMeshes[subMeshIndex]->data)[index] = vert;
}

void MeshData::AddVertex(float x, float y, float z, int index, int subMeshIndex)
{
	VertexNoColorNoUv vert;
	vert.x = x;
	vert.y = y;
	vert.z = z;

	((VertexNoColorNoUv*)subMeshes[subMeshIndex]->data)[index] = vert;
}

void MeshData::AddVertex(float u, float v, float x, float y, float z, int index, int subMeshIndex)
{
	VertexNoColor vert;
	vert.u = u;
	vert.v = v;
	vert.x = x;
	vert.y = y;
	vert.z = z;

	((VertexNoColor*)subMeshes[subMeshIndex]->data)[index] = vert;
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

	((VertexNormalsNoColor*)subMeshes[subMeshIndex]->data)[index] = vert;
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

	((VertexNormalsNoColorNoUv*)subMeshes[subMeshIndex]->data)[index] = vert;
}

void MeshData::SendDataToGpu()
{
	Engine::renderer->UploadMeshData(std::dynamic_pointer_cast<MeshData>(shared_from_this()));
}

void MeshData::Unload()
{
	for (int i = 0; i < subMeshCount; i++)
	{
		SubMesh* subMesh = subMeshes[i];
		if (subMesh)
		{
			if (subMesh->data)
				free(subMesh->data);
			if (subMesh->indices)
				free(subMesh->indices);
		}
	}
	subMeshes.clear();
	subMeshCount = 0;
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

#if defined(EDITOR)
		WavefrontLoader::LoadFromRawData(std::dynamic_pointer_cast<MeshData>(shared_from_this()));
		//std::thread threadLoading = std::thread(WavefrontLoader::LoadFromRawData, std::dynamic_pointer_cast<MeshData>(shared_from_this()));
		//threadLoading.detach();
#else
		WavefrontLoader::LoadFromRawData(std::dynamic_pointer_cast<MeshData>(shared_from_this()));
#endif
	}
}

void MeshData::OnLoadFileReferenceFinished()
{
	isValid = true;
}

void MeshData::UnloadFileReference()
{
	if (Engine::IsRunning())
	{
		if (isLoaded)
		{
			isLoaded = false;
			isValid = false;
			Unload();
		}
	}
}

void MeshData::AllocSubMesh(unsigned int vcount, unsigned int index_count)
{
	MeshData::SubMesh* newSubMesh = new MeshData::SubMesh();

#ifdef __PSP__
	newSubMesh->indices = (unsigned short*)memalign(16, sizeof(unsigned short) * index_count);
#else
	newSubMesh->indices = (unsigned short*)malloc(sizeof(unsigned short) * index_count);
#endif
	if (newSubMesh->indices == nullptr)
	{
		Debug::PrintError("No memory for Indices");
		delete newSubMesh;
		return;
	}

	// Allocate memory for mesh data
#ifdef __PSP__
	if (!hasNormal)
	{
		if (!hasUv)
			newSubMesh->data = (VertexNoColorNoUv*)memalign(16, sizeof(VertexNoColorNoUv) * vcount);
		else
			newSubMesh->data = (VertexNoColor*)memalign(16, sizeof(VertexNoColor) * vcount);
	}
	else
	{
		if (!hasUv)
			newSubMesh->data = (VertexNormalsNoColorNoUv*)memalign(16, sizeof(VertexNormalsNoColorNoUv) * vcount);
		else
			newSubMesh->data = (VertexNormalsNoColor*)memalign(16, sizeof(VertexNormalsNoColor) * vcount);
	}
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
	if (newSubMesh->data == nullptr)
	{
		Debug::PrintWarning("No memory for Vertex");
		free(newSubMesh->indices);
		delete newSubMesh;
		return;
	}

	newSubMesh->index_count = index_count;
	newSubMesh->vertice_count = vcount;

	subMeshes.push_back(newSubMesh);
	subMeshCount++;

	newSubMesh->VBO = Engine::renderer->CreateBuffer();
	newSubMesh->EBO = Engine::renderer->CreateBuffer();
}
