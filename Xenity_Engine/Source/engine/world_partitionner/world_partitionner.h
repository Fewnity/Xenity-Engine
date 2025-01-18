#pragma once

#include <vector>
#include <memory>
#include <unordered_map>

class Light;
class MeshRenderer;

class WorldPartitionner
{
public:
	static void ClearWorld();
	static void RemoveMeshRenderer(MeshRenderer* meshRenderer);
	static void RemoveLight(Light* light);
	static void OnDrawGizmos();

	class Chunk
	{
	public:
		std::vector<Light*> lights;
		std::vector<MeshRenderer*> meshes;
	};

	class ZNode
	{
	public:
		Chunk chunk;
	};

	class YNode
	{
	public:
		std::unordered_map<int, ZNode> children;
	};

	class XNode
	{
	public:
		std::unordered_map<int, YNode> children;
	};

	class Tree
	{
	public:
		static std::unordered_map<int, XNode> children;
	};

	static void ProcessMeshRenderer(MeshRenderer* meshRenderer);
	static void ProcessLight(Light* light);

private:
	static void DrawChunk(const Chunk& chunk, int x, int y, int z);
};

