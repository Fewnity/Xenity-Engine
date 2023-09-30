#include "wavefront_loader.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "../../../xenity.h"
#include "../../graphics/3d_graphics/mesh_data.h"

#ifdef __PSP__
#include <pspkernel.h>
#endif

using namespace std;

bool WavefrontLoader::LoadFromRawData(std::shared_ptr <MeshData> mesh)
{
	Debug::Print("Loading mesh: " + mesh->file->GetPath());

	// Open file
	ifstream file;
	file.open(mesh->file->GetPath());

	// Print error if the file can't be read
	if (file.fail())
	{
		Debug::PrintError("Mesh loading error. Path: " + mesh->file->GetPath());
		return false;
	}

	bool verticesFound = false;
	bool currentMeshFilled = false;

	std::vector<SubMesh*> submeshes;
	int currentSubMesh = -1;

	std::vector<Vector3> tempVertices;
	std::vector<Vector2> tempTexturesCoords;
	std::vector<Vector3> tempNormals;
	bool hasNoUv = false;
	bool hasNoNormals = false;
	int count = -1;

	// Read file
	std::string line;
	SubMesh* currentSubMeshPtr = nullptr;

	while (std::getline(file, line))
	{
		if (line[0] == 'v')
		{
			if (line[1] == ' ') // Add vertice
			{
				if (!verticesFound)
				{
					verticesFound = true;
					currentMeshFilled = false;

					submeshes.push_back(new SubMesh());
					currentSubMesh++;
					currentSubMeshPtr = submeshes[currentSubMesh];
				}

				float x = 0, y = 0, z = 0;
#if defined(_WIN32) || defined(_WIN64)
				sscanf_s(line.c_str(), "v %f %f %f\n", &x, &y, &z);
#elif defined(__PSP__) || defined(__vita__)
				sscanf(line.c_str(), "v %f %f %f\n", &x, &y, &z);
#endif
				if (currentSubMeshPtr)
					currentSubMeshPtr->verticesCount++;
				tempVertices.emplace_back(Vector3(x, y, z));
			}
			else if (line[2] == ' ')
			{
				verticesFound = false;
				if (line[1] == 't') // Add texture coordinate (UV)
				{
					float x = 0, y = 0;
#if defined(_WIN32) || defined(_WIN64)
					sscanf_s(line.c_str(), "vt %f %f\n", &x, &y);
#elif defined(__PSP__) || defined(__vita__)
					sscanf(line.c_str(), "vt %f %f\n", &x, &y);
#endif
					if (currentSubMeshPtr)
						currentSubMeshPtr->textureCordsCount++;
					tempTexturesCoords.emplace_back(Vector2(x, 1 - y));
				}
				else if (line[1] == 'n') // Add normal
				{
					float x = 0, y = 0, z = 0;
#if defined(_WIN32) || defined(_WIN64)
					sscanf_s(line.c_str(), "vn %f %f %f\n", &x, &y, &z);
#elif defined(__PSP__) || defined(__vita__)
					sscanf(line.c_str(), "vn %f %f %f\n", &x, &y, &z);
#endif
					if (currentSubMeshPtr)
						currentSubMeshPtr->normalsCount++;
					tempNormals.emplace_back(Vector3(x, y, z));
				}
			}
		}
		else if (line[0] == 'f' && line[1] == ' ') // Add indices
		{
			verticesFound = false;
			currentMeshFilled = true;
			// Find the number of param
			// int count = 0;
			if (count == -1)
			{
				count = 0;
				int lineSize = (int)line.size();
				for (int i = 0; i < lineSize - 1; i++)
				{
					if (line[i] == '/')
					{
						count++;
						if (line[(size_t)i + 1] == '/')
						{

							count = 6;
							hasNoUv = true;
							break;
						}
					}
				}
			}

			int v1 = 0, v2 = 0, v3 = 0;
			int vt1 = 0, vt2 = 0, vt3 = 0;
			int vn1 = 0, vn2 = 0, vn3 = 0;
			if (count == 0)
			{
#if defined(_WIN32) || defined(_WIN64)
				sscanf_s(line.c_str(), "f %d %d %d\n", &v1, &v2, &v3); // For no uv no normals
#elif defined(__PSP__) || defined(__vita__)
				sscanf(line.c_str(), "f %d %d %d\n", &v1, &v2, &v3); // For no uv no normals
#endif
				hasNoNormals = true;
				hasNoUv = true;
			}
			else if (count == 3)
			{
				hasNoNormals = true;
#if defined(_WIN32) || defined(_WIN64)
				sscanf_s(line.c_str(), "f %d/%d %d/%d %d/%d\n", &v1, &vt1, &v2, &vt2, &v3, &vt3); // For no normals
#elif defined(__PSP__) || defined(__vita__)
				sscanf(line.c_str(), "f %d/%d %d/%d %d/%d\n", &v1, &vt1, &v2, &vt2, &v3, &vt3); // For no normals
#endif
			}
			else if (count == 6)
			{
				if (hasNoUv)
#if defined(_WIN32) || defined(_WIN64)
					sscanf_s(line.c_str(), "f %d//%d %d//%d %d//%d\n", &v1, &vn1, &v2, &vn2, &v3, &vn3); // For no uv
#elif defined(__PSP__) || defined(__vita__)
					sscanf(line.c_str(), "f %d//%d %d//%d %d//%d\n", &v1, &vn1, &v2, &vn2, &v3, &vn3);						   // For no uv
#endif
				else
#if defined(_WIN32) || defined(_WIN64)
					sscanf_s(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d\n", &v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3); // For classic
#elif defined(__PSP__) || defined(__vita__)
					sscanf(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d\n", &v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3); // For classic
#endif
			}

			if (currentSubMeshPtr)
			{
				currentSubMeshPtr->indicesCount += 3;

				currentSubMeshPtr->vertexIndices.push_back(v1);
				currentSubMeshPtr->vertexIndices.push_back(v2);
				currentSubMeshPtr->vertexIndices.push_back(v3);

				currentSubMeshPtr->textureIndices.push_back(vt1);
				currentSubMeshPtr->textureIndices.push_back(vt2);
				currentSubMeshPtr->textureIndices.push_back(vt3);

				currentSubMeshPtr->normalsIndices.push_back(vn1);
				currentSubMeshPtr->normalsIndices.push_back(vn2);
				currentSubMeshPtr->normalsIndices.push_back(vn3);
			}
		}
		else if (line[0] == 'u' && line[1] == 's' && line[2] == 'e')
		{
			if (!verticesFound && currentMeshFilled)
			{
				currentMeshFilled = false;

				submeshes.push_back(new SubMesh());
				currentSubMesh++;
				currentSubMeshPtr = submeshes[currentSubMesh];
			}
		}
	}
	currentSubMeshPtr = nullptr;

	// Close the file
	file.close();

	//return false;
	mesh->hasUv = !hasNoUv;
	mesh->hasNormal = !hasNoNormals;
	mesh->hasColor = false;

	for (int i = 0; i < currentSubMesh + 1; i++)
	{
		const SubMesh* sub = submeshes[i];
		mesh->AllocSubMesh(sub->indicesCount, sub->indicesCount);
	}

	for (int subMeshIndex = 0; subMeshIndex < currentSubMesh + 1; subMeshIndex++)
	{
		const SubMesh* submesh = submeshes[subMeshIndex];
		// Push vertices in the right order
		int vertexIndicesSize = (int)submesh->vertexIndices.size();
		for (int i = 0; i < vertexIndicesSize; i++)
		{
			unsigned int vertexIndex = submesh->vertexIndices[i] - 1;
			unsigned int textureIndex = submesh->textureIndices[i] - 1;

			const Vector3& vertice = tempVertices.at(vertexIndex);
			if (!mesh->hasNormal)
			{
				if (!mesh->hasUv)
				{
					mesh->AddVertex(
						vertice.x, vertice.y, vertice.z, i, subMeshIndex);
				}
				else
				{
					const Vector2& uv = tempTexturesCoords.at(textureIndex);
					mesh->AddVertex(
						uv.x, uv.y,
						vertice.x, vertice.y, vertice.z, i, subMeshIndex);
				}
			}
			else
			{
				unsigned int normalIndices = submesh->normalsIndices[i] - 1;
				const Vector3& normal = tempNormals.at(normalIndices);
				if (!mesh->hasUv)
				{
					mesh->AddVertex(
						normal.x, normal.y, normal.z,
						vertice.x, vertice.y, vertice.z, i, subMeshIndex);
				}
				else
				{
					const Vector2& uv = tempTexturesCoords.at(textureIndex);
					mesh->AddVertex(
						uv.x, uv.y,
						normal.x, normal.y, normal.z,
						vertice.x, vertice.y, vertice.z, i, subMeshIndex);

				}
			}
			mesh->subMeshes[subMeshIndex]->indices[i] = i;
		}
	}

	// Free memory
	for (int i = 0; i < currentSubMesh + 1; i++)
	{
		submeshes[i]->vertexIndices.clear();
		submeshes[i]->textureIndices.clear();
		submeshes[i]->normalsIndices.clear();
		delete submeshes[i];
	}
	submeshes.clear();

#ifdef __PSP__
	sceKernelDcacheWritebackInvalidateAll(); // Very important
#endif

	mesh->isLoading = false;

	return true;
}