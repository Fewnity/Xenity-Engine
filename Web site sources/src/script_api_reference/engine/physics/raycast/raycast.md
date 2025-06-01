# Raycast

Add this in your code:
```cpp
#include <engine/physics/raycast.h>
```

## Description

Class to check collisions with a ray.

## Public methods

---
### Check
Check if a ray hits a collider.<br>
Return true if the raycast has hit a collider.

Parameters:
- `startPosition`: The start position of the ray
- `direction`: The direction of the ray
- `maxDistance`: The max distance of the ray
- `raycastHit`: The raycastHit struct that will be filled with the hit information
```cpp
bool Check(const Vector3& startPosition, const Vector3& direction, const float maxDistance, RaycastHit& raycastHit)
```
Code sample:
```cpp
// Check under the player
RaycastHit hit;
bool hasHit = Raycast::Check(
    raycastPointTransform->GetPosition(), 
    Vector3(0, -1, 0), 
    0.1f, 
    hit);

if (hasHit && hit.hitCollider.lock())
{
    // There is something under the player
}
```