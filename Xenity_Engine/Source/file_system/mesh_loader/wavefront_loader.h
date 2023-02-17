#pragma once

#include <string>

class MeshData;

class WavefrontLoader {
public:
	static void LoadMesh(MeshData* mesh, const std::string filePath);
};