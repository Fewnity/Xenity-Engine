// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <vector>
#include <memory>

#include <engine/api.h>
#include <engine/graphics/iDrawable.h>
#include <engine/graphics/3d_graphics/sphere.h>

class MeshData;
class Material;
class Light;

class API MeshRenderer : public IDrawable
{
public:
	MeshRenderer();
	~MeshRenderer();

	/**
	* @brief Set mesh data
	* @param meshData The mesh data
	*/
	void SetMeshData(std::shared_ptr <MeshData> meshData);

	/**
	* @brief Get mesh data
	*/
	inline const std::shared_ptr<MeshData>& GetMeshData()
	{
		return m_meshData;
	}

	inline std::vector<std::shared_ptr <Material>> GetMaterials() const
	{
		return m_materials;
	}

	void SetMaterial(std::shared_ptr <Material> material, int index);

	inline std::shared_ptr <Material> GetMaterial(int index) const
	{
		if (index < m_materials.size())
			return m_materials[index];

		return nullptr;
	}

	void OnDrawGizmosSelected() override;

	const Sphere& GetBoundingSphere() const
	{
		return boundingSphere;
	}

	void OnNewRender() override;

	void OnComponentAttached() override;

	std::vector<Vector3> worldChunkPositions;
	std::vector<Light*> affectedByLights;

protected:
	Sphere ProcessBoundingSphere() const;
	Sphere boundingSphere;

	friend class Lod;

	ReflectiveData GetReflectiveData() override;
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
	size_t m_matCount = 0;

	bool m_culled = false;
	bool outOfFrustum = false;
};