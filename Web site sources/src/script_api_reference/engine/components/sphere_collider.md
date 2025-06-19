# SphereCollider

Add this in your code:
```cpp
#include <engine/physics/sphere_collider.h>
```

## Description

Component to add a sphere-shaped collider to a GameObject.

## Public Methods

---
### GetSize
Get the radius of the sphere collider.
```cpp
float GetSize() const
```

---
### SetSize
Set the radius of the sphere collider.

Parameters:
- `size`: Radius of the sphere
```cpp
void SetSize(float size)
```

---
### GetOffset
Get the offset position of the sphere collider.
```cpp
const Vector3& GetOffset() const
```

---
### SetOffset
Set the offset position of the sphere collider.

Parameters:
- `offset`: Offset position of the sphere
```cpp
void SetOffset(const Vector3& offset)
```