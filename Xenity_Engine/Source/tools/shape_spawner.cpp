#include "shape_spawner.h"

#include "../gameobject.h"
#include "../graphics/mesh.h"
#include "../vectors/vector3.h"

Vector3 ShapeSpawner::defaultPosition = Vector3(0,0,0);
Vector3 ShapeSpawner::defaultRotation = Vector3(0, 0, 0);
Vector3 ShapeSpawner::defaultScale = Vector3(1, 1, 1);

GameObject* ShapeSpawner::SpawnCube()
{
    Mesh * mesh = new Mesh("Basic/CubeTriangulate.obj");
    mesh->material = AssetManager::default3DMaterial;
    return Spawn(mesh, "Cube");
}

GameObject* ShapeSpawner::SpawnSphere()
{
    Mesh* mesh = new Mesh("Basic/SphereTriangulateSmooth.obj");
    mesh->material = AssetManager::default3DMaterial;
    return Spawn(mesh, "Sphere");
}

GameObject* ShapeSpawner::SpawnCone()
{
    Mesh* mesh = new Mesh("Basic/ConeTriangulateSmooth.obj");
    mesh->material = AssetManager::default3DMaterial;
    return Spawn(mesh, "Cone");
}

GameObject* ShapeSpawner::SpawnDonut()
{
    Mesh* mesh = new Mesh("Basic/DonutTriangulateSmooth.obj");
    mesh->material = AssetManager::default3DMaterial;
    return Spawn(mesh, "Donut");
}

GameObject* ShapeSpawner::SpawnPlane()
{
    Mesh* mesh = new Mesh("Basic/PlaneTriangulate.obj");
    mesh->material = AssetManager::default3DMaterial;
    return Spawn(mesh, "Plane");
}

GameObject* ShapeSpawner::SpawnCylinder()
{
    Mesh* mesh = new Mesh("Basic/CylinderTriangulateSmooth.obj");
    mesh->material = AssetManager::default3DMaterial;
    return Spawn(mesh, "Cylinder");
}

GameObject* ShapeSpawner::Spawn(Mesh * mesh, std::string name)
{
    GameObject* gameObject = new GameObject(name);
    gameObject->transform.SetPosition(defaultPosition);
    gameObject->transform.SetRotation(defaultRotation);
    gameObject->transform.SetLocalScale(defaultScale);
    gameObject->AddExistingComponent(mesh);
    return gameObject;
}