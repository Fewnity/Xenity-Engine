#include "shape_spawner.h"
#include "../../xenity.h"

Vector3 ShapeSpawner::defaultPosition = Vector3(0,0,0);
Vector3 ShapeSpawner::defaultRotation = Vector3(0, 0, 0);
Vector3 ShapeSpawner::defaultScale = Vector3(1, 1, 1);

GameObject* ShapeSpawner::SpawnCube()
{
    GameObject* gameObject = new GameObject("Cube");
    MeshRenderer* mesh = gameObject->AddComponent<MeshRenderer>();

    mesh->LoadFromFile("Basic/CubeTriangulate.obj");

    mesh->material = AssetManager::default3DMaterial;
    return Spawn(gameObject);
}

GameObject* ShapeSpawner::SpawnSphere()
{
    GameObject* gameObject = new GameObject("Sphere");
    MeshRenderer* mesh = gameObject->AddComponent<MeshRenderer>();
    mesh->LoadFromFile("Basic/SphereTriangulateSmooth.obj");

    mesh->material = AssetManager::default3DMaterial;
    return Spawn(gameObject);
}

GameObject* ShapeSpawner::SpawnCone()
{
    GameObject* gameObject = new GameObject("Cone");
    MeshRenderer* mesh = gameObject->AddComponent<MeshRenderer>();
    mesh->LoadFromFile("Basic/ConeTriangulateSmooth.obj");

    mesh->material = AssetManager::default3DMaterial;
    return Spawn(gameObject);
}

GameObject* ShapeSpawner::SpawnDonut()
{
    GameObject* gameObject = new GameObject("Donut");
    MeshRenderer* mesh = gameObject->AddComponent<MeshRenderer>();
    mesh->LoadFromFile("Basic/DonutTriangulateSmooth.obj");

    mesh->material = AssetManager::default3DMaterial;
    return Spawn(gameObject);
}

GameObject* ShapeSpawner::SpawnPlane()
{
    GameObject* gameObject = new GameObject("Plane");
    MeshRenderer* mesh = gameObject->AddComponent<MeshRenderer>();
    mesh->LoadFromFile("Basic/PlaneTriangulate.obj");

    mesh->material = AssetManager::default3DMaterial;
    return Spawn(gameObject);
}

GameObject* ShapeSpawner::SpawnCylinder()
{
    GameObject* gameObject = new GameObject("Cylinder");
    MeshRenderer* mesh = gameObject->AddComponent<MeshRenderer>();
    mesh->LoadFromFile("Basic/CylinderTriangulateSmooth.obj");

    mesh->material = AssetManager::default3DMaterial;
    return Spawn(gameObject);
}

GameObject* ShapeSpawner::Spawn(GameObject* gameObject)
{
    gameObject->transform.SetPosition(defaultPosition);
    gameObject->transform.SetRotation(defaultRotation);
    gameObject->transform.SetLocalScale(defaultScale);
    return gameObject;
}