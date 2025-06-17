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
#include <engine/math/vector3.h>
#include <engine/debug/debug.h>
#include <engine/file_system/file_reference.h>
#include <engine/graphics/3d_graphics/sphere.h>
#include <engine/graphics/texture/texture.h>
#include "vertex_descriptor.h"

class API MeshData : public FileReference
{
public:
	class API SubMesh
	{
	public:
		friend MeshData;

		SubMesh() = default;
		void FreeData();

		~SubMesh();

		/**
		 * @brief Set the data of a vertex
		 * @param u U Axis texture coordinate
		 * @param v V Axis texture coordinate
		 * @param color Vertex color
		 * @param x Vertex X position
		 * @param y Vertex Y position
		 * @param z Vertex Z position
		 * @param index Vertex index
		 * @param subMeshIndex Submesh index
		 */
		void SetVertex(float u, float v, const Color& color, float x, float y, float z, uint32_t vertexIndex);

		/**
		 * @brief Set the data of a vertex
		 * @param x Vertex X position
		 * @param y Vertex Y position
		 * @param z Vertex Z position
		 * @param index Vertex index
		 * @param subMeshIndex Submesh index
		 */
		void SetVertex(float x, float y, float z, uint32_t vertexIndex);

		/**
		 * @brief Set the data of a vertex
		 * @param u U Axis texture coordinate
		 * @param v V Axis texture coordinate
		 * @param x Vertex X position
		 * @param y Vertex Y position
		 * @param z Vertex Z position
		 * @param index Vertex index
		 * @param subMeshIndex Submesh index
		 */
		void SetVertex(float u, float v, float x, float y, float z, uint32_t vertexIndex);

		/**
		 * @brief Set the data of a vertex
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
		void SetVertex(float u, float v, float nx, float ny, float nz, float x, float y, float z, uint32_t vertexIndex);

		/**
		 * @brief Set the data of a vertex
		 * @param nx Normal X direction
		 * @param ny Normal Y direction
		 * @param nz Normal Z direction
		 * @param x Vertex X position
		 * @param y Vertex Y position
		 * @param z Vertex Z position
		 * @param index Vertex index
		 * @param subMeshIndex Submesh index
		 */
		void SetVertex(float nx, float ny, float nz, float x, float y, float z, uint32_t vertexIndex);

		/**
		* @brief Set the position of a vertex
		*/
		void SetPosition(float x, float y, float z, uint32_t vertexIndex);

		/**
		* @brief Set the normal of a vertex
		*/
		void SetNormal(float nx, float ny, float nz, uint32_t vertexIndex);

		/**
		* @brief Set the UV coordinates of a vertex
		*/
		void SetUV(float u, float v, uint32_t vertexIndex);

		/**
		* @brief Set the color of a vertex
		*/
		void SetColor(const Color& color, uint32_t vertexIndex);

		/**
		* @brief Set index value
		* @param (value) The new vertex index
		* @param (index) Index to change
		*/
		void SetIndex(size_t index, uint32_t value)
		{
			XASSERT(value < m_vertice_count, "Value is larger than vertex count");
			XASSERT(index < m_index_count, "Value is larger than index count");

			if(usesShortIndices)
			{
				((uint16_t*)m_indices)[index] = static_cast<uint16_t>(value);
			}
			else
			{
				((uint32_t*)m_indices)[index] = value;
			}
		}

		/**
		* @brief Get indices data pointer
		*/
		[[nodiscard]] void* GetIndices() const
		{
			return m_indices;
		}

	private:
		void * m_indices = nullptr;
	public:
		MeshData* m_meshData = nullptr;
		// On PSP, we have to respect a specific order for the data
		// UV Color (uint32_t ARGB) Normal  Position
		void * m_data = nullptr;

		VertexDescriptor m_vertexDescriptor;

		uint32_t m_vertexMemSize = 0;
		uint32_t m_indexMemSize = 0;
		uint32_t m_index_count = 0;
		uint32_t m_vertice_count = 0;

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
#if defined(__PSP__)
		int pspDrawParam = 0;
#endif
#if defined(__PSP__)
		bool isOnVram = true;
#endif
		bool usesShortIndices = true;
		bool m_isQuad = false;
	};

	MeshData();
	~MeshData();

	[[nodiscard]] static std::shared_ptr<MeshData> CreateMeshData();

	Color unifiedColor = Color::CreateFromRGBA(255, 255, 255, 255);

	/**
	* @brief Get the minimum position of the bounding box. (Box including all submeshes).
	*/
	[[nodiscard]] const Vector3& GetMinBoundingBox() const
	{
		return m_minBoundingBox;
	}

	/**
	* @brief Get the maximum position of the bounding box. (Box including all submeshes).
	*/
	[[nodiscard]] const Vector3& GetMaxBoundingBox() const
	{
		return m_maxBoundingBox;
	}

	/**
	* @brief Get mesh bounding sphere
	*/
	[[nodiscard]] const Sphere& GetBoundingSphere() const
	{
		return m_boundingSphere;
	}

	/**
	* @brief Alloc memory for a new submesh
	*/
	void CreateSubMesh(uint32_t vertexCount, uint32_t indexCount, const VertexDescriptor& vertexDescriptorList);

	/**
	* @brief Get submesh pointer
	*/
	[[nodiscard]] const std::unique_ptr<SubMesh>& GetSubMesh(size_t index)
	{
		XASSERT(index < m_subMeshes.size(), "Wrong submesh index");
		return m_subMeshes[index];
	}

	/**
	* @brief Update the mesh after a modification on the submeshes
	*/
	void Update();

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
	friend class ProjectManager;

	Vector3 m_minBoundingBox;
	Vector3 m_maxBoundingBox;

	std::vector<std::unique_ptr<SubMesh>> m_subMeshes;

	/**
	* @brief Send the mesh data to the GPU
	*/
	void SendDataToGpu();

	[[nodiscard]] ReflectiveData GetReflectiveData() override;
	[[nodiscard]] ReflectiveData GetMetaReflectiveData(AssetPlatform platform) override;
	void LoadFileReference(const LoadOptions& loadOptions) override;
	void OnLoadFileReferenceFinished() override;
	void UnloadFileReference() override;
	[[nodiscard]] static std::shared_ptr<MeshData> MakeMeshDataForFile();

	/**
	* @brief Update ps2 packets
	* @param index The index of the submesh
	* @param texture The texture to use
	*/
	void UpdatePS2Packets(int index, std::shared_ptr<Texture> texture);

	uint32_t m_subMeshCount = 0;

	//bool m_hasIndices = true;
	bool m_isValid = true;

	Sphere m_boundingSphere;

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