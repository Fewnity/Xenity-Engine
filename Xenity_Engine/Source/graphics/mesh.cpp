#include "mesh.h"
#include <glad/glad.h>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include "../asset_manager.h"

#include "graphics.h"
#include "../file_system/file.h"
#include "../main.h"
#include "../vectors/vector2.h"
#include "../file_system/mesh_loader/wavefront_loader.h"

using namespace std;

Mesh::Mesh() : Component() {
	meshData->verticesCount = 0;
	meshData->vertices = (float*)calloc(meshData->verticesCount, sizeof(float));
	meshData->indicesCount = 0;
	meshData->indices = (unsigned int*)calloc(meshData->indicesCount, sizeof(unsigned int));
	CreateBuffers(true, true);
	OnLoadFinished();
}

Mesh::Mesh(float vertices[], unsigned int indices[], int verticesCount, int indicesCount) : Component() {
	meshData->verticesCount = verticesCount / sizeof(*vertices);
	meshData->vertices = (float*)calloc(meshData->verticesCount, sizeof(float));
	meshData->indicesCount = indicesCount / sizeof(*indices);
	meshData->indices = (unsigned int*)calloc(meshData->indicesCount, sizeof(unsigned int));
	LoadMesh(vertices, indices);
	CreateBuffers(true, true);
	OnLoadFinished();
}

Mesh::Mesh(const std::string meshpath) : Component() {
	WavefrontLoader::LoadMesh(meshData, meshpath);
	CreateBuffers(meshData->hasUv, meshData->hasNormal);
	OnLoadFinished();
}

void Mesh::Update() {
	if(gameObject)
		gameObject->transform.rotation.y -= 0.1f;
}

Mesh::~Mesh()
{
	AssetManager::RemoveMesh(this);

	delete meshData;
	glDeleteVertexArrays(1, &vertexArrayBuffer);
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &indiceBuffer);
}

void Mesh::CreateBuffers(bool addUv, bool addNormals)
{
	glGenVertexArrays(1, &vertexArrayBuffer); //Create a buffer ID for the vertex array buffer
	glGenBuffers(1, &vertexBuffer);//Create a buffer ID for the vertex buffer
	glGenBuffers(1, &indiceBuffer);//Create a buffer ID for the indice buffer

	glBindVertexArray(vertexArrayBuffer);//??????????????????
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer); //Set the current GL_ARRAY_BUFFER

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiceBuffer); //Set the current GL_ARRAY_BUFFER

	int byteCount = 3;
	if (addNormals)
		byteCount += 3;
	if (addUv)
		byteCount += 2;

	int finalByteCount = 0;
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, byteCount * sizeof(float), (void*)finalByteCount);
	//Enable my vertex attrib array number 0
	glEnableVertexAttribArray(0);//Vertex attribute array 0 is now available for use.

	if (addUv)
	{
		finalByteCount += 3;
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, byteCount * sizeof(float), (void*)(finalByteCount * sizeof(float)));
		//Enable texture coords attrib array number 1
		glEnableVertexAttribArray(1);//Texture attribute array 2 is now available for use.
	}

	if (addNormals)
	{
		finalByteCount += 2;
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, byteCount * sizeof(float), (void*)(finalByteCount * sizeof(float)));
		//Enable Normals attrib array number 2
		glEnableVertexAttribArray(2);//Normals attribute array 2 is now available for use.	
	}
	glBindVertexArray(0);

	glBufferData(GL_ARRAY_BUFFER, meshData->verticesCount * sizeof(*meshData->vertices), meshData->vertices, GL_STATIC_DRAW);//Put vertice in the array buffer
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshData->indicesCount * sizeof(*meshData->indices), meshData->indices, GL_STATIC_DRAW);//Put vertice in the array buffer
	//glDrawElements(GL_TRIANGLES, meshData->indicesCount, GL_UNSIGNED_INT, 0);
}

void Mesh::OnLoadFinished()
{
	AssetManager::AddMesh(this);
}

void Mesh::LoadMesh(float vertices[], unsigned int indices[]) {
	//Fill vertices
	for (int i = 0; i < meshData->verticesCount; i++)
	{
		meshData->vertices[i] = vertices[i];
	}

	//Fill indices
	for (int i = 0; i < meshData->indicesCount; i++)
	{
		meshData->indices[i] = indices[i];
	}
}

void Mesh::DrawModel() {
	if (gameObject != nullptr && gameObject->active)
	{
		UpdateShader();
		glBindVertexArray(vertexArrayBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer); //Set the current GL_ARRAY_BUFFER
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiceBuffer); //Set the current GL_ARRAY_BUFFER
		glDrawArrays(GL_TRIANGLES, 0, meshData->verticesCount);
		glBindVertexArray(0);
	}
}

void Mesh::UpdateShader() {
	if (shader != nullptr) {
		shader->Use();
		shader->SetShaderCameraPosition();
		shader->SetShaderProjection();
		shader->SetShaderPosition(gameObject->transform.position);
		shader->SetShaderRotation(gameObject->transform.rotation);
		shader->SetShaderScale(gameObject->transform.scale);
	}
}