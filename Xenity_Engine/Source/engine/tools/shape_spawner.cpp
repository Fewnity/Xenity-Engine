#include "shape_spawner.h"
#include "../../xenity.h"

Vector3 ShapeSpawner::defaultPosition = Vector3(0, 0, 0);
Vector3 ShapeSpawner::defaultRotation = Vector3(0, 0, 0);
Vector3 ShapeSpawner::defaultScale = Vector3(1, 1, 1);

using namespace std;

GameObject *ShapeSpawner::SpawnCube()
{
    GameObject *gameObject = new GameObject("Cube");
    weak_ptr<MeshRenderer> mesh = gameObject->AddComponent<MeshRenderer>();
    mesh.lock()->meshData = MeshManager::LoadMesh("Basic/CubeTriangulate.obj");

    // mesh->material = AssetManager::default3DMaterial;
    return Spawn(gameObject);
}

GameObject *ShapeSpawner::SpawnSphere()
{
    GameObject *gameObject = new GameObject("Sphere");
    weak_ptr<MeshRenderer>mesh = gameObject->AddComponent<MeshRenderer>();
    mesh.lock()->meshData = MeshManager::LoadMesh("Basic/SphereTriangulateSmooth.obj");

    // mesh->material = AssetManager::default3DMaterial;
    return Spawn(gameObject);
}

GameObject *ShapeSpawner::SpawnCone()
{
    GameObject *gameObject = new GameObject("Cone");
    weak_ptr<MeshRenderer>mesh = gameObject->AddComponent<MeshRenderer>();
    mesh.lock()->meshData = MeshManager::LoadMesh("Basic/ConeTriangulateSmooth.obj");

    // mesh->material = AssetManager::default3DMaterial;
    return Spawn(gameObject);
}

GameObject *ShapeSpawner::SpawnDonut()
{
    GameObject *gameObject = new GameObject("Donut");
    weak_ptr<MeshRenderer>mesh = gameObject->AddComponent<MeshRenderer>();
    mesh.lock()->meshData = MeshManager::LoadMesh("Basic/DonutTriangulateSmooth.obj");

    // mesh->material = AssetManager::default3DMaterial;
    return Spawn(gameObject);
}

GameObject *ShapeSpawner::SpawnPlane()
{
    GameObject *gameObject = new GameObject("Plane");
    weak_ptr<MeshRenderer>mesh = gameObject->AddComponent<MeshRenderer>();
    mesh.lock()->meshData = MeshManager::LoadMesh("Basic/PlaneTriangulate.obj");

    // mesh->material = AssetManager::default3DMaterial;
    return Spawn(gameObject);
}

GameObject *ShapeSpawner::SpawnCylinder()
{
    GameObject *gameObject = new GameObject("Cylinder");
    weak_ptr<MeshRenderer>mesh = gameObject->AddComponent<MeshRenderer>();
    mesh.lock()->meshData = MeshManager::LoadMesh("Basic/CylinderTriangulateSmooth.obj");

    // mesh->material = AssetManager::default3DMaterial;
    return Spawn(gameObject);
}

GameObject *ShapeSpawner::Spawn(GameObject *gameObject)
{
    auto transform = gameObject->GetTransform().lock();
    transform->SetPosition(defaultPosition);
    transform->SetRotation(defaultRotation);
    transform->SetLocalScale(defaultScale);
    return gameObject;
}