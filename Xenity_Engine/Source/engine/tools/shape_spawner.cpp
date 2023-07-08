#include "shape_spawner.h"
#include "../../xenity.h"

Vector3 ShapeSpawner::defaultPosition = Vector3(0, 0, 0);
Vector3 ShapeSpawner::defaultRotation = Vector3(0, 0, 0);
Vector3 ShapeSpawner::defaultScale = Vector3(1, 1, 1);

using namespace std;

std::weak_ptr <GameObject>ShapeSpawner::SpawnCube()
{
	auto gameObject = CreateGameObject("Cube");
	auto mesh = gameObject->AddComponent<MeshRenderer>();
	mesh->meshData = MeshManager::LoadMesh("Basic/CubeTriangulate.obj");

	// mesh->material = AssetManager::default3DMaterial;
	return Spawn(gameObject);
}

std::weak_ptr <GameObject>ShapeSpawner::SpawnSphere()
{
	auto gameObject = CreateGameObject("Sphere");
	auto mesh = gameObject->AddComponent<MeshRenderer>();
	mesh->meshData = MeshManager::LoadMesh("Basic/SphereTriangulateSmooth.obj");

	// mesh->material = AssetManager::default3DMaterial;
	return Spawn(gameObject);
}

std::weak_ptr <GameObject>ShapeSpawner::SpawnCone()
{
	auto gameObject = CreateGameObject("Cone");
	auto mesh = gameObject->AddComponent<MeshRenderer>();
	mesh->meshData = MeshManager::LoadMesh("Basic/ConeTriangulateSmooth.obj");

	// mesh->material = AssetManager::default3DMaterial;
	return Spawn(gameObject);
}

std::weak_ptr <GameObject >ShapeSpawner::SpawnDonut()
{
	auto gameObject = CreateGameObject("Donut");
	auto mesh = gameObject->AddComponent<MeshRenderer>();
	mesh->meshData = MeshManager::LoadMesh("Basic/DonutTriangulateSmooth.obj");

	// mesh->material = AssetManager::default3DMaterial;
	return Spawn(gameObject);
}

std::weak_ptr <GameObject>ShapeSpawner::SpawnPlane()
{
	auto gameObject = CreateGameObject("Plane");
	auto mesh = gameObject->AddComponent<MeshRenderer>();
	mesh->meshData = MeshManager::LoadMesh("Basic/PlaneTriangulate.obj");

	// mesh->material = AssetManager::default3DMaterial;
	return Spawn(gameObject);
}

std::weak_ptr <GameObject>ShapeSpawner::SpawnCylinder()
{
	auto gameObject = CreateGameObject("Cylinder");
	auto mesh = gameObject->AddComponent<MeshRenderer>();
	mesh->meshData = MeshManager::LoadMesh("Basic/CylinderTriangulateSmooth.obj");

	// mesh->material = AssetManager::default3DMaterial;
	return Spawn(gameObject);
}

std::weak_ptr <GameObject>ShapeSpawner::Spawn(std::weak_ptr <GameObject> gameObject)
{
	auto transform = gameObject.lock()->GetTransform();
	transform->SetPosition(defaultPosition);
	transform->SetRotation(defaultRotation);
	transform->SetLocalScale(defaultScale);
	return gameObject;
}