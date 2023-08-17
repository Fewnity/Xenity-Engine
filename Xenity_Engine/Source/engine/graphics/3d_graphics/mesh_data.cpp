#include "mesh_data.h"
#include <malloc.h>
#include "../color/color.h"
#include "../../debug/debug.h"

#ifdef __PSP__
#include <pspkernel.h>
#include <vram.h>
#endif
#include "../../file_system/mesh_loader/wavefront_loader.h"

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
	/*this->subMeshData.push_back(new MeshData::Data());
	this->subMeshDataCount++;
	MeshData::SubMesh* newSubMesh = new MeshData::SubMesh();
	newSubMesh->data = this->subMeshData[0];
	newSubMesh->data->hasUv = useUV;
	newSubMesh->data->hasNormal = useNormals;
	newSubMesh->data->hasColor = useVertexColor;
	this->subMeshes.push_back(newSubMesh);
	this->subMeshCount++;

	AllocMesh(vcount, index_count, 0);*/
	AllocData(vcount);
	AllocSubMesh(index_count);
}

// MeshData::MeshData(std::string filePath)
// {
// }

std::unordered_map<std::string, Variable> MeshData::GetReflection()
{
	std::unordered_map<std::string, Variable> reflectedVariables;
	reflectedVariables.insert_or_assign("fileId", fileId);
	return reflectedVariables;
}


std::unordered_map<std::string, Variable> MeshData::GetMetaReflection()
{
	std::unordered_map<std::string, Variable> reflectedVariables;
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
void MeshData::AddVertex(float u, float v, Color color, float x, float y, float z, int index)
{
	RGBA rgba = color.GetRGBA();
	Vertex vert;
	vert.u = u;
	vert.v = v;
#ifdef __PSP__
	vert.color = color.GetUnsignedIntABGR();
#else
	vert.r = rgba.r;
	vert.g = rgba.g;
	vert.b = rgba.b;
	vert.a = rgba.a;
#endif
	vert.x = x;
	vert.y = y;
	vert.z = z;

	((Vertex *)data)[index] = vert;
}

void MeshData::AddVertex(float x, float y, float z, int index)
{
	VertexNoColorNoUv vert;
	vert.x = x;
	vert.y = y;
	vert.z = z;

	((VertexNoColorNoUv*)data)[index] = vert;
}

void MeshData::AddVertex(float u, float v, float x, float y, float z, int index)
{
	VertexNoColor vert;
	vert.u = u;
	vert.v = v;
	vert.x = x;
	vert.y = y;
	vert.z = z;

	((VertexNoColor *)data)[index] = vert;
}

void MeshData::AddVertex(float u, float v, float nx, float ny, float nz, float x, float y, float z, int index)
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

	((VertexNormalsNoColor *)data)[index] = vert;
}

void MeshData::AddVertex(float nx, float ny, float nz, float x, float y, float z, int index)
{
	VertexNormalsNoColorNoUv vert;
	vert.normX = nx;
	vert.normY = ny;
	vert.normZ = nz;
	vert.x = x;
	vert.y = y;
	vert.z = z;

	((VertexNormalsNoColorNoUv*)data)[index] = vert;
}

void MeshData::Unload()
{
	/*if (data)
		free(data);
	if (indices)
		free(indices);*/
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
		WavefrontLoader::LoadFromRawData(this);
	}
}

void MeshData::UnloadFileReference()
{
	Unload();
}

void MeshData::AllocData(unsigned int vcount)
{
#ifdef __PSP__
	if (hasColor)
		data = (Vertex*)memalign(16, sizeof(Vertex) * vcount);
	else if (hasNormal)
		data = (VertexNormalsNoColor*)memalign(16, sizeof(VertexNormalsNoColor) * vcount);
	else
		data = (VertexNoColor*)memalign(16, sizeof(VertexNoColor) * vcount);
#else
	if (hasColor)
		data = (Vertex*)malloc(sizeof(Vertex) * vcount);
	else if (hasNormal)
		data = (VertexNormalsNoColor*)malloc(sizeof(VertexNormalsNoColor) * vcount);
	else
		data = (VertexNoColor*)malloc(sizeof(VertexNoColor) * vcount);
#endif
	// data = (Vertex *)malloc(sizeof(Vertex) * vcount);
	if (data == nullptr)
	{
		Debug::PrintWarning("No memory for Vertex");
		return;
	}

	vertice_count = vcount;
}

void MeshData::AllocSubMesh(unsigned int index_count)
{
	// Allocate memory for mesh data
	//hasColor = useVertexColor;
	//hasNormal = useNormals;

	MeshData::SubMesh* newSubMesh = new MeshData::SubMesh();

#ifdef __PSP__
	newSubMesh->indices = (unsigned short*)memalign(16, sizeof(unsigned short) * index_count);
#else
	newSubMesh->indices = (unsigned short*)malloc(sizeof(unsigned short) * index_count);
#endif
		if (newSubMesh->indices == nullptr)
		{
			Debug::PrintError("No memory for Indices");
			return;
		}
		newSubMesh->index_count = index_count;
	subMeshes.push_back(newSubMesh);
		subMeshCount++;

/*#ifdef __PSP__
	if (hasColor)
		subMeshData[subMeshDataIndex]->data = (Vertex*)memalign(16, sizeof(Vertex) * vcount);
	else if (hasNormal)
		subMeshData[subMeshDataIndex]->data = (VertexNormalsNoColor*)memalign(16, sizeof(VertexNormalsNoColor) * vcount);
	else
		subMeshData[subMeshDataIndex]->data = (VertexNoColor*)memalign(16, sizeof(VertexNoColor) * vcount);
#else
	if (hasColor)
		subMeshData[subMeshDataIndex]->data = (Vertex*)malloc(sizeof(Vertex) * vcount);
	else if (hasNormal)
		subMeshData[subMeshDataIndex]->data = (VertexNormalsNoColor*)malloc(sizeof(VertexNormalsNoColor) * vcount);
	else
		subMeshData[subMeshDataIndex]->data = (VertexNoColor*)malloc(sizeof(VertexNoColor) * vcount);
#endif
	// data = (Vertex *)malloc(sizeof(Vertex) * vcount);
	if (subMeshData[subMeshDataIndex]->data == nullptr)
	{
		Debug::PrintWarning("No memory for Vertex");
		return;
	}

	// indices = (unsigned int *)malloc(sizeof(unsigned int) * index_count);
	// indices = (unsigned int *)memalign(16, sizeof(unsigned int) * index_count);
#ifdef __PSP__
	subMeshData[subMeshDataIndex]->indices = (unsigned short*)memalign(16, sizeof(unsigned short) * index_count);
#else
	subMeshData[subMeshDataIndex]->indices = (unsigned short*)malloc(sizeof(unsigned short) * index_count);
#endif
	if (subMeshData[subMeshDataIndex]->indices == nullptr)
	{
		Debug::PrintError("No memory for Indices");
		return;
	}

	subMeshData[subMeshDataIndex]->index_count = index_count;
	subMeshData[subMeshDataIndex]->vertice_count = vcount;*/
}
