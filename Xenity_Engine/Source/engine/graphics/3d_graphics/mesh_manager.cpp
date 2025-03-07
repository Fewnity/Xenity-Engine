// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "mesh_manager.h"

#include <engine/graphics/graphics.h>
#include <engine/file_system/file_system.h>
#include <engine/game_elements/transform.h>
#include <engine/debug/debug.h>
#include <engine/tools/math.h>
#include <engine/vectors/quaternion.h>
#include <engine/debug/stack_debug_object.h>

#include "mesh_data.h"

void MeshManager::Init()
{
	Debug::Print("-------- Mesh Manager initiated --------", true);
}

std::shared_ptr <MeshData> MeshManager::LoadMesh(const std::string& path)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	XASSERT(!path.empty(), "[MeshManager::LoadMesh] path is empty");

	std::shared_ptr <MeshData> mesh = MeshData::MakeMeshDataForFile();
	mesh->m_file = FileSystem::MakeFile(path);
	mesh->m_fileType = FileType::File_Mesh;

	FileReference::LoadOptions loadOptions;
	loadOptions.platform = Application::GetPlatform();
	loadOptions.threaded = false;
	mesh->LoadFileReference(loadOptions);
	XASSERT(mesh->GetFileStatus() == FileStatus::FileStatus_Loaded, "[MeshManager::LoadMesh] Mesh file not loaded");
	return mesh;
}

//void MeshManager::DrawMesh(const Vector3& position, const Quaternion& rotation, const Vector3& scale, const MeshData::SubMesh& subMesh, Material& material, RenderingSettings& renderSettings)
//{
//	const glm::mat4 matrix = Math::CreateModelMatrix(position, rotation, scale);
//	Graphics::DrawSubMesh(subMesh, material, renderSettings, matrix, false);
//}

void MeshManager::DrawMesh(Transform& transform, const MeshData::SubMesh& subMesh, Material& material, RenderingSettings& renderSettings)
{
	const Vector3& scale = transform.GetScale();

	if (scale.x * scale.y * scale.z < 0)
	{
		renderSettings.invertFaces = !renderSettings.invertFaces;
	}

	Graphics::DrawSubMesh(subMesh, material, renderSettings, transform.GetTransformationMatrix(), transform.GetInverseNormalMatrix(), transform.GetMVPMatrix(Graphics::s_currentFrame), false);
}