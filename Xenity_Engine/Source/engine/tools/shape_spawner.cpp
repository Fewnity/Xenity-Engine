#include "shape_spawner.h"

#include <engine/vectors/vector3.h>

#include <engine/game_elements/gameobject.h>

#include <engine/asset_management/asset_manager.h>
#include <engine/graphics/3d_graphics/mesh_manager.h>
#include <engine/graphics/3d_graphics/mesh_renderer.h>
#include <engine/game_elements/transform.h>
#include <engine/asset_management/project_manager.h>

using namespace std;

Vector3 ShapeSpawner::defaultPosition = Vector3(0, 0, 0);
Vector3 ShapeSpawner::defaultRotation = Vector3(0, 0, 0);
Vector3 ShapeSpawner::defaultScale = Vector3(1, 1, 1);

std::shared_ptr <GameObject>ShapeSpawner::SpawnCube()
{
	return MakeMesh("Cube", "public_engine_assets/models/CubeTriangulate.obj");
}

std::shared_ptr <GameObject>ShapeSpawner::SpawnSphere()
{
	return MakeMesh("Sphere", "public_engine_assets/models/SphereTriangulate.obj");
}

std::shared_ptr <GameObject>ShapeSpawner::SpawnCone()
{
	return MakeMesh("Cone", "public_engine_assets/models/ConeTriangulate.obj");
}

std::shared_ptr <GameObject >ShapeSpawner::SpawnDonut()
{
	return MakeMesh("Donut", "public_engine_assets/models/DonutTriangulate.obj");
}

std::shared_ptr <GameObject>ShapeSpawner::SpawnPlane()
{
	return MakeMesh("Plane", "public_engine_assets/models/PlaneTriangulate.obj");
}

std::shared_ptr <GameObject>ShapeSpawner::SpawnCylinder()
{
	return MakeMesh("Cylinder", "public_engine_assets/models/CylinderTriangulate.obj");
}

std::shared_ptr<GameObject> ShapeSpawner::MakeMesh(const std::string& gameObjectName, const std::string& meshFilePath)
{
	DXASSERT(!meshFilePath.empty(), "[ShapeSpawner::MakeMesh] meshFilePath is empty")

	std::shared_ptr<GameObject> gameObject = CreateGameObject(gameObjectName);
	std::shared_ptr<MeshRenderer> mesh = gameObject->AddComponent<MeshRenderer>();

	std::shared_ptr<FileReference> fileRef = ProjectManager::GetFileReferenceByFilePath(meshFilePath);
	fileRef->LoadFileReference();

	mesh->SetMeshData(std::dynamic_pointer_cast<MeshData>(fileRef));
	mesh->SetMaterial(AssetManager::standardMaterial, 0);
	SetDefaultValues(gameObject);
	return gameObject;
}

void ShapeSpawner::SetDefaultValues(const std::shared_ptr <GameObject>& gameObject)
{
	DXASSERT(gameObject != nullptr, "[ShapeSpawner::SetDefaultValues] gameObject is empty")

	std::shared_ptr<Transform> transform = gameObject->GetTransform();
	transform->SetPosition(defaultPosition);
	transform->SetRotation(defaultRotation);
	transform->SetLocalScale(defaultScale);
}