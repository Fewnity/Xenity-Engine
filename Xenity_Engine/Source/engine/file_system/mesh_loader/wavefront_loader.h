#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include <vector>
#include <memory>

class MeshData;
class Vector3;
class Vector2;

class API WavefrontLoader
{
public:
	static bool LoadFromRawData(std::shared_ptr <MeshData> mesh);
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