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

bool AssimpMeshLoader::LoadMesh(MeshData& mesh, const LoadingOptions& options)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	XASSERT(mesh.m_file, "[AssimpMeshLoader::LoadMesh] File is null");
	XASSERT(!mesh.m_file->GetPath().empty(), "[AssimpMeshLoader::LoadMesh] File path is empty");

	const std::shared_ptr<File>& file = mesh.m_file;

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
		unsigned int flags = aiProcess_RemoveComponent | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_Triangulate;
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
			bool hasColors = assimpMesh->HasVertexColors(0);
			if (options.forceColors)
			{
				hasColors = true;
			}

			size_t verticesPerFace = 0;
			if (hasFaces)
			{
				verticesPerFace = assimpMesh->mFaces[0].mNumIndices;
			}

			VertexDescriptor vertexDescriptorList;
			if (hasUVs)
			{
				vertexDescriptorList.AddVertexElement(VertexElement::UV_32_BITS);
			}
			if (hasColors)
			{
				vertexDescriptorList.AddVertexElement(VertexElement::COLOR_4_FLOATS);
			}
			if (hasNormals)
			{
				vertexDescriptorList.AddVertexElement(VertexElement::NORMAL_32_BITS);
			}
			vertexDescriptorList.AddVertexElement(VertexElement::POSITION_32_BITS);

			// PSP for example prefer triangles only for performance
			bool useIndices = false;
			if (options.forceNoIndices)
			{
				useIndices = false;
			}
			else
			{
				useIndices = hasFaces;
			}

			// Allocate memory
			if (useIndices)
			{
				mesh.CreateSubMesh(static_cast<uint32_t>(assimpMesh->mNumVertices), static_cast<uint32_t>(assimpMesh->mNumFaces * verticesPerFace), vertexDescriptorList);
			}
			else
			{
				mesh.CreateSubMesh(static_cast<uint32_t>(assimpMesh->mNumFaces * verticesPerFace), 0, vertexDescriptorList);
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

			if (useIndices)
			{
				// Fill the mesh with the vertex data
				for (size_t vertexIndex = 0; vertexIndex < assimpMesh->mNumVertices; vertexIndex++)
				{
					SetVertex(mesh, options, assimpMesh, vertexIndex, subMeshIndex, vertexIndex);
				}

				// Fill the mesh with the indices data
				for (size_t faceIndex = 0; faceIndex < assimpMesh->mNumFaces; faceIndex++)
				{
					const aiFace& face = assimpMesh->mFaces[faceIndex];
					for (size_t index = 0; index < verticesPerFace; index++)
					{
						mesh.m_subMeshes[subMeshIndex]->SetIndex(faceIndex * verticesPerFace + index, face.mIndices[index]);
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
						SetVertex(mesh, options, assimpMesh, assimpVertexIndex, subMeshIndex, meshVertexIndex);
						meshVertexIndex++;
					}
				}
			}
		}
		free(data);
	}

	return true;
}

void AssimpMeshLoader::SetVertex(MeshData& mesh, const LoadingOptions& options, const aiMesh* assimpMesh, size_t assimpVertexIndex, size_t subMeshIndex, size_t meshVertexIndex)
{
	const aiVector3D& vertex = assimpMesh->mVertices[assimpVertexIndex];
	const bool hasNormals = assimpMesh->HasNormals();
	const bool hasUVs = assimpMesh->HasTextureCoords(0);
	const bool hasColors = assimpMesh->HasVertexColors(0);
	MeshData::SubMesh& subMesh = *mesh.m_subMeshes[subMeshIndex];

	if (hasNormals)
	{
		const aiVector3D& normals = assimpMesh->mNormals[assimpVertexIndex];
		subMesh.SetNormal(normals.x, normals.y, normals.z, static_cast<uint32_t>(meshVertexIndex));
	}
	if (hasUVs)
	{
		const aiVector3D& uv = assimpMesh->mTextureCoords[0][assimpVertexIndex];
		subMesh.SetUV(uv.x, uv.y, static_cast<uint32_t>(meshVertexIndex));
	}
	if (hasColors)
	{
		const aiColor4D& color = assimpMesh->mColors[0][assimpVertexIndex];
		subMesh.SetColor(Color::CreateFromRGBAFloat(color.r, color.g, color.b, color.a), static_cast<uint32_t>(meshVertexIndex));
	}
	else if (options.forceColors)
	{
		const aiColor4D color = aiColor4D(1, 1, 1, 1);
		subMesh.SetColor(Color::CreateFromRGBAFloat(color.r, color.g, color.b, color.a), static_cast<uint32_t>(meshVertexIndex));
	}

	subMesh.SetPosition(vertex.x, vertex.y, vertex.z, static_cast<uint32_t>(meshVertexIndex));
}
