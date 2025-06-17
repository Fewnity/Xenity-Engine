# SubMesh

Add this in your code:
```cpp
#include <engine/graphics/3d_graphics/mesh_data.h>
```

## Description

Class that store vertices and indices of a sub mesh.

## Public methods

---
### SetVertex (UV, Color, Position)
Set the data of a vertex.

Parameters:
- `u`: U texture coordinate
- `v`: v texture coordinate
- `color`: color
- `x`: X position
- `y`: Y position
- `z`: Z position
- `vertexIndex`: Index of the vertex to modify
```cpp
void SetVertex(float u, float v,
               const Color& color,
               float x, float y, float z,
               uint32_t vertexIndex)
```
Code sample:
```cpp
std::shared_ptr<MeshData> meshData; // Filled variable
const std::unique_ptr<SubMesh>& subMesh0 = meshData->GetSubMesh(0);
subMesh0->SetVertex(1.0f, 0.0f, Color::CreateFromRGBA(255, 128, 128, 255), 1.0f, 2.0f, 1.0f, 0);
```

---
### SetVertex (Position)
Set the data of a vertex.

Parameters:
- `x`: X position
- `y`: Y position
- `z`: Z position
- `vertexIndex`: Index of the vertex to modify
```cpp
void SetVertex(float x, float y, float z,
               uint32_t vertexIndex)
```
Code sample:
```cpp
std::shared_ptr<MeshData> meshData; // Filled variable
const std::unique_ptr<SubMesh>& subMesh0 = meshData->GetSubMesh(0);
subMesh0->SetVertex(1.0f, 2.0f, 1.0f, 0);
```

---
### SetVertex (UV, Position)
Set the data of a vertex.

Parameters:
- `u`: U texture coordinate
- `v`: v texture coordinate
- `x`: X position
- `y`: Y position
- `z`: Z position
- `vertexIndex`: Index of the vertex to modify
```cpp
void SetVertex(float u, float v,
               float x, float y, float z,
               uint32_t vertexIndex)
```
Code sample:
```cpp
std::shared_ptr<MeshData> meshData; // Filled variable
const std::unique_ptr<SubMesh>& subMesh0 = meshData->GetSubMesh(0);
subMesh0->SetVertex(1.0f, 0.0f, 1.0f, 2.0f, 1.0f, 0);
```

---
### SetVertex (UV, Normal, Position)
Set the data of a vertex.

Parameters:
- `u`: U texture coordinate
- `v`: v texture coordinate
- `nx`: X normal
- `ny`: Y normal
- `nz`: Z normal
- `x`: X position
- `y`: Y position
- `z`: Z position
- `vertexIndex`: Index of the vertex to modify
```cpp
void SetVertex(float u, float v,
               float nx, float ny, float nz,
               float x, float y, float z,
               uint32_t vertexIndex)
```
Code sample:
```cpp
std::shared_ptr<MeshData> meshData; // Filled variable
const std::unique_ptr<SubMesh>& subMesh0 = meshData->GetSubMesh(0);
subMesh0->SetVertex(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 2.0f, 1.0f, 0);
```

---
### SetVertex (Normal, Position)
Set the data of a vertex.

Parameters:
- `nx`: X normal
- `ny`: Y normal
- `nz`: Z normal
- `x`: X position
- `y`: Y position
- `z`: Z position
- `vertexIndex`: Index of the vertex to modify
```cpp
void SetVertex(float nx, float ny, float nz, 
               float x, float y, float z,
               uint32_t vertexIndex)
```
Code sample:
```cpp
std::shared_ptr<MeshData> meshData; // Filled variable
const std::unique_ptr<SubMesh>& subMesh0 = meshData->GetSubMesh(0);
subMesh0->SetVertex(0.0f, 0.0f, 1.0f, 1.0f, 2.0f, 1.0f, 0);
```

---
### SetPosition
Set the position of a vertex.

Parameters:
- `x`: X position
- `y`: Y position
- `z`: Z position
- `vertexIndex`: Index of the vertex to modify
```cpp
void SetPosition(float x, float y, float z, uint32_t vertexIndex)
```
Code sample:
```cpp
std::shared_ptr<MeshData> meshData; // Filled variable
const std::unique_ptr<SubMesh>& subMesh0 = meshData->GetSubMesh(0);
subMesh0->SetPosition(1.0f, 2.0f, 1.0f, 0);
```

---
### SetNormal
Set the normal of a vertex.

Parameters:
- `nx`: X normal
- `ny`: Y normal
- `nz`: Z normal
- `vertexIndex`: Index of the vertex to modify
```cpp
void SetNormal(float nx, float ny, float nz, uint32_t vertexIndex)
```
Code sample:
```cpp
std::shared_ptr<MeshData> meshData; // Filled variable
const std::unique_ptr<SubMesh>& subMesh0 = meshData->GetSubMesh(0);
subMesh0->SetNormal(0.0f, 0.0f, 1.0f, 0);
```

---
### SetUV
Set the UV coordinates of a vertex.

Parameters:
- `u`: U texture coordinate
- `v`: v texture coordinate
- `vertexIndex`: Index of the vertex to modify
```cpp
void SetUV(float u, float v, uint32_t vertexIndex)
```
Code sample:
```cpp
std::shared_ptr<MeshData> meshData; // Filled variable
const std::unique_ptr<SubMesh>& subMesh0 = meshData->GetSubMesh(0);
subMesh0->SetUV(1.0f, 0.0f, 0);
```

---
### SetColor
Set the color of a vertex.

Parameters:
- `color`: color
- `vertexIndex`: Index of the vertex to modify
```cpp
void SetColor(const Color& color, uint32_t vertexIndex)
```
Code sample:
```cpp
std::shared_ptr<MeshData> meshData; // Filled variable
const std::unique_ptr<SubMesh>& subMesh0 = meshData->GetSubMesh(0);
subMesh0->SetColor(Color::CreateFromRGBA(255, 128, 128, 255), 0);
```

---
### SetIndex
Set index value.

Parameters:
- `index`: The new vertex index
- `value`: Index to change
```cpp
void SetIndex(size_t index, uint32_t value)
```
Code sample:
```cpp
std::shared_ptr<MeshData> meshData; // Filled variable
const std::unique_ptr<SubMesh>& subMesh0 = meshData->GetSubMesh(0);
// Make a triangle with the 5,3,4 indices
subMesh0->SetIndex(5, 0);
subMesh0->SetIndex(3, 1);
subMesh0->SetIndex(4, 2);
```

---
### GetIndices
Get indices data pointer. (unsigned short or unsigned int).
```cpp
void* GetIndices() const
```
Code sample:
```cpp
std::shared_ptr<MeshData> meshData; // Filled variable
const std::unique_ptr<SubMesh>& subMesh0 = meshData->GetSubMesh(0);

if (subMesh->usesShortIndices)
{
    unsigned short& index = ((unsigned short*)subMesh->GetIndices())[0];
}
else
{
    unsigned int& index = ((unsigned int*)subMesh->GetIndices())[0];
}
```