# MeshData

Add this in your code:
```cpp
#include <engine/graphics/3d_graphics/skybox.h>
```

## Description

Asset that stores all the data of a mesh.

## Static methods

---
### CreateMeshData
Dynamically create a mesh.
```cpp
std::shared_ptr<MeshData> CreateMeshData()
```
Code sample:
```cpp
std::shared_ptr<MeshData> newMesh = MeshData::CreateMeshData();
```

## Public methods

---
### GetMinBoundingBox
Get the minimum position of the bounding box. (Box including all submeshes).
```cpp
const Vector3& GetMinBoundingBox() const
```
Code sample:
```cpp
std::shared_ptr<MeshData> meshData; // Filled variable
const Vector3& minPos = meshData->GetMinBoundingBox();
```

---
### GetMaxBoundingBox
Get the maximum position of the bounding box. (Box including all submeshes).
```cpp
const Vector3& GetMaxBoundingBox() const
```
Code sample:
```cpp
std::shared_ptr<MeshData> meshData; // Filled variable
const Vector3& maxPos = meshData->GetMaxBoundingBox();
```

---
### GetBoundingSphere
Get mesh bounding sphere. (Sphere including all submeshes).
```cpp
const Sphere& GetBoundingSphere() const
```
Code sample:
```cpp
std::shared_ptr<MeshData> meshData; // Filled variable
const Sphere& meshSphere = meshData->GetBoundingSphere();
```