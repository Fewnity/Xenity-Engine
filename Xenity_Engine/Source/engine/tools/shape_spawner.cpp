#include "shape_spawner.h"
#include "../../xenity.h"

using namespace std;

Vector3 ShapeSpawner::defaultPosition = Vector3(0, 0, 0);
Vector3 ShapeSpawner::defaultRotation = Vector3(0, 0, 0);
Vector3 ShapeSpawner::defaultScale = Vector3(1, 1, 1);

std::shared_ptr <GameObject>ShapeSpawner::SpawnCube()
{
	return MakeMesh("Cube", "models/Basic/CubeTriangulate.obj");
}

std::shared_ptr <GameObject>ShapeSpawner::SpawnSphere()
{
	return MakeMesh("Sphere", "models/Basic/SphereTriangulateSmooth.obj");
}

std::shared_ptr <GameObject>ShapeSpawner::SpawnCone()
{
	return MakeMesh("Cone", "models/Basic/ConeTriangulateSmooth.obj");
}

std::shared_ptr <GameObject >ShapeSpawner::SpawnDonut()
{
	return MakeMesh("Donut", "models/Basic/DonutTriangulateSmooth.obj");
}

std::shared_ptr <GameObject>ShapeSpawner::SpawnPlane()
{
	return MakeMesh("Plane", "models/Basic/PlaneTriangulate.obj");
}

std::shared_ptr <GameObject>ShapeSpawner::SpawnCylinder()
{
	return MakeMesh("Cylinder", "models/Basic/CylinderTriangulateSmooth.obj");
}

std::shared_ptr<GameObject> ShapeSpawner::MakeMesh(const std::string& gameObjectName, const std::string& meshFilePath)
{
	std::shared_ptr<GameObject> gameObject = CreateGameObject(gameObjectName);
	std::shared_ptr<MeshRenderer> mesh = gameObject->AddComponent<MeshRenderer>();
	mesh->meshData = MeshManager::LoadMesh(meshFilePath);
	mesh->textures.push_back(AssetManager::defaultTexture);
	SetDefaultValues(gameObject);
	// mesh->material = AssetManager::default3DMaterial;
	return gameObject;
}

void ShapeSpawner::SetDefaultValues(const std::shared_ptr <GameObject>& gameObject)
{
	auto transform = gameObject->GetTransform();
	transform->SetPosition(defaultPosition);
	transform->SetRotation(defaultRotation);
	transform->SetLocalScale(defaultScale);
}