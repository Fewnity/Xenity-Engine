#include "shape_spawner.h"
#include "../../xenity.h"

Vector3 ShapeSpawner::defaultPosition = Vector3(0,0,0);
Vector3 ShapeSpawner::defaultRotation = Vector3(0, 0, 0);
Vector3 ShapeSpawner::defaultScale = Vector3(1, 1, 1);

GameObject* ShapeSpawner::SpawnCube()
{
    MeshRenderer * mesh = new MeshRenderer("Basic/CubeTriangulate.obj");
    mesh->material = AssetManager::default3DMaterial;
    return Spawn(mesh, "Cube");
}

GameObject* ShapeSpawner::SpawnSphere()
{
    MeshRenderer* mesh = new MeshRenderer("Basic/SphereTriangulateSmooth.obj");
    mesh->material = AssetManager::default3DMaterial;
    return Spawn(mesh, "Sphere");
}

GameObject* ShapeSpawner::SpawnCone()
{
    MeshRenderer* mesh = new MeshRenderer("Basic/ConeTriangulateSmooth.obj");
    mesh->material = AssetManager::default3DMaterial;
    return Spawn(mesh, "Cone");
}

GameObject* ShapeSpawner::SpawnDonut()
{
    MeshRenderer* mesh = new MeshRenderer("Basic/DonutTriangulateSmooth.obj");
    mesh->material = AssetManager::default3DMaterial;
    return Spawn(mesh, "Donut");
}

GameObject* ShapeSpawner::SpawnPlane()
{
    MeshRenderer* mesh = new MeshRenderer("Basic/PlaneTriangulate.obj");
    mesh->material = AssetManager::default3DMaterial;
    return Spawn(mesh, "Plane");
}

GameObject* ShapeSpawner::SpawnCylinder()
{
    MeshRenderer* mesh = new MeshRenderer("Basic/CylinderTriangulateSmooth.obj");
    mesh->material = AssetManager::default3DMaterial;
    return Spawn(mesh, "Cylinder");
}

GameObject* ShapeSpawner::Spawn(MeshRenderer * mesh, std::string name)
{
    GameObject* gameObject = new GameObject(name);
    gameObject->transform.SetPosition(defaultPosition);
    gameObject->transform.SetRotation(defaultRotation);
    gameObject->transform.SetLocalScale(defaultScale);
    gameObject->AddExistingComponent(mesh);
    return gameObject;
}