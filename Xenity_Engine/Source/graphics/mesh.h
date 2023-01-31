#pragma once

#include "shader.h"
#include "../component.h"

class MeshData {
public:
	~MeshData() {
		free(vertices);
		free(indices);
	}
	float* vertices = nullptr;
	unsigned int* indices = nullptr;
	int verticesCount = 0;
	int indicesCount = 0;
	bool hasUv = false;
	bool hasNormal = false;
};

class Mesh : public Component
{
public:
	Mesh();
	Mesh(float vertices[], unsigned int indices[], int verticesCount, int indicesCount);
	Mesh(const std::string meshpath);
	~Mesh();
	Shader* shader = nullptr;
	void LoadMesh(float vertices[], unsigned int indices[]);
	void DrawModel();
	void CreateBuffers(bool addUv, bool addNormals);

private:
	void UpdateShader();
	void OnLoadFinished();

	unsigned int vertexArrayBuffer = 0;
	unsigned int vertexBuffer = 0;
	unsigned int indiceBuffer = 0;
	MeshData* meshData = new MeshData();
};