#include "shape_spawner.h"

#include "../gameobject.h"
#include "../graphics/mesh.h"

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
    gameObject->AddExistingComponent(mesh);
    return gameObject;
}