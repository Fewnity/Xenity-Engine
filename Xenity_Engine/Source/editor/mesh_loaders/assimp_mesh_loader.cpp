#include "assimp_mesh_loader.h"

#include <iostream>
#include <cstring>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <engine/graphics/3d_graphics/mesh_data.h>
#include <engine/file_system/file.h>
#include <engine/file_system/file_system.h>
#include <engine/asset_management/project_manager.h>
#include <engine/debug/debug.h>
#include <engine/debug/stack_debug_object.h>

#if defined(__PSP__)
#include <pspkernel.h>
#endif

/**
* Vertex descriptor - 4 bytes
* Sub mesh count - 4 bytes
* ------ For one sub mesh
* vertice_count - 4 bytes
* index_count - 4 bytes
* vertexMemSize - 4 bytes
* indexMemSize - 4 bytes
* vertice data - vertexMemSize bytes
* indices data - indexMemSize bytes
* ------
*/

bool AssimpMeshLoader::LoadMesh(MeshData& mesh, bool forceNoIndices)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	std::shared_ptr<File>& file = mesh.m_file;

	bool opened = true;
#if defined(EDITOR)
	opened = file->Open(FileMode::ReadOnly);
#endif
	if (opened)
	{
		size_t size = 0;
		unsigned char * data = file->ReadAllBinary(size);
		file->Close();

		Assimp::Importer importer;

		unsigned int flags = aiProcess_RemoveComponent | aiProcess_PreTransformVertices | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_Triangulate;
		const aiScene* scene = importer.ReadFileFromMemory(data, size, flags);
		for (size_t subMeshIndex = 0; subMeshIndex < scene->mNumMeshes; subMeshIndex++)
		{
			aiMesh* assimpMesh = scene->mMeshes[subMeshIndex];
			Debug::Print("Pim:" + std::to_string(assimpMesh->mPrimitiveTypes));
			bool hasNormals = assimpMesh->HasNormals();
			bool hasUVs = assimpMesh->HasTextureCoords(0);
			bool hasFaces = assimpMesh->HasFaces();
			size_t verticesPerFace = 0;
			if (hasFaces)
			{
				verticesPerFace = assimpMesh->mFaces[0].mNumIndices;
			}

			VertexElements vertexDescriptor = VertexElements::POSITION_32_BITS;
			if (hasUVs)
			{
				vertexDescriptor = (VertexElements)((uint32_t)vertexDescriptor | (uint32_t)VertexElements::UV_32_BITS);
			}
			if (hasNormals)
			{
				vertexDescriptor = (VertexElements)((uint32_t)vertexDescriptor | (uint32_t)VertexElements::NORMAL_32_BITS);
			}

			if (forceNoIndices)
			{
				mesh.m_hasIndices = false;
			}
			else 
			{
				mesh.m_hasIndices = hasFaces;
			}

			mesh.SetVertexDescriptor(vertexDescriptor);
			if (mesh.m_hasIndices)
			{
				mesh.AllocSubMesh(assimpMesh->mNumVertices, assimpMesh->mNumFaces * verticesPerFace);
			}
			else 
			{
				mesh.AllocSubMesh(assimpMesh->mNumFaces * verticesPerFace, assimpMesh->mNumFaces * verticesPerFace);

			}
			if (verticesPerFace == 4)
			{
				mesh.m_subMeshes[subMeshIndex]->m_isQuad = true;
			}
			else 
			{
				mesh.m_subMeshes[subMeshIndex]->m_isQuad = false;
			}

			if (mesh.m_hasIndices)
			{
				for (size_t vertexIndex = 0; vertexIndex < assimpMesh->mNumVertices; vertexIndex++)
				{
					AddVertex(mesh, assimpMesh, vertexIndex, subMeshIndex, vertexIndex);
				}

				for (size_t faceIndex = 0; faceIndex < assimpMesh->mNumFaces; faceIndex++)
				{
					aiFace& face = assimpMesh->mFaces[faceIndex];
					for (size_t index = 0; index < face.mNumIndices; index++)
					{
						if (mesh.m_subMeshes[subMeshIndex]->isShortIndices)
						{
							((unsigned short*)mesh.m_subMeshes[subMeshIndex]->indices)[faceIndex * verticesPerFace + index] = face.mIndices[index];
						}
						else
						{
							((unsigned int*)mesh.m_subMeshes[subMeshIndex]->indices)[faceIndex * verticesPerFace + index] = face.mIndices[index];
						}
					}
				}
			}
			else 
			{
				size_t vertexIndex2 = 0;
				for (size_t faceIndex = 0; faceIndex < assimpMesh->mNumFaces; faceIndex++)
				{
					for (size_t faceVertexIndex = 0; faceVertexIndex < verticesPerFace; faceVertexIndex++)
					{
						size_t vertexIndex = assimpMesh->mFaces[faceIndex].mIndices[faceVertexIndex];
						AddVertex(mesh, assimpMesh, vertexIndex, subMeshIndex, vertexIndex2);
						vertexIndex2++;
					}
					//vertexIndex2 += verticesPerFace;
				}
			}
		}
		free(data);
	}

	return true;
}

void AssimpMeshLoader::AddVertex(MeshData& mesh, const aiMesh* assimpMesh, unsigned int vertexIndex, unsigned int subMeshIndex, unsigned int vertexIndex2)
{
	const aiVector3D& vertex = assimpMesh->mVertices[vertexIndex];
	const bool hasNormals = assimpMesh->HasNormals();
	const bool hasUVs = assimpMesh->HasTextureCoords(0);

	if (!hasNormals)
	{
		if (!hasUVs)
		{
			mesh.AddVertex(
				vertex.x, vertex.y, vertex.z, vertexIndex2, subMeshIndex);
		}
		else
		{
			const aiVector3D& uv = assimpMesh->mTextureCoords[0][vertexIndex];
			mesh.AddVertex(
				uv.x, uv.y,
				vertex.x, vertex.y, vertex.z, vertexIndex2, subMeshIndex);
		}
	}
	else
	{
		const aiVector3D& normals = assimpMesh->mNormals[vertexIndex];
		if (!hasUVs)
		{
			mesh.AddVertex(
				normals.x, normals.y, normals.z,
				vertex.x, vertex.y, vertex.z, vertexIndex2, subMeshIndex);
		}
		else
		{
			const aiVector3D& uv = assimpMesh->mTextureCoords[0][vertexIndex];
			mesh.AddVertex(
				uv.x, uv.y,
				normals.x, normals.y, normals.z,
				vertex.x, vertex.y, vertex.z, vertexIndex2, subMeshIndex);

		}
	}
}
