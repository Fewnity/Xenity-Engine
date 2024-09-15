#pragma once

#include <vector>
#include <memory>
#include <map>

class Light;
class MeshRenderer;

#define CHUNK_SIZE 10
#define CHUNK_HALF_SIZE (CHUNK_SIZE / 2.0f)

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
		std::map<int, ZNode> children;
	};

	class XNode
	{
	public:
		std::map<int, YNode> children;
	};

	class Tree
	{
	public:
		static std::map<int, XNode> children;
	};

	static void ProcessMeshRenderer(MeshRenderer* meshRenderer);
	static void ProcessLight(Light* light);

private:
	static void DrawChunk(const Chunk& chunk, int x, int y, int z);
};

