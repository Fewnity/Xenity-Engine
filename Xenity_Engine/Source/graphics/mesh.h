#pragma once

#include "../component.h"
#include "iDrawable.h"

class Shader;
class Material;

class MeshData {
public:
	MeshData(std::string path) 
	{
		filePath = path;
	}

	~MeshData() {
		free(vertices);
		free(indices);
	}
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
	Mesh(float vertices[], unsigned int indices[], int verticesCount, int indicesCount);
	Mesh(const std::string meshpath);
	~Mesh();

	Material* material = nullptr;

	void LoadFromFile(const std::string meshpath);
	void LoadMesh(float vertices[], unsigned int indices[]);
	void CreateBuffers(bool addUv, bool addNormals);

private:
	void Draw();
	void Update();
	void UpdateMaterial();

	unsigned int vertexArrayBuffer = 0;
	unsigned int vertexBuffer = 0;
	unsigned int indiceBuffer = 0;
	MeshData* meshData = new MeshData();
};