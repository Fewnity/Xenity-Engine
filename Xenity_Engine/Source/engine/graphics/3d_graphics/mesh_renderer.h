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

class API MeshRenderer : public IDrawable
{
public:
	MeshRenderer();
	~MeshRenderer();

	ReflectiveData GetReflectiveData() override;
	void OnReflectionUpdated() override;

	/**
	 * @brief [Internal]
	 */
	int GetDrawPriority() const override;

	/**
	* @brief Set mesh data
	* @param meshData The mesh data
	*/
	void SetMeshData(std::shared_ptr <MeshData> meshData);

	/**
	* @brief Get mesh data
	*/
	std::shared_ptr <MeshData> GetMeshData();

	std::vector<std::shared_ptr <Material>> GetMaterials() const
	{
		return materials;
	}

	void SetMaterial(std::shared_ptr <Material> material, int index);

	std::shared_ptr <Material> GetMaterial(int index) const
	{
		if (index < materials.size())
			return materials[index];

		return nullptr;
	}


private:

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

	std::shared_ptr <MeshData> meshData = nullptr;
	std::vector<std::shared_ptr <Material>> materials;
	int matCount = 0;

public:
	bool culled = false;
};