#pragma once

#include <string>

class MeshData;

class WavefrontLoader {
public:
	static void LoadMesh(MeshData* mesh, std::string filePath);
};