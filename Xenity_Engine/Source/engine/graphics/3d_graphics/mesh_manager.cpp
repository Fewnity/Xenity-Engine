// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "mesh_manager.h"

#include "mesh_data.h"
#include <engine/graphics/graphics.h>
#include <engine/file_system/file_system.h>
#include <engine/game_elements/transform.h>
#include <engine/debug/debug.h>
#include <engine/tools/math.h>


void MeshManager::Init()
{
	Debug::Print("-------- Mesh Manager initiated --------", true);
}

std::shared_ptr <MeshData> MeshManager::LoadMesh(const std::string& path)
{
	XASSERT(!path.empty(), "[MeshManager::LoadMesh] path is empty")

	std::shared_ptr <MeshData> mesh = MeshData::MakeMeshData();
	mesh->file = FileSystem::MakeFile(path);
	mesh->fileType = FileType::File_Mesh;
	mesh->LoadFileReference();
	return mesh;
}

void MeshManager::DrawMesh(const Vector3& position, const Vector3& rotation, const Vector3& scale, const MeshData::SubMesh& subMesh, Material& material, RenderingSettings& renderSettings)
{
	const glm::mat4 matrix = Math::CreateModelMatrix(position, rotation, scale);
	Graphics::DrawSubMesh(subMesh, material, renderSettings, matrix, false);
}

void MeshManager::DrawMesh(const Transform& transform, const MeshData::SubMesh& subMesh, Material& material, RenderingSettings& renderSettings)
{
	const Vector3& scale = transform.GetScale();

	if (scale.x * scale.y * scale.z < 0)
		renderSettings.invertFaces = !renderSettings.invertFaces;

	Graphics::DrawSubMesh(subMesh, material, renderSettings, transform.GetTransformationMatrix(), false);
}