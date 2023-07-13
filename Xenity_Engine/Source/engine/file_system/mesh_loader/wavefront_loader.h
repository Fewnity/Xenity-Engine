#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include <string>

class MeshData;

class API WavefrontLoader
{
public:
	static MeshData *LoadFromRawData(const std::string filePath);
};