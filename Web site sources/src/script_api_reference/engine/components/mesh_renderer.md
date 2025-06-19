# MeshRenderer

Add this in your code:
```cpp
#include <engine/graphics/3d_graphics/mesh_renderer.h>
```

## Description

Component that renders a mesh with materials

## Public methods

---
### GetMeshData
Get mesh data.
```cpp
const std::shared_ptr<MeshData>& GetMeshData() const
```

---
### SetMeshData
Set mesh data.

Parameters:
- `meshData`: Mesh data to use
```cpp
void SetMeshData(const std::shared_ptr <MeshData>& meshData)
```

---
### GetMaterials
Get materials list.
```cpp
std::vector<std::shared_ptr<Material>> GetMaterials() const
```

---
### GetMaterial
Get a material at a specific index.

Parameters:
- `index`: Index of the material
```cpp
std::shared_ptr<Material> GetMaterial(int index) const
```

---
### SetMeshData
Set a material at a specific index.

Parameters:
- `material`: Material to use
- `index`: Index of the material
```cpp
void SetMaterial(const std::shared_ptr <Material>& material, int index)
```

---
### GetBoundingSphere
Get the bounding sphere of the mesh.
```cpp
const Sphere& GetBoundingSphere() const
```

---
### GetUseAdvancedLighting
Get if the mesh can use advanced lighting (point lights, spot lights), improve performance if disabled.
```cpp
bool GetUseAdvancedLighting() const
```

---
### SetUseAdvancedLighting
Set if the mesh can use advanced lighting (point lights, spot lights), improve performance if disabled.

Parameters:
- `value`: Enable advanced lighting?
```cpp
void SetUseAdvancedLighting(bool value)
```