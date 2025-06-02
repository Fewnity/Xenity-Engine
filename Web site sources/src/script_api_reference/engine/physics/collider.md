# Collider

Add this in your code:
```cpp
#include <engine/physics/collider.h>
```

## Description

Parent class of collider components (BoxCollider, SphereCollider...).

Function of Collider can be used on every colllider component.

## Public methods

---
### SetIsTrigger
Set if the collider is a trigger or not.<br>
A trigger collider will not collide with other colliders, but will still generate collision events.

Parameters:
- `isTrigger`: New isTrigger value
```cpp
void SetIsTrigger(bool isTrigger)
```
Code sample:
```cpp
std::shared_ptr<BoxColldier> boxCollider = GetGameObject()->AddComponent<BoxColldier>(); 
boxCollider->SetIsTrigger(true);
```

---
### IsTrigger
Check if the collider is a trigger or not.
```cpp
bool IsTrigger() const
```
Code sample:
```cpp
std::shared_ptr<BoxColldier> boxCollider = GetGameObject()->AddComponent<BoxColldier>(); 
boxCollider->SetIsTrigger(true);
bool isTrigger = boxCollider->IsTrigger(); // = true
```

---
### SetGenerateCollisionEvents
Set if the collider should generate collision events or not.

Parameters:
- `generateCollisionEvents`: New generateCollisionEvents value
```cpp
void SetGenerateCollisionEvents(bool generateCollisionEvents)
```
Code sample:
```cpp
std::shared_ptr<BoxColldier> boxCollider = GetGameObject()->AddComponent<BoxColldier>(); 
boxCollider->SetGenerateCollisionEvents(true);
```

---
### GetGenerateCollisionEvents
Check if the collider should generate collision events or not.
```cpp
bool GetGenerateCollisionEvents() const
```
Code sample:
```cpp
std::shared_ptr<BoxColldier> boxCollider = GetGameObject()->AddComponent<BoxColldier>(); 
boxCollider->SetGenerateCollisionEvents(true);
bool generateCollisionEvents = boxCollider->GetGenerateCollisionEvents(); // = true
```