#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>

class MeshData;
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
	static void DrawMesh(const Vector3& position, const Vector3& rotation, const Vector3& scale, const std::vector< std::shared_ptr<Texture>>& textures, std::shared_ptr < MeshData> meshData, RenderingSettings& renderSettings, std::shared_ptr <Material> material);
	static void DrawMesh(const std::shared_ptr<Transform>& transform, const std::vector< std::shared_ptr<Texture>>& textures, const std::shared_ptr <MeshData>& meshData, RenderingSettings& renderSettings, const std::shared_ptr<Material>& material);

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
	static void DrawMesh(const Vector3& position, const Vector3& rotation, const Vector3& scale, const std::shared_ptr<Texture>& texture, const std::shared_ptr<MeshData>& meshData, RenderingSettings& renderSettings, const std::shared_ptr<Material>& material);

	/**
	* Load a mesh from a file path
	* @param path File path
	*/
	static std::shared_ptr <MeshData> LoadMesh(const std::string& path);

private:
};