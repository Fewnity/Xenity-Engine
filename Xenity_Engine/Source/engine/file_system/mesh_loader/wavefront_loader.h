// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

/**
 * [Internal]
 */

#include <vector>
#include <string>

class MeshData;
class Vector3;
class Vector2;

class WavefrontLoader
{
public:

	/**
	* @brief Load a mesh from a file
	* @param mesh Mesh file to load
	* @return True if the mesh was loaded successfully
	*/
	static bool LoadFromRawData(MeshData& mesh);

private:

	static bool ReadMtlFile(const std::string& path);

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
		std::string matName;
	};
};