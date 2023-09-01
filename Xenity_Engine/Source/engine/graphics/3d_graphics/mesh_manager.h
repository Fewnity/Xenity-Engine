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

class MeshData;
class Vector3;
class Texture;

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
	* @param useDepth Is depth for drawing
	* @param useBlend Is depth for drawing
	* @param useLighting Is depth for drawing
	*/
	static void DrawMesh(Vector3 position, Vector3 rotation, Vector3 scale, std::vector< std::shared_ptr<Texture>>textures, std::shared_ptr < MeshData> meshData, bool useDepth, bool useBlend, bool useLighting);
	
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
	static void DrawMesh(Vector3 position, Vector3 rotation, Vector3 scale, std::shared_ptr<Texture> texture, std::shared_ptr<MeshData> meshData, bool useDepth, bool useBlend, bool useLighting);
	
	/**
	* Load a mesh from a file path
	* @param path File path
	*/
	static std::shared_ptr <MeshData> LoadMesh(std::string path);

private:
};