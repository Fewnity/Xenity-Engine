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
		unsigned char* data = file->ReadAllBinary(size);
		file->Close();

		// Load the mesh with assimp
		Assimp::Importer importer;
		// aiProcess_Triangulate because we want to have only triangles, some submeshes mix triangles and quads and I don't know how to handle that
		unsigned int flags = aiProcess_RemoveComponent | aiProcess_PreTransformVertices | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_Triangulate;
		const std::string fileExtension = file->GetFileExtension();
		const aiScene* scene = importer.ReadFileFromMemory(data, size, flags, fileExtension.substr(1).c_str());
		if (!scene)
		{
			const std::string assimpError = importer.GetErrorString();
			Debug::PrintError("[AssimpMeshLoader::LoadMesh] Failed to load the mesh: " + file->GetPath() + " (" + assimpError + ")", true);
			free(data);
			return false;
		}

		// Put assimp submesh data in the engine mesh
		for (size_t subMeshIndex = 0; subMeshIndex < scene->mNumMeshes; subMeshIndex++)
		{
			const aiMesh* assimpMesh = scene->mMeshes[subMeshIndex];

			const bool hasNormals = assimpMesh->HasNormals();
			const bool hasUVs = assimpMesh->HasTextureCoords(0);
			const bool hasFaces = assimpMesh->HasFaces();
			const bool hasColors = assimpMesh->HasVertexColors(0);

			size_t verticesPerFace = 0;
			if (hasFaces)
			{
				verticesPerFace = assimpMesh->mFaces[0].mNumIndices;
			}

			VertexDescriptorList vertexDescriptorList;
			VertexElements vertexDescriptor = VertexElements::POSITION_32_BITS;
			if (hasUVs)
			{
				vertexDescriptor = (VertexElements)((uint32_t)vertexDescriptor | (uint32_t)VertexElements::UV_32_BITS);
				vertexDescriptorList.AddVertexDescriptor(VertexElements::UV_32_BITS);
			}
			if (hasColors)
			{
				vertexDescriptor = (VertexElements)((uint32_t)vertexDescriptor | (uint32_t)VertexElements::COLOR);
				vertexDescriptorList.AddVertexDescriptor(VertexElements::COLOR);
			}
			if (hasNormals)
			{
				vertexDescriptor = (VertexElements)((uint32_t)vertexDescriptor | (uint32_t)VertexElements::NORMAL_32_BITS);
				vertexDescriptorList.AddVertexDescriptor(VertexElements::NORMAL_32_BITS);
			}
			vertexDescriptorList.AddVertexDescriptor(VertexElements::POSITION_32_BITS);
			mesh.SetVertexDescriptor(vertexDescriptor);

			// PSP for example prefer triangles only for performance
			if (forceNoIndices)
			{
				mesh.m_hasIndices = false;
			}
			else
			{
				mesh.m_hasIndices = hasFaces;
			}

			// Allocate memory
			if (mesh.m_hasIndices)
			{
				mesh.AllocSubMesh(assimpMesh->mNumVertices, assimpMesh->mNumFaces * verticesPerFace, vertexDescriptorList);
			}
			else
			{
				mesh.AllocSubMesh(assimpMesh->mNumFaces * verticesPerFace, 0, vertexDescriptorList);
			}

			// Check if the mesh is	using triangles or quads
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
				// Fill the mesh with the vertex data
				for (size_t vertexIndex = 0; vertexIndex < assimpMesh->mNumVertices; vertexIndex++)
				{
					AddVertex(mesh, assimpMesh, vertexIndex, subMeshIndex, vertexIndex);
				}

				// Fill the mesh with the indices data
				for (size_t faceIndex = 0; faceIndex < assimpMesh->mNumFaces; faceIndex++)
				{
					const aiFace& face = assimpMesh->mFaces[faceIndex];
					for (size_t index = 0; index < verticesPerFace; index++)
					{
						if (mesh.m_subMeshes[subMeshIndex]->isShortIndices) // 16 bits indices
						{
							((unsigned short*)mesh.m_subMeshes[subMeshIndex]->indices)[faceIndex * verticesPerFace + index] = face.mIndices[index];
						}
						else // 32 bits indices
						{
							((unsigned int*)mesh.m_subMeshes[subMeshIndex]->indices)[faceIndex * verticesPerFace + index] = face.mIndices[index];
						}
					}
				}
			}
			else
			{
				size_t meshVertexIndex = 0;
				for (size_t faceIndex = 0; faceIndex < assimpMesh->mNumFaces; faceIndex++)
				{
					for (size_t faceVertexIndex = 0; faceVertexIndex < verticesPerFace; faceVertexIndex++)
					{
						const size_t assimpVertexIndex = assimpMesh->mFaces[faceIndex].mIndices[faceVertexIndex];
						AddVertex(mesh, assimpMesh, assimpVertexIndex, subMeshIndex, meshVertexIndex);
						meshVertexIndex++;
					}
				}
			}
		}
		free(data);
	}

	return true;
}

void AssimpMeshLoader::AddVertex(MeshData& mesh, const aiMesh* assimpMesh, unsigned int assimpVertexIndex, unsigned int subMeshIndex, unsigned int meshVertexIndex)
{
	const aiVector3D& vertex = assimpMesh->mVertices[assimpVertexIndex];
	const bool hasNormals = assimpMesh->HasNormals();
	const bool hasUVs = assimpMesh->HasTextureCoords(0);
	const bool hasColors = assimpMesh->HasVertexColors(0);

	if (hasNormals)
	{
		const aiVector3D& normals = assimpMesh->mNormals[assimpVertexIndex];
		mesh.AddNormal(normals.x, normals.y, normals.z, meshVertexIndex, subMeshIndex);
	}
	if (hasUVs)
	{
		const aiVector3D& uv = assimpMesh->mTextureCoords[0][assimpVertexIndex];
		mesh.AddUV(uv.x, uv.y, meshVertexIndex, subMeshIndex);
	}
	if (hasColors)
	{
		const aiColor4D& color = assimpMesh->mColors[0][assimpVertexIndex];
		mesh.AddColor(Color::CreateFromRGBAFloat(color.r, color.g, color.b, color.a), meshVertexIndex, subMeshIndex);
	}
	mesh.AddPosition(vertex.x, vertex.y, vertex.z, meshVertexIndex, subMeshIndex);
}
