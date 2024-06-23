#pragma once

#include <vector>
#include <memory>
#include <map>

class Light;
class MeshRenderer;

#define CHUNK_SIZE 10
#define CHUNK_HALF_SIZE 5


class WorldPartitionner
{
public:
	static void ProcessPartionning();
	static void OnDrawGizmos();
	class Chunk
	{
	public:
		std::vector<std::weak_ptr<Light>> lights;
		std::vector<std::weak_ptr<MeshRenderer>> meshes;
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

	static std::vector<const Chunk*> GetChunksOfMeshRenderer(const std::shared_ptr<MeshRenderer>& meshRenderer);

private:
	static void DrawChunk(const Chunk& chunk, int x, int y, int z);
};

