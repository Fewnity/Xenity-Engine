// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <engine/api.h>

#include <string>

#include <engine/graphics/color/color.h>
#include <engine/vectors/vector3.h>
#include <engine/file_system/file_reference.h>
#include <vector>

#if defined(_EE)
#include <draw3d.h>
#include <packet2.h>
#include <packet2_utils.h>
#endif

struct Vertex
{
	float u, v;
#if defined(__PSP__)
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

class API MeshData : public FileReference, public Reflective
{
public:
	class SubMesh
	{
	public:
		SubMesh() = default;
		unsigned short *indices = nullptr;
		MeshData* meshData = nullptr;
		void *data = nullptr;
		unsigned int index_count = 0;
		unsigned int vertice_count = 0;
#if defined(_EE)
		VECTOR *c_verts = nullptr;
		VECTOR *c_colours = nullptr;
		VECTOR *c_st = nullptr;
		packet2_t *meshPacket = nullptr;
#endif
		unsigned int VBO = 0;
		unsigned int EBO = 0;
		unsigned int VAO = 0;

#if defined(DEBUG)
		size_t debugVertexMemSize = 0;
		size_t debugIndexMemSize = 0;
#endif
	};

	MeshData();
	MeshData(unsigned int vcount, unsigned int index_count, bool useVertexColor, bool useNormals, bool useUV);

	static std::shared_ptr<MeshData> MakeMeshData();
	static std::shared_ptr<MeshData> MakeMeshData(unsigned int vcount, unsigned int index_count, bool useVertexColor, bool useNormals, bool useUV);
	~MeshData();

	/**
	 * @brief Add a vertex to a submesh
	 * @param u U Axis texture coordinate
	 * @param v V Axis texture coordinate
	 * @param color Vertex color
	 * @param x Vertex X position
	 * @param y Vertex Y position
	 * @param z Vertex Z position
	 * @param index Vertex index
	 * @param subMeshIndex Submesh index
	 */
	void AddVertex(float u, float v, const Color &color, float x, float y, float z, unsigned int index, unsigned int subMeshIndex);

	/**
	 * @brief Add a vertex to a submesh
	 * @param x Vertex X position
	 * @param y Vertex Y position
	 * @param z Vertex Z position
	 * @param index Vertex index
	 * @param subMeshIndex Submesh index
	 */
	void AddVertex(float x, float y, float z, unsigned int index, unsigned int subMeshIndex);

	/**
	 * @brief Add a vertex to a submesh
	 * @param u U Axis texture coordinate
	 * @param v V Axis texture coordinate
	 * @param x Vertex X position
	 * @param y Vertex Y position
	 * @param z Vertex Z position
	 * @param index Vertex index
	 * @param subMeshIndex Submesh index
	 */
	void AddVertex(float u, float v, float x, float y, float z, unsigned int index, unsigned int subMeshIndex);

	/**
	 * @brief Add a vertex to a submesh
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
	void AddVertex(float u, float v, float nx, float ny, float nz, float x, float y, float z, unsigned int index, unsigned int subMeshIndex);

	/**
	 * @brief Add a vertex to a submesh
	 * @param nx Normal X direction
	 * @param ny Normal Y direction
	 * @param nz Normal Z direction
	 * @param x Vertex X position
	 * @param y Vertex Y position
	 * @param z Vertex Z position
	 * @param index Vertex index
	 * @param subMeshIndex Submesh index
	 */
	void AddVertex(float nx, float ny, float nz, float x, float y, float z, unsigned int index, unsigned int subMeshIndex);


	Color unifiedColor = Color::CreateFromRGBA(255, 255, 255, 255);

	inline Vector3 GetMinBoundingBox()  const
	{
		return minBoundingBox;
	}

	inline Vector3 GetMaxBoundingBox()  const
	{
		return maxBoundingBox;
	}

protected:
	friend class RendererOpengl;
	friend class RendererGU;
	friend class RendererGsKit;
	friend class RendererVU1;
	friend class WavefrontLoader;
	friend class SpriteManager;
	friend class Tilemap;
	friend class TextManager;
	friend class InspectorMenu;
	friend class MeshRenderer;
	friend class Graphics;
	friend class LineRenderer;
	friend class SceneMenu;
	friend class ParticleSystem;
	friend class MeshManager;

	Vector3 minBoundingBox;
	Vector3 maxBoundingBox;

	std::vector<SubMesh *> subMeshes;

	/**
	* @brief Send the mesh data to the GPU
	*/
	void SendDataToGpu();

	ReflectiveData GetReflectiveData() override;
	ReflectiveData GetMetaReflectiveData() override;
	void LoadFileReference() override;
	void OnLoadFileReferenceFinished() override;
	void UnloadFileReference() override;

	/**
	* @brief Update ps2 packets
	* @param index The index of the submesh
	* @param texture The texture to use
	*/
	void UpdatePS2Packets(int index, std::shared_ptr<Texture> texture);

	/**
	 * @brief Alloc memory for a new submesh
	 */
	void AllocSubMesh(unsigned int vcount, unsigned int index_count);

	int subMeshCount = 0;
	bool hasUv = false;
	bool hasNormal = false;
	bool hasColor = true;
	bool hasIndices = true;
	bool isQuad = false;
	bool isValid = true;

	/**
	* @brief Compute the bounding box of the mesh
	*/
	void ComputeBoundingBox();

#if defined(__PSP__)
	int pspDrawParam = 0;
#endif

#if defined(__PSP__)
	bool isOnVram = true;
#endif
	void Unload();

	/**
	* @brief Free the mesh data
	* @param deleteSubMeshes If the submeshes should be deleted
	*/
	void FreeMeshData(bool deleteSubMeshes);
};