#include "wavefront_loader.h"
#include <fstream>
#include <iostream>
#include <vector>
#include "../../../xenity.h"
#include "../../graphics/3d_graphics/mesh_data.h"

#ifdef __PSP__
#include <pspkernel.h>
#endif

using namespace std;

MeshData *WavefrontLoader::LoadFromRawData(const std::string filePath)
{
	Debug::Print("Loading mesh...");

	std::string finalpath = "";
#ifdef __vita__
	finalpath += "ux0:";
#endif
	finalpath += FileSystem::fileSystem->modelsPath;

	// Open file
	ifstream file;
	file.open(finalpath + filePath);

	// Print error if the file can't be read
	if (file.fail())
	{
		Debug::Print("Mesh loading error. Path: " + finalpath + filePath);

		return nullptr;
		// std::cout << "\033[31mModel load error. Path : \"" << finalpath + filePath << "\"\033[0m" << std::endl;
	}
	Debug::Print("Mesh loading started");
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
				// sscanf_s(line.c_str(), "v %f %f %f\n", &x, &y, &z);
				sscanf(line.c_str(), "v %f %f %f\n", &x, &y, &z);
				verticesCount++;
				tempVertices.emplace_back(x, y, z);
			}
			else if (line[2] == ' ')
			{
				if (line[1] == 't') // Add texture coordinate (UV)
				{
					float x = 0, y = 0;
					// sscanf_s(line.c_str(), "vt %f %f\n", &x, &y);
					sscanf(line.c_str(), "vt %f %f\n", &x, &y);
					textureCordsCount++;
					tempTexturesCoords.emplace_back(x, 1 - y);
				}
				else if (line[1] == 'n') // Add normal
				{
					float x = 0, y = 0, z = 0;
					// sscanf_s(line.c_str(), "vn %f %f %f\n", &x, &y, &z);
					sscanf(line.c_str(), "vn %f %f %f\n", &x, &y, &z);
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
						if (line[i + 1] == '/')
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
				// sscanf_s(line.c_str(), "f %d %d %d\n", &v1, &v2, &v3); // For no uv no normals
				sscanf(line.c_str(), "f %d %d %d\n", &v1, &v2, &v3); // For no uv no normals
				hasNoNormals = true;
				hasNoUv = true;
			}
			else if (count == 3)
			{
				hasNoNormals = true;
				sscanf(line.c_str(), "f %d/%d %d/%d %d/%d\n", &v1, &vt1, &v2, &vt2, &v3, &vt3); // For no normals
																								// sscanf_s(line.c_str(), "f %d/%d %d/%d %d/%d\n", &v1, &vt1, &v2, &vt2, &v3, &vt3); // For no normals

				// mesh->AddVertice(
				// 	tempTexturesCoords.at(textureIndex).x, tempTexturesCoords.at(textureIndex).y,
				// 	0xFFFFFFFF, tempVertices.at(vertexIndex).x, tempVertices.at(vertexIndex).y, tempVertices.at(vertexIndex).z, vertexIndex);

				// mesh->indices[i] = vertexIndex;
			}
			else if (count == 6)
			{
				if (hasNoUv)
					// sscanf_s(line.c_str(), "f %d//%d %d//%d %d//%d\n", &v1, &vn1, &v2, &vn2, &v3, &vn3); // For no uv
					sscanf(line.c_str(), "f %d//%d %d//%d %d//%d\n", &v1, &vn1, &v2, &vn2, &v3, &vn3); // For no uv
				else
					// sscanf_s(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d\n", &v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3); // For classic
					sscanf(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d\n", &v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3); // For classic
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

	MeshData *mesh = new MeshData(indicesCount, indicesCount);

	// Alloc memory for vertices and indices
	// mesh->verticesCount = indicesCount * (byteCount);
	// mesh->vertices = (float *)calloc(mesh->verticesCount, sizeof(float));
	// mesh->indicesCount = indicesCount;
	// mesh->indices = (unsigned int *)calloc(mesh->indicesCount, sizeof(unsigned int));

	mesh->hasUv = !hasNoUv;
	mesh->hasNormal = !hasNoNormals;

	// Push vertices in the right order
	int vertexIndicesSize = (int)vertexIndices.size();
	int i2 = 0;
	int i3 = 0;
	int i4 = 0;

	for (int i = 0; i < vertexIndicesSize; i++)
	{
		unsigned int vertexIndex = vertexIndices[i] - 1;
		unsigned int textureIndex = textureIndices[i] - 1;
		unsigned int normalIndices = normalsIndices[i] - 1;
		int test = 0;
		int index = i * byteCount;

		// (float u, float v, unsigned int color, float x, float y, float z, int indice);
		mesh->AddVertice(
			tempTexturesCoords.at(textureIndex).x, tempTexturesCoords.at(textureIndex).y,
			0xFFFFFFFF, tempVertices.at(vertexIndex).x, tempVertices.at(vertexIndex).y, tempVertices.at(vertexIndex).z, i);

		mesh->indices[i] = i;

		// mesh->AddVertice(
		// 	tempTexturesCoords.at(0).x, tempTexturesCoords.at(i2++).y,
		// 	0xFFFFFFFF, tempVertices.at(i3++).x, tempVertices.at(i3++).y, tempVertices.at(i3++).z, i);

		// mesh->vertices[index] = tempVertices.at(vertexIndex).x;
		// mesh->vertices[index + ++test] = tempVertices.at(vertexIndex).y;
		// mesh->vertices[index + ++test] = tempVertices.at(vertexIndex).z;
		// if (!hasNoUv)
		// {
		// 	mesh->vertices[index + ++test] = tempTexturesCoords.at(textureIndex).x;
		// 	mesh->vertices[index + ++test] = tempTexturesCoords.at(textureIndex).y;
		// }
		// if (!hasNoNormals)
		// {
		// 	mesh->vertices[index + ++test] = tempNormals.at(normalIndices).x;
		// 	mesh->vertices[index + ++test] = tempNormals.at(normalIndices).y;
		// 	mesh->vertices[index + ++test] = tempNormals.at(normalIndices).z;
		// }
	}

#ifdef __PSP__
	sceKernelDcacheWritebackInvalidateAll(); // Very important
#endif

	return mesh;
}
