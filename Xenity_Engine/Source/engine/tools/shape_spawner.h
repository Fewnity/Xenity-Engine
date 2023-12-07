#pragma once
#include <engine/api.h>

#include <string>
#include <memory>

class GameObject;
class Vector3;

class API ShapeSpawner
{
public:

	/**
	* Spawn a cube
	*/
	static std::shared_ptr <GameObject> SpawnCube();

	/**
	* Spawn a sphere
	*/
	static std::shared_ptr <GameObject> SpawnSphere();

	/**
	* Spawn a cone
	*/
	static std::shared_ptr <GameObject> SpawnCone();

	/**
	* Spawn a donut
	*/
	static std::shared_ptr <GameObject> SpawnDonut();

	/**
	* Spawn a plane
	*/
	static std::shared_ptr <GameObject> SpawnPlane();

	/**
	* Spawn a cylinder
	*/
	static std::shared_ptr <GameObject> SpawnCylinder();


private:
	/**
	* Create a GameObject and add a MeshRenderer to it
	*/
	static std::shared_ptr<GameObject> MakeMesh(const std::string& gameObjectName, const std::string& meshFilePath);

	static Vector3 defaultPosition;
	static Vector3 defaultRotation;
	static Vector3 defaultScale;
	static void SetDefaultValues(const std::shared_ptr <GameObject>& gameObject);
};
