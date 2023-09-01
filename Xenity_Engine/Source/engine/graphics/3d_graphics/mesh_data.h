#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include <string>

#include "../color/color.h"
#include "../../file_system/file_reference.h"
#include <vector>

struct Vertex
{
	float u, v;
#ifdef __PSP__
	unsigned int color;
#else
	float r, g, b, a;
#endif
	float x, y, z;
};

struct VertexNoColor
{
	float u, v;
	float x, y, z;
};

struct VertexNoColorNoUv
{
	float x, y, z;
};

struct VertexNormalsNoColor
{
	float u, v;
	float normX, normY, normZ;
	float x, y, z;
};

struct VertexNormalsNoColorNoUv
{
	float normX, normY, normZ;
	float x, y, z;
};

class API MeshData : public FileReference
{
public:
	class SubMesh
	{
	public:
		SubMesh() = default;
		unsigned int index_count = 0;
		unsigned short* indices = nullptr;
		unsigned int vertice_count = 0;
		void* data = nullptr;
	};
	MeshData();
	//MeshData() = delete;
	MeshData(unsigned int vcount, unsigned int index_count, bool useVertexColor, bool useNormals, bool useUV);

	static std::shared_ptr<MeshData> MakeMeshData();
	static std::shared_ptr<MeshData> MakeMeshData(unsigned int vcount, unsigned int index_count, bool useVertexColor, bool useNormals, bool useUV);
	std::unordered_map<std::string, Variable> GetReflection();
	std::unordered_map<std::string, Variable> GetMetaReflection();
	~MeshData();

	void LoadFileReference();

	void UnloadFileReference();

	/**
	* Alloc memory for a new submesh
	*/
	void AllocSubMesh(unsigned int vcount, unsigned int index_count);

	/**
	* Add a vertex to a submesh
	* @param u U Axis texture coordinate
	* @param v V Axis texture coordinate
	* @param color Vertex color
	* @param x Vertex X position
	* @param y Vertex Y position
	* @param z Vertex Z position
	* @param index Vertex index
	* @param subMeshIndex Submesh index
	*/
	void AddVertex(float u, float v, Color color, float x, float y, float z, int index, int subMeshIndex);

	/**
	* Add a vertex to a submesh
	* @param x Vertex X position
	* @param y Vertex Y position
	* @param z Vertex Z position
	* @param index Vertex index
	* @param subMeshIndex Submesh index
	*/
	void AddVertex(float x, float y, float z, int index, int subMeshIndex);

	/**
	* Add a vertex to a submesh
	* @param u U Axis texture coordinate
	* @param v V Axis texture coordinate
	* @param x Vertex X position
	* @param y Vertex Y position
	* @param z Vertex Z position
	* @param index Vertex index
	* @param subMeshIndex Submesh index
	*/
	void AddVertex(float u, float v, float x, float y, float z, int index, int subMeshIndex);

	/**
	* Add a vertex to a submesh
	* @param u U Axis texture coordinate
	* @param v V Axis texture coordinate
	* @param nx Normal X direction
	* @param ny Normal Y direction
	* @param nz Normal Z direction
	* @param x Vertex X position
	* @param y Vertex Y position
	* @param z Vertex Z position
	* @param index Vertex index
	* @param subMeshIndex Submesh index
	*/
	void AddVertex(float u, float v, float nx, float ny, float nz, float x, float y, float z, int index, int subMeshIndex);

	/**
	* Add a vertex to a submesh
	* @param nx Normal X direction
	* @param ny Normal Y direction
	* @param nz Normal Z direction
	* @param x Vertex X position
	* @param y Vertex Y position
	* @param z Vertex Z position
	* @param index Vertex index
	* @param subMeshIndex Submesh index
	*/
	void AddVertex(float nx, float ny, float nz, float x, float y, float z, int index, int subMeshIndex);

	std::vector<SubMesh*> subMeshes;

	int subMeshCount  = 0;
	bool hasUv = false;
	bool hasNormal = false;
	bool hasColor = true;
	bool hasIndices = true;
	bool isQuad = false;
	Color unifiedColor = Color::CreateFromRGBA(255, 255, 255, 255);
private:
	void Unload();
};