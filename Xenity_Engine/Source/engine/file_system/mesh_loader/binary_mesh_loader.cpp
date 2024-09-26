#include "binary_mesh_loader.h"
#include <engine/graphics/3d_graphics/mesh_data.h>
#include <engine/file_system/file.h>
#include <engine/file_system/file_system.h>
#include <engine/asset_management/project_manager.h>
#include <iostream>
#include <engine/debug/debug.h>

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

bool BinaryMeshLoader::LoadMesh(MeshData& mesh)
{
	unsigned char* fileData = ProjectManager::fileDataBase.GetBitFile().ReadBinary(mesh.m_filePosition, mesh.m_fileSize);
	unsigned char* fileDataOriginalPtr = fileData;

	const VertexElements vertexDescriptor = *(VertexElements*)fileData;
	fileData += sizeof(VertexElements);

	const uint32_t subMeshCount = *(uint32_t*)fileData;
	fileData += sizeof(uint32_t);

#if defined(__PSP__)
	mesh.m_hasIndices = false; // Disable indices on psp, this will improve performances
#else
	mesh.m_hasIndices = true;
#endif
	mesh.m_hasColor = false;

	mesh.SetVertexDescriptor(vertexDescriptor);

	// Read all sub meshes
	for (uint32_t i = 0; i < subMeshCount; i++)
	{
		uint32_t vertice_count;
		uint32_t index_count;
		uint32_t vertexMemSize;
		uint32_t indexMemSize;

		// Use memcpy to avoid alignment issues
		memcpy(&vertice_count, fileData, sizeof(uint32_t));
		fileData += sizeof(uint32_t);
		memcpy(&index_count, fileData, sizeof(uint32_t));
		fileData += sizeof(uint32_t);
		memcpy(&vertexMemSize, fileData, sizeof(uint32_t));
		fileData += sizeof(uint32_t);
		memcpy(&indexMemSize, fileData, sizeof(uint32_t));
		fileData += sizeof(uint32_t);

		mesh.AllocSubMesh(vertice_count, index_count);
		MeshData::SubMesh* subMesh = mesh.m_subMeshes[mesh.m_subMeshCount - 1];

		// Copy vertices data
		memcpy(subMesh->data, fileData, vertexMemSize);
		fileData += vertexMemSize;

		// Copy indices data
		if(mesh.m_hasIndices)
		{
			memcpy(subMesh->indices, fileData, indexMemSize);
		}
		fileData += indexMemSize;
	}

#if defined(__PSP__)
	sceKernelDcacheWritebackInvalidateAll(); // Very important
#endif

	delete[] fileDataOriginalPtr;

	return true;
}
