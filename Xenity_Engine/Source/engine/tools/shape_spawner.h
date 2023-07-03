#pragma once

#include <string>
#include <memory>

class GameObject;
class Vector3;

class ShapeSpawner
{
public:
	static std::weak_ptr <GameObject> SpawnCube();
	static std::weak_ptr <GameObject> SpawnSphere();
	static std::weak_ptr <GameObject> SpawnCone();
	static std::weak_ptr <GameObject> SpawnDonut();
	static std::weak_ptr <GameObject> SpawnPlane();
	static std::weak_ptr <GameObject> SpawnCylinder();

	static Vector3 defaultPosition;
	static Vector3 defaultRotation;
	static Vector3 defaultScale;

private:
	static std::weak_ptr <GameObject> Spawn(std::weak_ptr <GameObject> gameObject);
};
