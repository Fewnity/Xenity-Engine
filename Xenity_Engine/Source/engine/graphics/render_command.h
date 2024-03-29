#pragma once

#include <engine/graphics/3d_graphics/mesh_data.h>
#include <glm/glm.hpp>

class Material;
class IDrawable;

class RenderCommand
{
public:
	const MeshData::SubMesh* subMesh = nullptr;
	std::shared_ptr<Transform> transform = nullptr;
	std::shared_ptr<Material> material = nullptr;
	IDrawable* drawable = nullptr;
	bool isEnabled;
};

class RenderQueue 
{
public:
	RenderQueue() 
	{
		commands.resize(1500);
	}
	int commandIndex = 0;
	std::vector<RenderCommand> commands;
};

class RenderBatch
{
public:
	RenderBatch()
	{
		transparentMeshCommands.resize(100);
		spriteCommands.resize(1000);
		uiCommands.resize(200);
	}

	std::unordered_map<uint64_t, RenderQueue> renderQueues;

	std::vector<RenderCommand> transparentMeshCommands;
	std::vector<RenderCommand> spriteCommands;
	std::vector<RenderCommand> uiCommands;
	int transparentMeshCommandIndex = 0;
	int spriteCommandIndex = 0;
	int uiCommandIndex = 0;

	void Reset()
	{
		for (auto& renderQueue : renderQueues)
		{
			renderQueue.second.commandIndex = 0;
		}
		transparentMeshCommandIndex = 0;
		spriteCommandIndex = 0;
		uiCommandIndex = 0;
	}
};