#pragma once

#include <string>

class MeshData;

class WavefrontLoader {
public:
	static void LoadFromRawData(MeshData* mesh, const std::string filePath);
};