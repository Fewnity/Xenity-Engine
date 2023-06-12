#pragma once

#include <string>

class MeshData;
class Vector3;
class Texture;

class MeshManager
{
public:
	static void Init();
	static void DrawMesh(Vector3 position, Vector3 rotation, Vector3 scale, Texture *texture, MeshData *meshData, bool useDepth);
	static MeshData *LoadMesh(std::string path);

private:
};