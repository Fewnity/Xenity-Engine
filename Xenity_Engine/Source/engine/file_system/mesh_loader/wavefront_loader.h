#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include <vector>

class MeshData;
class Vector3;

class API WavefrontLoader
{
public:
	static bool LoadFromRawData(MeshData* mesh);
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
	};
};