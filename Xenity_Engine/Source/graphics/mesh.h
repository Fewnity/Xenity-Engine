#pragma once

#include "../component.h"
#include "iDrawable.h"

class Shader;
class Material;

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

class Mesh : public Component, public IDrawable
{
public:
	Mesh();
	Mesh(float vertices[], unsigned int indices[], int verticesCount, int indicesCount);
	Mesh(const std::string meshpath);
	void LoadFromFile(const std::string meshpath);
	~Mesh();
	//Shader* shader = nullptr;
	Material* material;
	void LoadMesh(float vertices[], unsigned int indices[]);
	void DrawModel();
	void CreateBuffers(bool addUv, bool addNormals);
	void Draw();
	void Update();

private:
	void UpdateShader();
	void OnLoadFinished();

	unsigned int vertexArrayBuffer = 0;
	unsigned int vertexBuffer = 0;
	unsigned int indiceBuffer = 0;
	MeshData* meshData = new MeshData();
};