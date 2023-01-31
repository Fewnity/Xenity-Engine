#include "wavefront_loader.h"
#include "../../main.h"
#include <fstream>
#include <iostream>
#include <vector>
#include "../../vectors/vector2.h"
#include "../../vectors/vector3.h"
#include "../../graphics/mesh.h"
using namespace std;

std::string modelsPath = R"(Xenity_Engine\Source\models\)"; //TODO remove this

void WavefrontLoader::LoadMesh(MeshData* mesh, std::string filePath)
{
	std::string finalpath = PROJECT_FOLDER + modelsPath;
	//Open file
	ifstream file;
	file.open(finalpath + filePath);

	//Print error if the file can't be read
	if (file.fail()) {
		std::cout << "\033[31mModel load error. Path : \"" << finalpath + filePath << "\"\033[0m" << std::endl;
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
	std::string line;
	while (std::getline(file, line)) {
		//printf(line.c_str());
		//printf("%s", line);
		if (line[0] == 'v') {
			if (line[1] == ' ') {
				float x = 0, y = 0, z = 0;
				sscanf_s(line.c_str(), "v %f %f %f\n", &x, &y, &z);
				verticesCount++;
				tempVertices.emplace_back(x, y, z);
			}
			else if (line[2] == ' ') {
				if (line[1] == 't') {
					float x = 0, y = 0;
					sscanf_s(line.c_str(), "vt %f %f\n", &x, &y);
					textureCordsCount++;
					tempTexturesCoords.emplace_back(x, y);
				}
				else if (line[1] == 'n') {
					float x = 0, y = 0, z = 0;
					sscanf_s(line.c_str(), "vn %f %f %f\n", &x, &y, &z);
					normalsCount++;
					tempNormals.emplace_back(x, y, z);
				}
			}
		}
		else if (line[0] == 'f' && line[1] == ' ') {
			//f 5 3 1 NO UV NO NORMALS 0
			//f 5/1 3/2 1/3 NO NORMALS 3
			//f 5//1 3//1 1//1 NO UVS 6
			//f 5/1/1 3/2/1 1/3/1 TRIANGULATE 6

			//f 1 5 7 3 CLASSIC 0
			//f 1/1 5/2 7/3 3/4 CLASSIC 4
			//f 1//1 5//1 7//1 3//1 CLASSIC 8
			//f 1/1/1 5/2/1 7/3/1 3/4/1 CLASSIC 8
			int count = 0;
			int lineSize = line.size();
			for (int i = 0; i < lineSize - 1; i++)
			{
				if (line[i] == '/')
				{
					count++;
					if (line[i + 1] == '/') {

						count = 6;
						hasNoUv = true;//TODO maybe add a break?
						break;
					}
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

	//Close the file
	file.close();

	int byteCount = 3;
	if (!hasNoNormals)
		byteCount += 3;
	if (!hasNoUv)
		byteCount += 2;
	mesh->verticesCount = indicesCount * (byteCount);
	mesh->vertices = (float*)calloc(mesh->verticesCount, sizeof(float));
	mesh->indicesCount = indicesCount;
	mesh->indices = (unsigned int*)calloc(mesh->indicesCount, sizeof(unsigned int));

	mesh->hasUv = !hasNoUv;
	mesh->hasNormal = !hasNoNormals;

	int vertexIndicesSize = vertexIndices.size();
	for (int i = 0; i < vertexIndicesSize; i++)
	{
		unsigned int vertexIndex = vertexIndices[i] - 1;
		unsigned int textureIndex = textureIndices[i] - 1;
		unsigned int normalIndices = normalsIndices[i] - 1;
		int test = 0;
		int index = i * byteCount;
		mesh->vertices[index] = tempVertices.at(vertexIndex).x;
		mesh->vertices[index + ++test] = tempVertices.at(vertexIndex).y;
		mesh->vertices[index + ++test] = tempVertices.at(vertexIndex).z;
		if (!hasNoUv)
		{
			mesh->vertices[index + ++test] = tempTexturesCoords.at(textureIndex).x;
			mesh->vertices[index + ++test] = tempTexturesCoords.at(textureIndex).y;
		}
		if (!hasNoNormals)
		{
			mesh->vertices[index + ++test] = tempNormals.at(normalIndices).x;
			mesh->vertices[index + ++test] = tempNormals.at(normalIndices).y;
			mesh->vertices[index + ++test] = tempNormals.at(normalIndices).z;
		}
	}
}
