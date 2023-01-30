#pragma once

#include "shader.h"
#include "../component.h"

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

private:
	void UpdateShader();
	void OnLoadFinished();
	float *vertices = nullptr;
	int verticesCount = 0;
	int indicesCount = 0;
	unsigned int *indices = nullptr;
	unsigned int vertexArrayBuffer = 0;
	unsigned int vertexBuffer = 0;
	unsigned int indiceBuffer = 0;
	void CreateBuffers(bool addUv, bool addNormals);
};