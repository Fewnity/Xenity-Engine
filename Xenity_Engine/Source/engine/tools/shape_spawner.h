#pragma once

#include <string>

class GameObject;
class MeshRenderer;
class Vector3;

class ShapeSpawner
{
public:
	static GameObject* SpawnCube();
	static GameObject* SpawnSphere();
	static GameObject* SpawnCone();
	static GameObject* SpawnDonut();
	static GameObject* SpawnPlane();
	static GameObject* SpawnCylinder();

	static Vector3 defaultPosition;
	static Vector3 defaultRotation;
	static Vector3 defaultScale;

private:
	static GameObject* Spawn(GameObject* gameObject);
};

