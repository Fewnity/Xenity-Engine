// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

/**
 * [Internal]
 */
#include <string>
#include <memory>

#include <engine/api.h>
#include <engine/graphics/3d_graphics/mesh_data.h>

class Vector3;
class Texture;
class Transform;
struct RenderingSettings;
class Material;
class Quaternion;

class API MeshManager
{
public:

	/**
	* @brief Init mesh manager
	*/
	static void Init();
	
	/**
	* @brief Draw a submesh
	* @param transform Mesh transform
	* @param subMesh Submesh to draw
	* @param material Material to use
	* @param renderSettings Rendering settings
	*/
	static void DrawMesh(const Transform& transform, const MeshData::SubMesh& subMesh, Material& material, RenderingSettings& renderSettings);
	
	/**
	* @brief Draw a submesh
	* @param position Mesh position
	* @param rotation Mesh rotation
	* @param scale Mesh scale
	* @param subMesh Submesh to draw
	* @param material Material to use
	* @param renderSettings Rendering settings
	*/
	static void DrawMesh(const Vector3& position, const Quaternion& rotation, const Vector3& scale, const MeshData::SubMesh& subMesh, Material& material, RenderingSettings& renderSettings);

	
	/**
	* @brief Load a mesh from a file path
	* @param path File path
	* @return The loaded mesh
	*/
	static std::shared_ptr <MeshData> LoadMesh(const std::string& path);

private:
};