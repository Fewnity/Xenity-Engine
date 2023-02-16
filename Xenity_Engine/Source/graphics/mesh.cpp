#include "mesh.h"
#include <glad/glad.h>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include "../asset_manager.h"

#include "graphics.h"
#include "material.h"
#include "../file_system/file.h"
#include "../main.h"
#include "../vectors/vector2.h"
#include "../file_system/mesh_loader/wavefront_loader.h"

using namespace std;

Mesh::Mesh() : Component()
{
	meshData->verticesCount = 0;
	meshData->vertices = (float*)calloc(meshData->verticesCount, sizeof(float));
	meshData->indicesCount = 0;
	meshData->indices = (unsigned int*)calloc(meshData->indicesCount, sizeof(unsigned int));
	CreateBuffers(true, true);
	OnLoadFinished();
}

Mesh::Mesh(float vertices[], unsigned int indices[], int verticesCount, int indicesCount) : Component()
{
	meshData->verticesCount = verticesCount / sizeof(*vertices);
	meshData->vertices = (float*)calloc(meshData->verticesCount, sizeof(float));
	meshData->indicesCount = indicesCount / sizeof(*indices);
	meshData->indices = (unsigned int*)calloc(meshData->indicesCount, sizeof(unsigned int));
	LoadMesh(vertices, indices);
	CreateBuffers(true, true);
	OnLoadFinished();
}

Mesh::Mesh(const std::string meshpath) : Component()
{
	LoadFromFile(meshpath);
}

void Mesh::LoadFromFile(const std::string meshpath) {
	WavefrontLoader::LoadMesh(meshData, meshpath);
	CreateBuffers(meshData->hasUv, meshData->hasNormal);
	OnLoadFinished();
}

void Mesh::Update()
{
}

Mesh::~Mesh()
{
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

	int finalByteCount = 3;
	if (addNormals)
		finalByteCount += 3;
	if (addUv)
		finalByteCount += 2;

	int byteOffset = 0;
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, finalByteCount * sizeof(float), (void*)byteOffset);
	//Enable my vertex attrib array number 0
	glEnableVertexAttribArray(0);//Vertex attribute array 0 is now available for use.
	byteOffset += 3;

	if (addUv)
	{
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, finalByteCount * sizeof(float), (void*)(byteOffset * sizeof(float)));
		//Enable texture coords attrib array number 1
		glEnableVertexAttribArray(1);//Texture attribute array 2 is now available for use.
		byteOffset += 2;
	}

	if (addNormals)
	{
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, finalByteCount * sizeof(float), (void*)(byteOffset * sizeof(float)));
		//Enable Normals attrib array number 2
		glEnableVertexAttribArray(2);//Normals attribute array 2 is now available for use.	
		byteOffset += 3;
	}
	glBindVertexArray(0);

	glBufferData(GL_ARRAY_BUFFER, meshData->verticesCount * sizeof(*meshData->vertices), meshData->vertices, GL_STATIC_DRAW);//Put vertice in the array buffer
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshData->indicesCount * sizeof(*meshData->indices), meshData->indices, GL_STATIC_DRAW);//Put vertice in the array buffer
	//glDrawElements(GL_TRIANGLES, meshData->indicesCount, GL_UNSIGNED_INT, 0);
}

void Mesh::Draw()
{
	if (gameObject != nullptr && gameObject->GetLocalActive() && meshData != nullptr)
	{
		UpdateShader();
		glBindVertexArray(vertexArrayBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer); //Set the current GL_ARRAY_BUFFER
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiceBuffer); //Set the current GL_ARRAY_BUFFER
		glDrawArrays(GL_TRIANGLES, 0, meshData->verticesCount);
		glBindVertexArray(0);
	}
}

void Mesh::OnLoadFinished()
{
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

void Mesh::UpdateShader() {
	//return;
	if (material != nullptr) {
		if (material->shader != nullptr) {
			material->Use();
			bool noNeedUpdate = material->updated;
			if (!noNeedUpdate)
			{
			material->Update();
				material->shader->SetShaderCameraPosition();
				material->shader->SetShaderProjection3D();
				material->shader->SetShaderAttribut("cameraPos", Graphics::usedCamera->gameObject->transform.GetPosition());
				material->shader->SetShaderAttribut("offsetPosition", Vector3(0, 0, 0));
				material->shader->UpdateLights();
			}
			material->shader->SetShaderModel(gameObject->transform.transformationMatrix);
		}
	}
}