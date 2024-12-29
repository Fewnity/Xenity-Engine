// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <unordered_map>

#include <engine/graphics/3d_graphics/mesh_data.h>
#include <engine/constants.h>

class Material;
class IDrawable;

class RenderCommand
{
public:
	Transform* transform = nullptr;
	Material* material = nullptr;
	const MeshData::SubMesh* subMesh = nullptr;
	IDrawable* drawable = nullptr;
	bool isEnabled;
};

class RenderQueue 
{
public:
	std::vector<RenderCommand> commands;
	int commandIndex = 0;
};

class RenderBatch
{
public:
	// uint64_t = material id
	std::unordered_map<uint64_t, RenderQueue> renderQueues;
#if defined(ENABLE_OVERDRAW_OPTIMIZATION)
	std::vector<RenderCommand> opaqueMeshCommands;
#endif
	std::vector<RenderCommand> transparentMeshCommands;
	std::vector<RenderCommand> spriteCommands;
	std::vector<RenderCommand> uiCommands;
#if defined(ENABLE_OVERDRAW_OPTIMIZATION)
	size_t opaqueMeshCommandIndex = 0;
#endif
	size_t transparentMeshCommandIndex = 0;
	size_t spriteCommandIndex = 0;
	size_t uiCommandIndex = 0;

	/**
	* @brief Reset the render batch
	*/
	void Reset()
	{
		for (auto& renderQueue : renderQueues)
		{
			renderQueue.second.commands.clear();
			renderQueue.second.commandIndex = 0;
		}
#if defined(ENABLE_OVERDRAW_OPTIMIZATION)
		opaqueMeshCommandIndex = 0;
#endif
		transparentMeshCommandIndex = 0;
		spriteCommandIndex = 0;
		uiCommandIndex = 0;
#if defined(ENABLE_OVERDRAW_OPTIMIZATION)
		opaqueMeshCommands.clear();
#endif
		transparentMeshCommands.clear();
		spriteCommands.clear();
		uiCommands.clear();
	}
};