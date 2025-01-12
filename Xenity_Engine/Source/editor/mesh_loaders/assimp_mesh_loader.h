#pragma once

class MeshData;
struct aiMesh;

/**
* Class used to load binarized meshdata
*/
class AssimpMeshLoader
{
public:
	/**
	* Load mesh data
	*/
	static bool LoadMesh(MeshData& mesh, bool forceNoIndices = false);
private:
	static void AddVertex(MeshData& mesh, const aiMesh* assimpMesh, unsigned int vertexIndex, unsigned int subMeshIndex, unsigned int vertexIndex2);
};

