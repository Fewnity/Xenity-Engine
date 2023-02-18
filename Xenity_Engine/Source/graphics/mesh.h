#pragma once

#include "../component.h"
#include "iDrawable.h"

class Shader;
class Material;

class MeshData 
{
public:
	MeshData();
	MeshData(const std::string path);
	~MeshData();

	std::string filePath = "";
	float* vertices = nullptr;
	unsigned int* indices = nullptr;
	int verticesCount = 0;
	int indicesCount = 0;
	bool hasUv = false;
	bool hasNormal = false;
};

class Mesh : public Component, public IDrawable
{
public:
	Mesh();
	Mesh(MeshData* meshData);
	Mesh(const float vertices[], const unsigned int indices[], const int verticesCount, const int indicesCount);
	Mesh(const std::string meshpath);
	~Mesh();

	Material* material = nullptr;

	void LoadFromFile(const std::string meshpath);
	void LoadMesh(const float vertices[], const unsigned int indices[]);
	void CreateBuffers(const bool addUv, const bool addNormals);

private:
	void Draw();
	void Update();
	void UpdateMaterial();
	void LoadFromMeshData(MeshData* meshData);

	unsigned int vertexArrayBuffer = 0;
	unsigned int vertexBuffer = 0;
	unsigned int indiceBuffer = 0;
	MeshData* meshData = nullptr;
	//MeshData* meshData = new MeshData();
};