#pragma once

/**
 * [Internal]
 */

#include <engine/api.h>

#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>

#include <engine/graphics/3d_graphics/mesh_data.h>

class Vector3;
class Texture;
class Transform;
class RenderingSettings;
class Material;

class API MeshManager
{
public:

	/**
	* Init mesh manager
	*/
	static void Init();

	/**
	* Draw a mesh with multiple textures for submeshes
	* @param position Mesh position
	* @param rotation Mesh rotation
	* @param scale Mesh scale
	* @param textures Submeshes textures
	* @param meshData Mesh data
	* @param useDepth Use depth for drawing
	* @param useBlend Use blend for drawing
	* @param useLighting Use lighting for drawing
	*/
	
	static void DrawMesh(const std::shared_ptr<Transform>& transform, const MeshData::SubMesh& subMesh, const std::shared_ptr<Material>& material, RenderingSettings& renderSettings);
	static void DrawMesh(const Vector3& position, const Vector3& rotation, const Vector3& scale, const MeshData::SubMesh& subMesh, const std::shared_ptr<Material>& material, RenderingSettings& renderSettings);


	/**
	* Draw a mesh with a signem submesh
	* @param position Mesh position
	* @param rotation Mesh rotation
	* @param scale Mesh scale
	* @param texture Mesh texture
	* @param meshData Mesh data (With only one submesh)
	* @param useDepth Is depth for drawing
	* @param useBlend Is depth for drawing
	* @param useLighting Is depth for drawing
	*/
	
	/**
	* Load a mesh from a file path
	* @param path File path
	*/
	static std::shared_ptr <MeshData> LoadMesh(const std::string& path);

private:
};