#pragma once

/**
 * [Internal]
 */

#include <vector>
#include <memory>

class MeshData;
class Vector3;
class Vector2;

class WavefrontLoader
{
public:
	static bool LoadFromRawData(const std::shared_ptr <MeshData>& mesh);
private:
	struct SubMesh
	{
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