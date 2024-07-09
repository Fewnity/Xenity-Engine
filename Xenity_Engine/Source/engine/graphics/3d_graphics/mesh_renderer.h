// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <engine/api.h>

#include <engine/graphics/iDrawable.h>
#include <vector>
#include <memory>

class MeshData;
class Material;

struct Sphere 
{
	Vector3 position;
	float radius;
};

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
	inline std::shared_ptr<MeshData> GetMeshData()
	{
		return meshData;
	}

	inline std::vector<std::shared_ptr <Material>> GetMaterials() const
	{
		return materials;
	}

	void SetMaterial(std::shared_ptr <Material> material, int index);

	inline std::shared_ptr <Material> GetMaterial(int index) const
	{
		if (index < materials.size())
			return materials[index];

		return nullptr;
	}

	void OnDrawGizmosSelected() override;

	Sphere GetBoundingSphere() const;

	void OnNewRender() override;

protected:
	Sphere boundingSphere;
	bool outOfFrustum = false;

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

	std::shared_ptr <MeshData> meshData = nullptr;
	std::vector<std::shared_ptr <Material>> materials;
	size_t matCount = 0;

	bool culled = false;
};