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
MeshData::MeshData(unsigned int vcount, unsigned int index_count, bool useVertexColor, bool useNormals)
{
	AllocMesh(vcount, index_count, useVertexColor, useNormals);
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
// void MeshData::AddVertex(float u, float v, unsigned int color, float x, float y, float z, int index)
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

void MeshData::Unload()
{
	if (data)
		free(data);
	if (indices)
		free(indices);
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

void MeshData::AllocMesh(unsigned int vcount, unsigned int index_count, bool useVertexColor, bool useNormals)
{
	// Allocate memory for mesh data
	hasColor = useVertexColor;
	hasNormal = useNormals;

#ifdef __PSP__
	if (useVertexColor)
		data = (Vertex*)memalign(16, sizeof(Vertex) * vcount);
	else if (useNormals)
		data = (VertexNormalsNoColor*)memalign(16, sizeof(VertexNormalsNoColor) * vcount);
	else
		data = (VertexNoColor*)memalign(16, sizeof(VertexNoColor) * vcount);
#else
	if (useVertexColor)
		data = (Vertex*)malloc(sizeof(Vertex) * vcount);
	else if (useNormals)
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

	// indices = (unsigned int *)malloc(sizeof(unsigned int) * index_count);
	// indices = (unsigned int *)memalign(16, sizeof(unsigned int) * index_count);
#ifdef __PSP__
	indices = (unsigned short*)memalign(16, sizeof(unsigned short) * index_count);
#else
	indices = (unsigned short*)malloc(sizeof(unsigned short) * index_count);
#endif
	if (indices == nullptr)
	{
		Debug::PrintError("No memory for Indices");
		return;
	}

	this->index_count = index_count;
	this->vertice_count = vcount;
}
