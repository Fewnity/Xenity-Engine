#pragma once

#ifndef MESH_H /* Include guard */
#define MESH_H

#include "shader.h"
#include "../component.h"

class Mesh : public Component
{
public:
	Mesh();
	Mesh(float vertices[], unsigned int indices[], int verticesCount, int indicesCount);
	Mesh(const std::string meshpath);
	Shader* shader = nullptr;
	void LoadMesh(float vertices[], unsigned int indices[]);
	void DrawModel();

private:
	void UpdateShader();
	void OnLoadFinished();
	float *vertices;
	int verticesCount;
	int indicesCount;
	unsigned int *indices;
	unsigned int vertexArrayBuffer;
	unsigned int vertexBuffer;
	unsigned int indiceBuffer;
	void CreateBuffers(bool addUv, bool addNormals);
};

#endif