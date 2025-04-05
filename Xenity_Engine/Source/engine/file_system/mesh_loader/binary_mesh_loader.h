#pragma once

class MeshData;

/**
* Class used to load binarized meshdata
*/
class BinaryMeshLoader
{
public:
	/**
	* Load mesh data
	*/
	[[nodiscard]] static bool LoadMesh(MeshData& mesh);
};

