#include "binary_mesh_loader.h"

#include <iostream>
#include <cstring>

#include <engine/graphics/3d_graphics/mesh_data.h>
#include <engine/file_system/file.h>
#include <engine/file_system/file_system.h>
#include <engine/asset_management/project_manager.h>
#include <engine/debug/debug.h>
#include <engine/debug/stack_debug_object.h>
#include <engine/tools/endian_utils.h>

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
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	unsigned char* fileData = ProjectManager::fileDataBase.GetBitFile().ReadBinary(mesh.m_filePosition, mesh.m_fileSize);
	unsigned char* fileDataOriginalPtr = fileData;

	uint32_t subMeshCount = *reinterpret_cast<uint32_t*>(fileData);

#if defined(__PS3__)
	subMeshCount = EndianUtils::SwapEndian(subMeshCount);
#endif // defined(__PS3__)

	fileData += sizeof(uint32_t);

	// Read all sub meshes
	for (uint32_t i = 0; i < subMeshCount; i++)
	{
		// Read vertex descriptor
		uint32_t vertexDescriptorListSize = 0;

		memcpy(&vertexDescriptorListSize, fileData, sizeof(uint32_t));
		fileData += sizeof(uint32_t);

#if defined(__PS3__)
		vertexDescriptorListSize = EndianUtils::SwapEndian(vertexDescriptorListSize);
#endif

		VertexDescriptor vertexDescriptorList;

		for (size_t vertexDescIndex = 0; vertexDescIndex < vertexDescriptorListSize; vertexDescIndex++)
		{
			VertexElement vertexElement;
			memcpy(&vertexElement, fileData, sizeof(VertexElement));
			fileData += sizeof(VertexElement);

#if defined(__PS3__)
			vertexElement = EndianUtils::SwapEndian(vertexElement);
#endif

			vertexDescriptorList.AddVertexElement(vertexElement);
		}

		uint32_t vertice_count = 0;
		uint32_t index_count = 0;
		uint32_t vertexMemSize = 0;
		uint32_t indexMemSize = 0;

		// Use memcpy to avoid alignment issues
		memcpy(&vertice_count, fileData, sizeof(uint32_t));
		fileData += sizeof(uint32_t);
		memcpy(&index_count, fileData, sizeof(uint32_t));
		fileData += sizeof(uint32_t);
		memcpy(&vertexMemSize, fileData, sizeof(uint32_t));
		fileData += sizeof(uint32_t);
		memcpy(&indexMemSize, fileData, sizeof(uint32_t));
		fileData += sizeof(uint32_t);

#if defined(__PS3__)
		vertice_count = EndianUtils::SwapEndian(vertice_count);
		index_count = EndianUtils::SwapEndian(index_count);
		vertexMemSize = EndianUtils::SwapEndian(vertexMemSize);
		indexMemSize = EndianUtils::SwapEndian(indexMemSize);
#endif // defined(__PS3__)

		mesh.CreateSubMesh(vertice_count, index_count, vertexDescriptorList);
		const std::unique_ptr<MeshData::SubMesh>& subMesh = mesh.m_subMeshes[mesh.m_subMeshCount - 1];

		// Copy vertices data
		memcpy(subMesh->m_data, fileData, vertexMemSize);
		fileData += vertexMemSize;

#if defined(__PS3__)
		for (int vertexIndex = 0; vertexIndex < vertice_count; vertexIndex++)
		{
			for (auto& element : vertexDescriptorList.GetVertexElementList())
			{
				float* vertex = (float*)((char*)subMesh->m_data + element.offset + vertexIndex * vertexDescriptorList.GetVertexSize());
				if (element.vertexElement == VertexElement::POSITION_32_BITS)
				{
					vertex[0] = EndianUtils::SwapEndian(vertex[0]);
					vertex[1] = EndianUtils::SwapEndian(vertex[1]);
					vertex[2] = EndianUtils::SwapEndian(vertex[2]);
				}
				else if (element.vertexElement == VertexElement::NORMAL_32_BITS)
				{
					vertex[0] = EndianUtils::SwapEndian(vertex[0]);
					vertex[1] = EndianUtils::SwapEndian(vertex[1]);
					vertex[2] = EndianUtils::SwapEndian(vertex[2]);
				}
				else if (element.vertexElement == VertexElement::UV_32_BITS)
				{
					vertex[0] = EndianUtils::SwapEndian(vertex[0]);
					vertex[1] = EndianUtils::SwapEndian(vertex[1]);
				}
				else if (element.vertexElement == VertexElement::COLOR_4_FLOATS)
				{
					vertex[0] = EndianUtils::SwapEndian(vertex[0]);
					vertex[1] = EndianUtils::SwapEndian(vertex[1]);
					vertex[2] = EndianUtils::SwapEndian(vertex[2]);
					vertex[3] = EndianUtils::SwapEndian(vertex[3]);
				}
			}
		}
#endif

		// Copy indices data
		if(index_count != 0)
		{
			memcpy(subMesh->GetIndices(), fileData, indexMemSize);
			
#if defined(__PS3__)
			for (int indexIndex = 0; indexIndex < index_count; indexIndex++)
			{
				if (subMesh->usesShortIndices)
				{
					unsigned short& index = ((unsigned short*)subMesh->GetIndices())[indexIndex];
					index = EndianUtils::SwapEndian(index);
				}
				else 
				{
					unsigned int& index = ((unsigned int*)subMesh->GetIndices())[indexIndex];
					index = EndianUtils::SwapEndian(index);
				}
			}
#endif // defined(__PS3__)
		}
		fileData += indexMemSize;
	}

#if defined(__PSP__)
	sceKernelDcacheWritebackInvalidateAll(); // Very important
#endif // defined(__PSP__)

	delete[] fileDataOriginalPtr;

	return true;
}
