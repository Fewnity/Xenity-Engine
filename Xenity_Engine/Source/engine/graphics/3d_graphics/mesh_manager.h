#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include <string>
#include <vector>

class MeshData;
class Vector3;
class Texture;

class API MeshManager
{
public:
	static void Init();
	static void DrawMesh(Vector3 position, Vector3 rotation, Vector3 scale, std::vector<Texture*>textures, MeshData *meshData, bool useDepth, bool useBlend, bool useLighting);
	static void DrawMesh(Vector3 position, Vector3 rotation, Vector3 scale, Texture*texture, MeshData* meshData, bool useDepth, bool useBlend, bool useLighting);
	static MeshData *LoadMesh(std::string path);

private:
};