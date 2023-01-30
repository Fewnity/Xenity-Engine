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

using namespace std;

Mesh::Mesh() : Component() {
	this->verticesCount = 0;
	this->vertices = (float*)calloc(this->verticesCount, sizeof(float));
	this->indicesCount = 0;
	this->indices = (unsigned int*)calloc(this->indicesCount, sizeof(unsigned int));
	CreateBuffers(true, true);
	OnLoadFinished();
}

Mesh::Mesh(float vertices[], unsigned int indices[], int verticesCount, int indicesCount) : Component() {
	this->verticesCount = verticesCount / sizeof(*vertices);
	this->vertices = (float*)calloc(this->verticesCount, sizeof(float));
	this->indicesCount = indicesCount / sizeof(*indices);
	this->indices = (unsigned int*)calloc(this->indicesCount, sizeof(unsigned int));
	LoadMesh(vertices, indices);
	CreateBuffers(true, true);
	OnLoadFinished();
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
}

std::string modelsPath = R"(Xenity_Engine\Source\models\)"; //TODO remove this

Mesh::Mesh(const std::string meshpath) : Component()  {
	std::string finalpath = PROJECT_FOLDER + modelsPath;
	//Open file
	ifstream file;
	file.open(finalpath + meshpath);

	//Print error if the file can't be read
	if (file.fail()) {
		std::cout << "\033[31mModel load error. Path : \"" << finalpath + meshpath << "\"\033[0m" << std::endl;
	}
	int verticesCount = 0;
	int indicesCount = 0;
	int normalsCount = 0;
	int textureCordsCount = 0;

	vector<Vector3> tempVertices;
	vector<Vector2> tempTexturesCoords;
	vector<Vector3> tempNormals;
	vector<int> vertexIndices;
	vector<int> textureIndices;
	vector<int> normalsIndices;
	bool hasNoUv = false;
	bool hasNoNormals = false;
	//Read file
	std::string text, line;
	while (std::getline(file, line)) {
		//printf(line.c_str());
		//printf("%s", line);
		if (line.substr(0, 2) == "v ") {
			float x = 0, y = 0, z = 0;
			sscanf_s(line.c_str(), "v %f %f %f\n", &x, &y, &z);
			verticesCount++;
			tempVertices.emplace_back(x, y, z);
		}
		else if (line.substr(0, 3) == "vt ") {
			float x = 0, y = 0;
			sscanf_s(line.c_str(), "vt %f %f\n", &x, &y);
			textureCordsCount++;
			tempTexturesCoords.emplace_back(x, y);
		}
		else if (line.substr(0, 3) == "vn ") {
			float x = 0, y = 0, z = 0;
			sscanf_s(line.c_str(), "vn %f %f %f\n", &x, &y, &z);
			normalsCount++;
			tempNormals.emplace_back(x, y, z);
		}
		else if (line.substr(0, 2) == "f ") {
			//f 5 3 1 NO UV NO NORMALS 0
			//f 5/1 3/2 1/3 NO NORMALS 3
			//f 5//1 3//1 1//1 NO UVS 6
			//f 5/1/1 3/2/1 1/3/1 TRIANGULATE 6

			//f 1 5 7 3 CLASSIC 0
			//f 1/1 5/2 7/3 3/4 CLASSIC 4
			//f 1//1 5//1 7//1 3//1 CLASSIC 8
			//f 1/1/1 5/2/1 7/3/1 3/4/1 CLASSIC 8
			int count = 0;
			for (int i = 0; i < line.size() - 1; i++)
			{
				if (line[i] == '/')
					count++;
				if (line[i] == '/' && line[i + 1] == '/')
				{
					count = 6;
					hasNoUv = true;//TODO maybe add a break?
					break;
				}
			}

			int v1 = 0, v2 = 0, v3 = 0;
			int vt1 = 0, vt2 = 0, vt3 = 0;
			int vn1 = 0, vn2 = 0, vn3 = 0;
			if (count == 0)
			{
				sscanf_s(line.c_str(), "f %d %d %d\n", &v1, &v2, &v3); //For no uv no normals
				hasNoNormals = true;
				hasNoUv = true;
			}
			else if (count == 3)
			{
				hasNoNormals = true;
				sscanf_s(line.c_str(), "f %d/%d %d/%d %d/%d\n", &v1, &vt1, &v2, &vt2, &v3, &vt3); //For no normals
			}
			else if (count == 6)
			{
				if (hasNoUv)
					sscanf_s(line.c_str(), "f %d//%d %d//%d %d//%d\n", &v1, &vn1, &v2, &vn2, &v3, &vn3); //For no uv
				else
					sscanf_s(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d\n", &v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3); //For classic
			}

			indicesCount += 3;
			//tempIndices.emplace_back(v1, v2, v3);
			vertexIndices.push_back(v1);
			vertexIndices.push_back(v2);
			vertexIndices.push_back(v3);

			textureIndices.push_back(vt1);
			textureIndices.push_back(vt2);
			textureIndices.push_back(vt3);

			normalsIndices.push_back(vn1);
			normalsIndices.push_back(vn2);
			normalsIndices.push_back(vn3);
		}
		text += line + '\n';
	}

	//Close the file
	file.close();

	int byteCount = 3;
	if (!hasNoNormals)
		byteCount += 3;
	if (!hasNoUv)
		byteCount += 2;
	this->verticesCount = indicesCount * (byteCount);
	this->vertices = (float*)calloc(this->verticesCount, sizeof(float));
	this->indicesCount = indicesCount;
	this->indices = (unsigned int*)calloc(this->indicesCount, sizeof(unsigned int));

	CreateBuffers(!hasNoUv, !hasNoNormals);

	for (int i = 0; i < vertexIndices.size(); i++)
	{
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int textureIndex = textureIndices[i];
		unsigned int normalIndices = normalsIndices[i];
		int test = 0;
		this->vertices[i * byteCount] = tempVertices.at(vertexIndex - 1).x;
		this->vertices[i * byteCount + ++test] = tempVertices.at(vertexIndex - 1).y;
		this->vertices[i * byteCount + ++test] = tempVertices.at(vertexIndex - 1).z;
		if (!hasNoUv)
		{
			this->vertices[i * byteCount + ++test] = tempTexturesCoords.at(textureIndex - 1).x;
			this->vertices[i * byteCount + ++test] = tempTexturesCoords.at(textureIndex - 1).y;
		}
		if (!hasNoNormals)
		{
			this->vertices[i * byteCount + ++test] = tempNormals.at(normalIndices - 1).x;
			this->vertices[i * byteCount + ++test] = tempNormals.at(normalIndices - 1).y;
			this->vertices[i * byteCount + ++test] = tempNormals.at(normalIndices - 1).z;
		}
	}
	OnLoadFinished();
}

Mesh::~Mesh()
{
	AssetManager::RemoveMesh(this);

	free(vertices);
	free(indices);
	glDeleteVertexArrays(1, &vertexArrayBuffer);
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &indiceBuffer);
}

void Mesh::OnLoadFinished()
{
	AssetManager::AddMesh(this);
	//Graphics::allMesh.push_back(this);
}

void Mesh::LoadMesh(float vertices[], unsigned int indices[]) {
	//Fill vertices
	for (int i = 0; i < verticesCount; i++)
	{
		this->vertices[i] = vertices[i];
	}

	//Fill indices
	for (int i = 0; i < indicesCount; i++)
	{
		this->indices[i] = indices[i];
	}
}

void Mesh::DrawModel() {
	if (gameObject != nullptr && gameObject->active)
	{
		glBindVertexArray(vertexArrayBuffer);//??????????????????
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer); //Set the current GL_ARRAY_BUFFER

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiceBuffer); //Set the current GL_ARRAY_BUFFER

		UpdateShader();
		glBufferData(GL_ARRAY_BUFFER, verticesCount * sizeof(*vertices), vertices, GL_STATIC_DRAW);//Put vertice in the array buffer
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesCount * sizeof(*indices), indices, GL_STATIC_DRAW);//Put vertice in the array buffer
		//glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0);

		glDrawArrays(GL_TRIANGLES, 0, verticesCount);
		glBindVertexArray(0);
	}
}

void Mesh::UpdateShader() {
	if (shader != nullptr && gameObject != nullptr) {
		shader->Use();
		shader->SetShaderCameraPosition();
		shader->SetShaderProjection();
		shader->SetShaderPosition(gameObject->transform.position);
		shader->SetShaderRotation(gameObject->transform.rotation);
		shader->SetShaderScale(gameObject->transform.scale);
	}
}