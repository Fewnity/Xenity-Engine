# MeshData

Add this in your code:
```cpp
#include <engine/graphics/3d_graphics/mesh_data.h>
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

---
### CreateSubMesh 
Create a sub mesh and allocate memory for it.

Parameters:
- `vertexCount`: The number of vertices allocated
- `indexCount`: The number of indices allocated
- `vertexDescriptorList`: The vertex data structure
```cpp
void CreateSubMesh(uint32_t verticeCount, uint32_t index_count, const VertexDescriptor& vertexDescriptorList)
```
Code sample:
```cpp
std::shared_ptr<MeshData> newMesh = MeshData::CreateMeshData();

VertexDescriptor descriptor;
descriptor.AddVertexElement(VertexElements::UV_32_BITS);
descriptor.AddVertexElement(VertexElements::POSITION_32_BITS);

newMesh->CreateSubMesh(6,0, descriptor);
```

---
### GetSubMesh 
Get a submesh by index.

Parameters:
- `index`: Index of the submesh
```cpp
const std::unique_ptr<SubMesh>& GetSubMesh(size_t index)
```
Code sample:
```cpp
std::shared_ptr<MeshData> meshData; // Filled variable
const std::unique_ptr<SubMesh>& subMesh0 = meshData->GetSubMesh(0);
const std::unique_ptr<SubMesh>& subMesh2 = meshData->GetSubMesh(2);
```

---
### Update 
Update the mesh after a modification on the submeshes.
```cpp
void Update()
```
Code sample:
```cpp

```