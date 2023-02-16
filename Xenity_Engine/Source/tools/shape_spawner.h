#pragma once

#include <string>

class GameObject;
class Mesh;

class ShapeSpawner
{
public:
	static GameObject* SpawnCube();
	static GameObject* SpawnSphere();
	static GameObject* SpawnCone();
	static GameObject* SpawnDonut();
	static GameObject* SpawnPlane();
	static GameObject* SpawnCylinder();

private:
	static GameObject* Spawn(Mesh* mesh, std::string name);
};

