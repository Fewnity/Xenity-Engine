#pragma once
#include <engine/api.h>

#include <vector>
#include <memory>

class MeshData;
class Vector3;
class Vector2;

class API WavefrontLoader
{
public:
	static bool LoadFromRawData(const std::shared_ptr <MeshData>& mesh);
private:
	class SubMesh
	{
	public:
		SubMesh() = default;
		std::vector<int> vertexIndices;
		std::vector<int> textureIndices;
		std::vector<int> normalsIndices;
		int dataId = -1;
		int indicesCount = 0;
		int verticesCount = 0;
		int normalsCount = 0;
		int textureCordsCount = 0;
	};
};