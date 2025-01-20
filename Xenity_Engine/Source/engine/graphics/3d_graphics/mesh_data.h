// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <vector>
#include <memory>

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
#include <engine/graphics/texture.h>
#include "vertex_descriptor.h"

class API MeshData : public FileReference
{
public:
	class SubMesh
	{
	public:
		SubMesh() = default;
		void FreeData();

		~SubMesh();
		void *indices = nullptr;
		MeshData* meshData = nullptr;
		void *data = nullptr;

		VertexDescriptorList m_vertexDescriptor;

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

#if defined(__vita__) || defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
		unsigned int VBO = 0;
		unsigned int EBO = 0;
		unsigned int VAO = 0;
#endif
#if defined(__PS3__)
		uint32_t normalOffset = 0;
		uint32_t uvOffset = 0;
		uint32_t positionOffset = 0;
		uint32_t colorOffset = 0;
		uint32_t indicesOffset = 0;
#endif

		uint16_t m_vertexSize = 0;

#if defined(__PSP__)
		bool isOnVram = true;
#endif
		bool isShortIndices = true;
		bool m_isQuad = false;
	};

	MeshData();

	static std::shared_ptr<MeshData> MakeMeshData();
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
	void AddVertex(float u, float v, const Color &color, float x, float y, float z, unsigned int vertexIndex, unsigned int subMeshIndex);

	/**
	 * @brief Add a vertex to a submesh
	 * @param x Vertex X position
	 * @param y Vertex Y position
	 * @param z Vertex Z position
	 * @param index Vertex index
	 * @param subMeshIndex Submesh index
	 */
	void AddVertex(float x, float y, float z, unsigned int vertexIndex, unsigned int subMeshIndex);

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
	void AddVertex(float u, float v, float x, float y, float z, unsigned int vertexIndex, unsigned int subMeshIndex);

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
	void AddVertex(float u, float v, float nx, float ny, float nz, float x, float y, float z, unsigned int vertexIndex, unsigned int subMeshIndex);

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
	void AddVertex(float nx, float ny, float nz, float x, float y, float z, unsigned int vertexIndex, unsigned int subMeshIndex);

	void AddPosition(float x, float y, float z, unsigned int vertexIndex, unsigned int subMeshIndex);
	void AddNormal(float nx, float ny, float nz, unsigned int vertexIndex, unsigned int subMeshIndex);
	void AddUV(float u, float v, unsigned int vertexIndex, unsigned int subMeshIndex);
	void AddColor(const Color& color, unsigned int vertexIndex, unsigned int subMeshIndex);

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
		return m_boundingSphere;
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
	friend class AssimpMeshLoader;

	Vector3 m_minBoundingBox;
	Vector3 m_maxBoundingBox;

	std::vector<std::unique_ptr<SubMesh>> m_subMeshes;

	/**
	* @brief Send the mesh data to the GPU
	*/
	void SendDataToGpu();

	ReflectiveData GetReflectiveData() override;
	ReflectiveData GetMetaReflectiveData(AssetPlatform platform) override;
	void LoadFileReference(const LoadOptions& loadOptions) override;
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
	void AllocSubMesh(unsigned int vcount, unsigned int index_count, const VertexDescriptorList& vertexDescriptorList);

	int m_subMeshCount = 0;

	bool m_hasIndices = true;
	bool m_isValid = true;

	Sphere m_boundingSphere;

#if defined(__PSP__)
	int pspDrawParam = 0;
#endif

	//VertexElements m_vertexDescriptor = VertexElements::NONE;
	//uint16_t m_vertexSize = 0;

	/**
	* Set mesh vertex descriptor
	*/
	void SetVertexDescriptor(VertexElements vertexDescriptor) 
	{
		//m_vertexDescriptor = vertexDescriptor;

		/*if ((m_vertexDescriptor & VertexElements::POSITION_32_BITS) == VertexElements::POSITION_32_BITS)
		{
			m_vertexSize += sizeof(float[3]);
		}
		else if ((m_vertexDescriptor & VertexElements::POSITION_16_BITS) == VertexElements::POSITION_16_BITS)
		{
			m_vertexSize += sizeof(uint16_t[3]);
		}

		if ((m_vertexDescriptor & VertexElements::NORMAL_32_BITS) == VertexElements::NORMAL_32_BITS)
		{
			m_vertexSize += sizeof(float[3]);
		}
		else if ((m_vertexDescriptor & VertexElements::NORMAL_16_BITS) == VertexElements::NORMAL_16_BITS)
		{
			m_vertexSize += sizeof(uint16_t[3]);
		}
		else if ((m_vertexDescriptor & VertexElements::NORMAL_8_BITS) == VertexElements::NORMAL_8_BITS)
		{
			m_vertexSize += sizeof(char[3]);
		}

		if ((m_vertexDescriptor & VertexElements::UV_32_BITS) == VertexElements::UV_32_BITS)
		{
			m_vertexSize += sizeof(float[2]);
		}
		else if ((m_vertexDescriptor & VertexElements::UV_16_BITS) == VertexElements::UV_16_BITS)
		{
			m_vertexSize += sizeof(uint16_t[2]);
		}

		if ((m_vertexDescriptor & VertexElements::COLOR) == VertexElements::COLOR)
		{
#if defined(__PSP__)
			m_vertexSize += sizeof(uint32_t);
#else
			m_vertexSize += sizeof(float[4]);
#endif
		}*/
	}

	/**
	* @brief Compute the bounding box of the mesh
	*/
	void ComputeBoundingBox();

	/**
	* @brief Compute the bounding spehere of the mesh
	*/
	void ComputeBoundingSphere();


	void Unload();

	/**
	* @brief Free the mesh data
	* @param deleteSubMeshes If the submeshes should be deleted
	*/
	void FreeMeshData(bool deleteSubMeshes);
};