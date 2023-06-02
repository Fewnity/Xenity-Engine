#pragma once

#include <string>

class MeshData;

class WavefrontLoader
{
public:
	static MeshData *LoadFromRawData(const std::string filePath);
};