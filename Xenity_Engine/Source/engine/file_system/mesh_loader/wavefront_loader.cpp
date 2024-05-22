// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "wavefront_loader.h"

#include <engine/debug/debug.h>
#include <engine/file_system/file.h>
#include <engine/file_system/file_system.h>

#include <engine/vectors/vector2.h>
#include <engine/vectors/vector3.h>

#include <engine/graphics/3d_graphics/mesh_data.h>

#if defined(__PSP__)
#include <pspkernel.h>
#endif

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

bool WavefrontLoader::LoadFromRawData(const std::shared_ptr<MeshData>& mesh)
{
	DXASSERT(mesh != nullptr, "[WavefrontLoader::LoadFromRawData] mesh is nullptr")

	std::shared_ptr<File>& file = mesh->file;
	Debug::Print("Loading mesh: " + file->GetPath(), true);

	const bool opened = file->Open(FileMode::ReadOnly);
	if (opened)
	{
		const std::string allString = file->ReadAll();
		file->Close();
		const int textSize = allString.size();

		bool verticesFound = false;
		bool currentMeshFilled = false;

		std::vector<SubMesh*> submeshes;
		int currentSubMesh = -1;

		std::vector<Vector3> tempVertices;
		std::vector<Vector2> tempTexturesCoords;
		std::vector<Vector3> tempNormals;
		int tempVerticesCount = 0;
		int tempTexturesCoordsCount = 0;
		int tempNormalsCount = 0;
		bool hasNoUv = false;
		bool hasNoNormals = false;
		int count = -1;
		bool notSupported = false;

		std::string mtlFile = "";
		std::string currentMaterial = "";

		// Read file
		int lastLine = 0;
		std::string line = "";
		SubMesh* currentSubMeshPtr = nullptr;

		for (int i = lastLine; i < textSize; i++)
		{
			if (allString[i] == '\n')
			{
				line = allString.substr(0, i);
				lastLine = i + 1;
				break;
			}
		}

		bool stop = false;
		while (true)
		{
			for (int i = lastLine; i < textSize; i++)
			{
				if (i == textSize - 1)
				{
					stop = true;
				}
				if (allString[i] == '\n')
				{
					line = allString.substr(lastLine, i - lastLine);
					lastLine = i + 1;
					break;
				}
			}
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
#elif defined(__PSP__) || defined(__vita__) || defined(_EE)
					sscanf(line.c_str(), "v %f %f %f\n", &x, &y, &z);
#endif
					if (currentSubMeshPtr)
						currentSubMeshPtr->verticesCount++;
					tempVertices.emplace_back(Vector3(x, y, z));
					tempVerticesCount++;
				}
				else if (line[2] == ' ')
				{
					verticesFound = false;
					if (line[1] == 't') // Add texture coordinate (UV)
					{
						float x = 0, y = 0;
#if defined(_WIN32) || defined(_WIN64)
						sscanf_s(line.c_str(), "vt %f %f\n", &x, &y);
#elif defined(__PSP__) || defined(__vita__) || defined(_EE)
						sscanf(line.c_str(), "vt %f %f\n", &x, &y);
#endif
						if (currentSubMeshPtr)
							currentSubMeshPtr->textureCordsCount++;
						tempTexturesCoords.emplace_back(Vector2(x, 1 - y));
						tempTexturesCoordsCount++;
					}
					else if (line[1] == 'n') // Add normal
					{
						float x = 0, y = 0, z = 0;
#if defined(_WIN32) || defined(_WIN64)
						sscanf_s(line.c_str(), "vn %f %f %f\n", &x, &y, &z);
#elif defined(__PSP__) || defined(__vita__) || defined(_EE)
						sscanf(line.c_str(), "vn %f %f %f\n", &x, &y, &z);
#endif
						if (currentSubMeshPtr)
							currentSubMeshPtr->normalsCount++;
						tempNormals.emplace_back(Vector3(x, y, z));
						tempNormalsCount++;
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
					const int lineSize = (int)line.size();
					int spaceCount = 0;
					for (int i = 0; i < lineSize - 1; i++)
					{
						if (line[i] == '/')
						{
							count++;
							/*if (line[(size_t)i + 1] == '/')
							{
								count = 6;
								hasNoUv = true;
								break;
							}*/
						}
						else if (line[i] == ' ')
						{
							spaceCount++;
						}
					}
					if (spaceCount == 4)
					{
						notSupported = true;
						break;
					}
				}

				int v1 = 0, v2 = 0, v3 = 0;
				int vt1 = 0, vt2 = 0, vt3 = 0;
				int vn1 = 0, vn2 = 0, vn3 = 0;
				if (count == 0)
				{
#if defined(_WIN32) || defined(_WIN64)
					sscanf_s(line.c_str(), "f %d %d %d\n", &v1, &v2, &v3); // For no uv no normals
#elif defined(__PSP__) || defined(__vita__) || defined(_EE)
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
#elif defined(__PSP__) || defined(__vita__) || defined(_EE)
					sscanf(line.c_str(), "f %d/%d %d/%d %d/%d\n", &v1, &vt1, &v2, &vt2, &v3, &vt3); // For no normals
#endif
				}
				else if (count == 6)
				{
					if (hasNoUv)
#if defined(_WIN32) || defined(_WIN64)
						sscanf_s(line.c_str(), "f %d//%d %d//%d %d//%d\n", &v1, &vn1, &v2, &vn2, &v3, &vn3); // For no uv
#elif defined(__PSP__) || defined(__vita__) || defined(_EE)
						sscanf(line.c_str(), "f %d//%d %d//%d %d//%d\n", &v1, &vn1, &v2, &vn2, &v3, &vn3);						   // For no uv
#endif
					else
#if defined(_WIN32) || defined(_WIN64)
						sscanf_s(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d\n", &v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3); // For classic
#elif defined(__PSP__) || defined(__vita__) || defined(_EE)
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
			else if (line[0] == 'u' && line[1] == 's' && line[2] == 'e' && line[3] == 'm' && line[4] == 't' && line[5] == 'l') // Find the material
			{
				// Create a new submesh before reading faces data
				if (!verticesFound && currentMeshFilled)
				{
					currentMeshFilled = false;

					submeshes.push_back(new SubMesh());
					currentSubMesh++;
					currentSubMeshPtr = submeshes[currentSubMesh];
				}
				/*int endOffset = 8;
				if (line[line.size() - 1] == '\r') 
				{
					endOffset = 7;
				}*/
				//currentMaterial = line.substr(7, line.size() - 7);
			}
			//else if (line[0] == 'm' && line[1] == 't' && line[2] == 'l' && line[3] == 'l' && line[4] == 'i' && line[5] == 'b') // Find the mtl file
			//{
			//	// Get the name of the mtl file
			//	mtlFile = line.substr(7, line.size() - 7);
			//}

			if (stop)
			{
				break;
			}
		}

		if (!notSupported)
		{
			stop = false;
			currentSubMeshPtr = nullptr;

			mesh->hasUv = !hasNoUv;
			mesh->hasNormal = !hasNoNormals;
			mesh->hasColor = false;
#if defined(__PSP__)
			mesh->hasIndices = false; // Disable indices on psp, this will improve performances
#else
			mesh->hasIndices = true;
#endif
			for (int i = 0; i < currentSubMesh + 1; i++)
			{
				const SubMesh* sub = submeshes[i];
				mesh->AllocSubMesh(sub->indicesCount, sub->indicesCount);
			}

			/*if(!mtlFile.empty())
				ReadMtlFile(mesh->file->GetFolderPath() + mtlFile);*/

			for (int subMeshIndex = 0; subMeshIndex < currentSubMesh + 1; subMeshIndex++)
			{
				const SubMesh* submesh = submeshes[subMeshIndex];
				// Push vertices in the right order
				const int vertexIndicesSize = (int)submesh->vertexIndices.size();
				for (int i = 0; i < vertexIndicesSize; i++)
				{
					const unsigned int vertexIndex = submesh->vertexIndices[i] - 1;
					unsigned int textureIndex = 0;

					if (vertexIndex >= tempVerticesCount)
					{
						stop = true;
						break;
					}
					if (mesh->hasUv)
					{
						textureIndex = submesh->textureIndices[i] - 1;
						if (textureIndex >= tempTexturesCoordsCount)
						{
							stop = true;
							break;
						}
					}
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
						const unsigned int normalIndices = submesh->normalsIndices[i] - 1;
						if (normalIndices >= tempNormalsCount)
						{
							stop = true;
							break;
						}
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
					if (mesh->hasIndices)
						mesh->subMeshes[subMeshIndex]->indices[i] = i;
				}
				if (stop)
				{
					break;
				}
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

#if defined(__PSP__)
		sceKernelDcacheWritebackInvalidateAll(); // Very important
#endif

		mesh->SetIsLoading(false);
		if (notSupported)
		{
			Debug::PrintError("[WavefrontLoader::LoadFromRawData] Mesh loading error. Only triangulated meshes are supported. Path: " + mesh->file->GetPath());
			return false;
		}
	}
	else
	{
		// Print error if the file can't be read
		Debug::PrintError("[WavefrontLoader::LoadFromRawData] Mesh loading error. Path: " + mesh->file->GetPath());
		return false;
	}
	return true;
}

bool WavefrontLoader::ReadMtlFile(const std::string& path)
{
	return false;
	Debug::Print("Loading mtl file: " + path, true);
	std::shared_ptr<File> file = FileSystem::MakeFile(path);
	const bool opened = file->Open(FileMode::ReadOnly);
	if (opened)
	{
		const std::string allString = file->ReadAll();
		file->Close();
		const int textSize = allString.size();
		Debug::Print("[WavefrontLoader::ReadMtlFile] Mtl file: " + allString);
	}
	else 
	{
		Debug::PrintError("[WavefrontLoader::ReadMtlFile] Mtl file loading error. Path: " + path);
	}
	return false;
}
