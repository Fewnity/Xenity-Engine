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
		// std::cout << "\033[31mModel load error. Path : \"" << finalpath + filePath << "\"\033[0m" << std::endl;
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
	int count = -1;

	// Read file
	std::string line;
	while (std::getline(file, line))
	{
		if (line[0] == 'v')
		{
			if (line[1] == ' ') // Add vertice
			{
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
				sscanf(line.c_str(), "f %d %d %d\n", &v1, &v2, &v3);														   // For no uv no normals
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
				sscanf(line.c_str(), "f %d/%d %d/%d %d/%d\n", &v1, &vt1, &v2, &vt2, &v3, &vt3);								   // For no normals
#endif
				// mesh->AddVertex(
				// 	tempTexturesCoords.at(textureIndex).x, tempTexturesCoords.at(textureIndex).y,
				// 	0xFFFFFFFF, tempVertices.at(vertexIndex).x, tempVertices.at(vertexIndex).y, tempVertices.at(vertexIndex).z, vertexIndex);

				// mesh->indices[i] = vertexIndex;
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

			indicesCount += 3;

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
	}

	// Close the file
	file.close();

	int byteCount = 3;
	if (!hasNoNormals)
		byteCount += 3;
	if (!hasNoUv)
		byteCount += 2;

	mesh->AllocMesh(indicesCount, indicesCount, false, !hasNoNormals);

	mesh->hasUv = !hasNoUv;
	mesh->hasNormal = !hasNoNormals;

	// Push vertices in the right order
	int vertexIndicesSize = (int)vertexIndices.size();
	for (int i = 0; i < vertexIndicesSize; i++)
	{
		unsigned int vertexIndex = vertexIndices[i] - 1;
		unsigned int textureIndex = textureIndices[i] - 1;
		unsigned int normalIndices = normalsIndices[i] - 1;

		if(!mesh->hasNormal)
		mesh->AddVertex(
			tempTexturesCoords.at(textureIndex).x, tempTexturesCoords.at(textureIndex).y, tempVertices.at(vertexIndex).x, tempVertices.at(vertexIndex).y, tempVertices.at(vertexIndex).z, i);
		else
			mesh->AddVertex(
				tempTexturesCoords.at(textureIndex).x, tempTexturesCoords.at(textureIndex).y, tempNormals.at(normalIndices).x, tempNormals.at(normalIndices).y, tempNormals.at(normalIndices).z,
				tempVertices.at(vertexIndex).x, tempVertices.at(vertexIndex).y, tempVertices.at(vertexIndex).z, i);

		mesh->indices[i] = i;
	}

#ifdef __PSP__
	sceKernelDcacheWritebackInvalidateAll(); // Very important
#endif
	return true;
}
