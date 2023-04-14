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
#include "../debug/performance.h"

using namespace std;

#pragma region MeshData Constructors / Destructor

/// <summary>
/// Used if a mesh is not created from a file
/// </summary>
MeshData::MeshData()
{
	filePath = "";
	AssetManager::AddMeshData(this);
}

MeshData::MeshData(const std::string path)
{
	filePath = path;
	AssetManager::AddMeshData(this);
}

MeshData::~MeshData()
{
	AssetManager::RemoveMeshData(this);
	free(vertices);
	free(indices);
}

#pragma endregion

#pragma region MeshRenderer Constructors / Destructor

/// <summary>
/// Instantiate an empty mesh
/// </summary>
MeshRenderer::MeshRenderer() : Component()
{
	meshData = new MeshData();
	meshData->verticesCount = 0;
	meshData->vertices = (float*)calloc(meshData->verticesCount, sizeof(float));
	meshData->indicesCount = 0;
	meshData->indices = (unsigned int*)calloc(meshData->indicesCount, sizeof(unsigned int));
	CreateBuffers(true, true);
	SetIDrawbleSettings();
}

MeshRenderer::MeshRenderer(MeshData* meshData) : Component()
{
	LoadFromMeshData(meshData);
	SetIDrawbleSettings();
}

/// <summary>
/// Instantiate a mesh from a vertices and indices lists
/// </summary>
/// <param name="vertices"></param>
/// <param name="indices"></param>
/// <param name="verticesCount"></param>
/// <param name="indicesCount"></param>
MeshRenderer::MeshRenderer(const float vertices[], const unsigned int indices[], const int verticesCount, const int indicesCount) : Component()
{
	meshData = new MeshData();
	meshData->verticesCount = verticesCount / sizeof(*vertices);
	meshData->vertices = (float*)calloc(meshData->verticesCount, sizeof(float));
	meshData->indicesCount = indicesCount / sizeof(*indices);
	meshData->indices = (unsigned int*)calloc(meshData->indicesCount, sizeof(unsigned int));
	LoadFromRawData(vertices, indices);
	CreateBuffers(true, true);
	SetIDrawbleSettings();
}

/// <summary>
/// Instantiate a mesh from a file
/// </summary>
/// <param name="meshpath"></param>
MeshRenderer::MeshRenderer(const std::string meshpath) : Component()
{
	LoadFromFile(meshpath);
	SetIDrawbleSettings();
}

/// <summary>
/// Destructor
/// </summary>
MeshRenderer::~MeshRenderer()
{
	delete meshData;
	glDeleteVertexArrays(1, &vertexArrayBuffer);
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &indiceBuffer);
}

#pragma endregion

void MeshRenderer::SetIDrawbleSettings() 
{
	invertedTriangles = true;
}

#pragma region Data loading

/// <summary>
/// Load the mesh from a file
/// </summary>
/// <param name="meshpath"></param>
void MeshRenderer::LoadFromFile(const std::string meshpath)
{
	MeshData* foundData = AssetManager::GetMeshData(meshpath);
	if (foundData == nullptr)
	{
		meshData = new MeshData(meshpath);
		WavefrontLoader::LoadFromRawData(meshData, meshpath);
		CreateBuffers(meshData->hasUv, meshData->hasNormal);
	}
	else
	{
		LoadFromMeshData(foundData);
	}
}

void MeshRenderer::LoadFromMeshData(MeshData* meshData)
{
	this->meshData = meshData;
	CreateBuffers(meshData->hasUv, meshData->hasNormal);
}

/// <summary>
/// Fill mesh data with vertices and indices lists
/// </summary>
/// <param name="vertices"></param>
/// <param name="indices"></param>
void MeshRenderer::LoadFromRawData(const float vertices[], const unsigned int indices[]) {
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

#pragma endregion



void MeshRenderer::Update()
{
}

/// <summary>
/// Create mesh's buffers
/// </summary>
/// <param name="addUv"></param>
/// <param name="addNormals"></param>
void MeshRenderer::CreateBuffers(const bool addUv, const bool addNormals)
{
	glGenVertexArrays(1, &vertexArrayBuffer); //Create a buffer ID for the vertex array buffer
	glGenBuffers(1, &vertexBuffer);//Create a buffer ID for the vertex buffer
	glGenBuffers(1, &indiceBuffer);//Create a buffer ID for the indice buffer

	glBindVertexArray(vertexArrayBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer); //Set the current GL_ARRAY_BUFFER

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiceBuffer); //Set the current GL_ARRAY_BUFFER

	int finalByteCount = 3;
	if (addNormals)
		finalByteCount += 3;
	if (addUv)
		finalByteCount += 2;

	int byteOffset = 0;
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, finalByteCount * sizeof(float), (void*)byteOffset);
	//Enable vertex attrib array number 0
	glEnableVertexAttribArray(0);
	byteOffset += 3;

	if (addUv)
	{
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, finalByteCount * sizeof(float), (void*)(byteOffset * sizeof(float)));
		//Enable texture coords attrib array number 1
		glEnableVertexAttribArray(1);
		byteOffset += 2;
	}

	if (addNormals)
	{
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, finalByteCount * sizeof(float), (void*)(byteOffset * sizeof(float)));
		//Enable Normals attrib array number 2
		glEnableVertexAttribArray(2);
		byteOffset += 3;
	}

	glBufferData(GL_ARRAY_BUFFER, meshData->verticesCount * sizeof(*meshData->vertices), meshData->vertices, GL_STATIC_DRAW);//Put vertice in the array buffer
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshData->indicesCount * sizeof(*meshData->indices), meshData->indices, GL_STATIC_DRAW);//Put vertice in the array buffer
	//glDrawElements(GL_TRIANGLES, meshData->indicesCount, GL_UNSIGNED_INT, 0);
	
	glBindVertexArray(0);
}

#pragma region Drawing

/// <summary>
/// Draw mesh
/// </summary>
void MeshRenderer::Draw()
{
	//Draw the mesh only if the mesh is on an active gameobject and if the mesh data is not null
	if (gameObject != nullptr && gameObject->GetLocalActive() && meshData != nullptr)
	{
		if (material != nullptr)
		{
			glEnable(GL_DEPTH_TEST);
			UpdateMaterial();
			Performance::AddDrawCall();
			glBindVertexArray(vertexArrayBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer); //Set the current GL_ARRAY_BUFFER
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiceBuffer); //Set the current GL_ARRAY_BUFFER
			glDrawArrays(GL_TRIANGLES, 0, meshData->verticesCount);
			glBindVertexArray(0);
		}
	}
}

/// <summary>
/// Update mesh's material
/// </summary>
void MeshRenderer::UpdateMaterial()
{
	if (material != nullptr && material->shader != nullptr)
	{
		material->Use();
		bool noNeedUpdate = material->updated;
		if (!noNeedUpdate)
		{
			material->Update();
			material->shader->SetShaderCameraPosition();
			material->shader->SetShaderProjection();
			material->shader->SetShaderAttribut("cameraPos", Graphics::usedCamera->gameObject->transform.GetPosition());
			material->shader->SetShaderAttribut("offsetPosition", Vector3(0, 0, 0));
			material->shader->UpdateLights();
		}
		material->shader->SetShaderModel(&gameObject->transform.transformationMatrix);
	}
}

#pragma endregion

