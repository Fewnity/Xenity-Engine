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

bool WavefrontLoader::LoadFromRawData(MeshData* mesh)
{
	std::string finalpath = "";
#ifdef __vita__
	finalpath += "ux0:";
#endif
	//finalpath += FileSystem::fileSystem->modelsPath;
	finalpath += mesh->file->GetPath();
	Debug::Print("Loading mesh: " + finalpath);

	// Open file
	ifstream file;
	file.open(finalpath);

	// Print error if the file can't be read
	if (file.fail())
	{
		Debug::PrintError("Mesh loading error. Path: " + finalpath);
		return false;
	}

	bool verticesFound = false;
	bool currentMeshFilled = false;

	std::vector<SubMesh> submeshes;
	//std::vector<Data> submeshesData;
	int currentSubMesh = -1;

	int verticesCount = 0;
	int normalsCount = 0;
	int textureCordsCount = 0;

	vector<Vector3> tempVertices;
	vector<Vector2> tempTexturesCoords;
	vector<Vector3> tempNormals;
	bool hasNoUv = false;
	bool hasNoNormals = false;
	int count = -1;

	// Read file
	std::string line;
	while (std::getline(file, line))
	{
		if (line[0] == 'v')
		{
			if (line[1] == ' ') // Add vertice
			{
				if (!verticesFound)
				{
					Debug::PrintWarning("New data found A");
					verticesFound = true;
					currentMeshFilled = false;

					submeshes.push_back(SubMesh());
					currentSubMesh++;
				}

				float x = 0, y = 0, z = 0;
#if defined(_WIN32) || defined(_WIN64)
				sscanf_s(line.c_str(), "v %f %f %f\n", &x, &y, &z);
#elif defined(__PSP__) || defined(__vita__)
				sscanf(line.c_str(), "v %f %f %f\n", &x, &y, &z);
#endif
				verticesCount++;
				tempVertices.emplace_back(x, y, z);
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
					textureCordsCount++;
					tempTexturesCoords.emplace_back(x, 1 - y);
				}
				else if (line[1] == 'n') // Add normal
				{
					float x = 0, y = 0, z = 0;
#if defined(_WIN32) || defined(_WIN64)
					sscanf_s(line.c_str(), "vn %f %f %f\n", &x, &y, &z);
#elif defined(__PSP__) || defined(__vita__)
					sscanf(line.c_str(), "vn %f %f %f\n", &x, &y, &z);
#endif
					normalsCount++;
					tempNormals.emplace_back(x, y, z);
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

			submeshes[currentSubMesh].indicesCount += 3;

			submeshes[currentSubMesh].vertexIndices.push_back(v1);
			submeshes[currentSubMesh].vertexIndices.push_back(v2);
			submeshes[currentSubMesh].vertexIndices.push_back(v3);

			submeshes[currentSubMesh].textureIndices.push_back(vt1);
			submeshes[currentSubMesh].textureIndices.push_back(vt2);
			submeshes[currentSubMesh].textureIndices.push_back(vt3);

			submeshes[currentSubMesh].normalsIndices.push_back(vn1);
			submeshes[currentSubMesh].normalsIndices.push_back(vn2);
			submeshes[currentSubMesh].normalsIndices.push_back(vn3);
		}
		else if (line[0] == 'u' && line[1] == 's' && line[2] == 'e')
		{
			if (!verticesFound && currentMeshFilled)
			{
				currentMeshFilled = false;

				submeshes.push_back(SubMesh());
				currentSubMesh++;

				Debug::PrintWarning("New submesh found B");
			}
		}
	}

	std::string smC = "subMeshCount " + std::to_string(currentSubMesh+1);
	Debug::PrintWarning(smC);

	// Close the file
	file.close();

	mesh->hasUv = !hasNoUv;
	mesh->hasNormal = !hasNoNormals;
	mesh->hasColor = false;

	int indiceCount = 0;
	for (int i = 0; i < currentSubMesh + 1; i++)
	{
		indiceCount += submeshes[i].indicesCount;
	}

	mesh->AllocData(indiceCount);

	for (int i = 0; i < currentSubMesh+1; i++)
	{
		mesh->AllocSubMesh(submeshes[i].indicesCount);
	}
	int totalIndice = 0;

	for (int subMeshIndex = 0; subMeshIndex < currentSubMesh + 1; subMeshIndex++)
	{
		// Push vertices in the right order
		int vertexIndicesSize = (int)submeshes[subMeshIndex].vertexIndices.size();
		for (int i = 0; i < vertexIndicesSize; i++)
		{
			unsigned int vertexIndex = submeshes[subMeshIndex].vertexIndices[i] - 1;
			unsigned int textureIndex = submeshes[subMeshIndex].textureIndices[i] - 1;
			unsigned int normalIndices = submeshes[subMeshIndex].normalsIndices[i] - 1;

			if (!mesh->hasNormal)
			{
				if (!mesh->hasUv)
				{
					mesh->AddVertex(
						tempVertices.at(vertexIndex).x, tempVertices.at(vertexIndex).y, tempVertices.at(vertexIndex).z, totalIndice);
				}
				else
				{
					mesh->AddVertex(
						tempTexturesCoords.at(textureIndex).x, tempTexturesCoords.at(textureIndex).y,
						tempVertices.at(vertexIndex).x, tempVertices.at(vertexIndex).y, tempVertices.at(vertexIndex).z, totalIndice);
				}
			}
			else
			{
				if (!mesh->hasUv)
				{
					mesh->AddVertex(
						tempNormals.at(normalIndices).x, tempNormals.at(normalIndices).y, tempNormals.at(normalIndices).z,
						tempVertices.at(vertexIndex).x, tempVertices.at(vertexIndex).y, tempVertices.at(vertexIndex).z, totalIndice);
				}
				else
				{
					mesh->AddVertex(
						tempTexturesCoords.at(textureIndex).x, tempTexturesCoords.at(textureIndex).y,
						tempNormals.at(normalIndices).x, tempNormals.at(normalIndices).y, tempNormals.at(normalIndices).z,
						tempVertices.at(vertexIndex).x, tempVertices.at(vertexIndex).y, tempVertices.at(vertexIndex).z, totalIndice);
				}
			}
			mesh->subMeshes[subMeshIndex]->indices[i] = totalIndice;
			totalIndice++;
		}
	}
#ifdef __PSP__
	sceKernelDcacheWritebackInvalidateAll(); // Very important
#endif
	return true;
}
