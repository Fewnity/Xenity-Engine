#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

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
	static std::weak_ptr <GameObject> SpawnCube();

	/**
	* Spawn a sphere
	*/
	static std::weak_ptr <GameObject> SpawnSphere();

	/**
	* Spawn a cone
	*/
	static std::weak_ptr <GameObject> SpawnCone();

	/**
	* Spawn a donut
	*/
	static std::weak_ptr <GameObject> SpawnDonut();

	/**
	* Spawn a plane
	*/
	static std::weak_ptr <GameObject> SpawnPlane();

	/**
	* Spawn a cylinder
	*/
	static std::weak_ptr <GameObject> SpawnCylinder();


private:
	static Vector3 defaultPosition;
	static Vector3 defaultRotation;
	static Vector3 defaultScale;
	static std::weak_ptr <GameObject> Spawn(std::weak_ptr <GameObject> gameObject);
};
