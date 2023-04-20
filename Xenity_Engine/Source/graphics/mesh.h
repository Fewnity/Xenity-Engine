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

class MeshRenderer : public IDrawable
{
public:
	MeshRenderer();
	MeshRenderer(MeshData* meshData);
	MeshRenderer(const float vertices[], const unsigned int indices[], const int verticesCount, const int indicesCount);
	MeshRenderer(const std::string meshpath);
	~MeshRenderer();

	Material* material = nullptr;

	void LoadFromFile(const std::string meshpath);
	void LoadFromRawData(const float vertices[], const unsigned int indices[]);
	void CreateBuffers(const bool addUv, const bool addNormals);

private:
	void Draw();
	void Update();
	void UpdateMaterial();
	void LoadFromMeshData(MeshData* meshData);
	void SetIDrawbleSettings();

	unsigned int vertexArrayBuffer = 0;
	unsigned int vertexBuffer = 0;
	unsigned int indiceBuffer = 0;
	MeshData* meshData = nullptr;
	//MeshData* meshData = new MeshData();
};