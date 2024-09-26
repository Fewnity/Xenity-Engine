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
	static bool LoadMesh(MeshData& mesh);
};

