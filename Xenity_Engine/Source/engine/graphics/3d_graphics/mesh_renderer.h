// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <vector>
#include <memory>

#include <engine/api.h>
#include <engine/graphics/iDrawable.h>
#include <engine/graphics/3d_graphics/sphere.h>
#include <engine/world_partitionner/world_partitionner.h>

class MeshData;
class Material;
class Light;

/**
* @brief Component that renders a mesh with materials
*/
class API MeshRenderer : public IDrawable
{
public:
	MeshRenderer();
	~MeshRenderer();

	/**
	* @brief Get mesh data
	*/
	[[nodiscard]] const std::shared_ptr<MeshData>& GetMeshData() const
	{
		return m_meshData;
	}

	/**
	* @brief Set mesh data
	* @param meshData The mesh data
	*/
	void SetMeshData(const std::shared_ptr <MeshData>& meshData);

	/**
	* Get materials list
	*/
	[[nodiscard]] std::vector<std::shared_ptr<Material>> GetMaterials() const
	{
		return m_materials;
	}

	/**
	* @brief Get a material at a specific index
	*/
	[[nodiscard]] std::shared_ptr<Material> GetMaterial(int index) const
	{
		if (index < m_materials.size())
			return m_materials[index];

		return nullptr;
	}

	/**
	* @brief Set a material at a specific index
	*/
	void SetMaterial(const std::shared_ptr<Material>& material, int index);

	/**
	* @brieg Get the bounding sphere of the mesh
	*/
	[[nodiscard]] const Sphere& GetBoundingSphere() const
	{
		return m_boundingSphere;
	}

	/**
	* @brief Get if the mesh can use advanced lighting (point lights, spot lights), improve performance if disabled
	*/
	[[nodiscard]] bool GetUseAdvancedLighting() const
	{
		return m_useAdvancedLighting;
	}

	/**
	* @brief Set if the mesh can use advanced lighting (point lights, spot lights), improve performance if disabled
	*/
	void SetUseAdvancedLighting(bool value);

protected:
	friend class WorldPartitionner;

	void OnNewRender(int cameraIndex) override;
	void OnComponentAttached() override;
	void OnDrawGizmosSelected() override;

	std::vector<WorldPartitionner::Chunk*> m_worldChunkPositions;
	std::vector<Light*> m_affectedByLights;
	[[nodiscard]] Sphere ProcessBoundingSphere() const;
	Sphere m_boundingSphere;

	friend class Lod;

	[[nodiscard]] ReflectiveData GetReflectiveData() override;
	void OnReflectionUpdated() override;

	/**
	* @brief Called when the component is disabled
	*/
	void OnDisabled() override;

	/**
	* @brief Called when the component is enabled
	*/
	void OnEnabled() override;

	/**
	* @brief Create the render commands
	*/
	void CreateRenderCommands(RenderBatch& renderBatch) override;

	/**
	* @brief Draw the command
	*/
	void DrawCommand(const RenderCommand& renderCommand) override;

	void OnTransformPositionUpdated();

	std::shared_ptr <MeshData> m_meshData = nullptr;
	std::vector<std::shared_ptr <Material>> m_materials;
	uint8_t m_matCount = 0;

	bool m_culled = false;
	bool m_outOfFrustum = false;
	bool m_useAdvancedLighting = true;
};