# RaycastHit

Add this in your code:
```cpp
#include <engine/physics/raycast.h>
```

## Description

This class stores information of a racyast hit.

## Variables

---
| Name | Type | Description |
|-|-|-|
hitGameObject | `std::weak_ptr<GameObject>` | Pointer of the GameObject hit by the ray
hitCollider | `std::weak_ptr<Collider>` | Pointer of the Collider hit by the ray
hitPosition | `Vector3` | World position of the hit
distance | `float` | Distance between the start position and the hit position