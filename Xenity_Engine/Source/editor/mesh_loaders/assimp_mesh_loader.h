#pragma once

class MeshData;
struct aiMesh;

/**
* Class used to load binarized meshdata
*/
class AssimpMeshLoader
{
public:
	struct LoadingOptions
	{
		bool forceNoIndices = false;
		bool forceColors = false;
	};

	/**
	* Load mesh data
	*/
	static bool LoadMesh(MeshData& mesh, const LoadingOptions& options);
private:
	static void AddVertex(MeshData& mesh, const LoadingOptions& options, const aiMesh* assimpMesh, unsigned int vertexIndex, unsigned int subMeshIndex, unsigned int vertexIndex2);
};

