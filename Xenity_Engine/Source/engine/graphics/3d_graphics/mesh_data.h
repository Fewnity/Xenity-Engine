// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <vector>

#if defined(_EE)
#include <draw3d.h>
#include <packet2.h>
#include <packet2_utils.h>
#endif

#include <engine/api.h>
#include <engine/graphics/color/color.h>
#include <engine/vectors/vector3.h>
#include <engine/file_system/file_reference.h>
#include <engine/graphics/3d_graphics/sphere.h>

enum class VertexElements : uint32_t
{
	NONE = 0,
	POSITION_32_BITS = 1 << 0,
	POSITION_16_BITS = 1 << 1, // Used for PSP
	NORMAL_32_BITS = 1 << 2,
	NORMAL_16_BITS = 1 << 3, // Used for PSP
	NORMAL_8_BITS = 1 << 4, // Used for PSP
	UV_32_BITS = 1 << 5,
	UV_16_BITS = 1 << 6, // Used for PSP
	COLOR = 1 << 7,
};

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
		uint32_t vertexMemSize = 0;
		uint32_t indexMemSize = 0;
#if defined(DEBUG)
		uint32_t debugVertexMemSize = 0;
		uint32_t debugIndexMemSize = 0;
#endif
		uint32_t index_count = 0;
		uint32_t vertice_count = 0;
#if defined(_EE)
		VECTOR *c_verts = nullptr;
		VECTOR *c_colours = nullptr;
		VECTOR *c_st = nullptr;
		packet2_t *meshPacket = nullptr;
#endif
		unsigned int VBO = 0;
		unsigned int EBO = 0;
		unsigned int VAO = 0;

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

	/**
	* Get mesh min bounding box point
	*/
	inline const Vector3& GetMinBoundingBox()  const
	{
		return m_minBoundingBox;
	}

	/**
	* Get mesh max bounding box point
	*/
	inline const Vector3& GetMaxBoundingBox()  const
	{
		return m_maxBoundingBox;
	}

	/**
	* Get mesh bounding sphere
	*/
	const Sphere& GetBoundingSphere() const
	{
		return boundingSphere;
	}

protected:
	friend class RendererOpengl;
	friend class RendererRSX;
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
	friend class Cooker;
	friend class BinaryMeshLoader;

	Vector3 m_minBoundingBox;
	Vector3 m_maxBoundingBox;

	std::vector<SubMesh *> m_subMeshes;

	/**
	* @brief Send the mesh data to the GPU
	*/
	void SendDataToGpu();

	ReflectiveData GetReflectiveData() override;
	ReflectiveData GetMetaReflectiveData(AssetPlatform platform) override;
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

	int m_subMeshCount = 0;
	bool m_hasUv = false;
	bool m_hasNormal = false;
	bool m_hasColor = true;
	bool m_hasIndices = true;
	bool m_isQuad = false;
	bool m_isValid = true;

	Sphere boundingSphere;

	VertexElements m_vertexDescriptor = VertexElements::NONE;

	/**
	* Set mesh vertex descriptor
	*/
	void SetVertexDescriptor(VertexElements vertexDescriptor) 
	{
		m_vertexDescriptor = vertexDescriptor;
	}

	/**
	* @brief Compute the bounding box of the mesh
	*/
	void ComputeBoundingBox();

	/**
	* @brief Compute the bounding spehere of the mesh
	*/
	void ComputeBoundingSphere();

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