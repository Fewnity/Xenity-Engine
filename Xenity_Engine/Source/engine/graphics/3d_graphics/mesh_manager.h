#pragma once

class MeshData;
class Vector3;
class Texture;

class MeshManager
{
public:
	static void DrawMeshData(MeshData *meshData);
	static void DrawMesh(Vector3 position, Vector3 rotation, Vector3 scale, Texture *texture, MeshData *meshData);

private:
};