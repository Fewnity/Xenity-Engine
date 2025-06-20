# Transform

Add this in your code:
```cpp
#include <engine/game_elements/transform.h>
```

## Description

Class representing a 3D transformation (position, rotation, scale) of a GameObject.

## Public Methods

---
### GetPosition
Get world position.
```cpp
const Vector3& GetPosition() const
```

---
### SetPosition
Set world position.

Parameters:
- `position`: New world position
```cpp
void SetPosition(const Vector3& position)
```

---
### GetLocalPosition
Get local position.
```cpp
const Vector3& GetLocalPosition() const
```

---
### SetLocalPosition
Set local position.

Parameters:
- `position`: New world position
```cpp
void SetLocalPosition(const Vector3& position)
```

---
### GetEulerAngles
Get rotation (in degree).
```cpp
const Vector3& GetEulerAngles() const
```

---
### GetLocalEulerAngles
Get local rotation (in degree).
```cpp
const Vector3& GetLocalEulerAngles() const
```

---
### GetRotation
Get rotation.
```cpp
const Quaternion& GetRotation() const
```

---
### GetLocalRotation
Get local rotation.
```cpp
const Quaternion& GetLocalRotation() const
```

---
### SetEulerAngles
Set world rotation in degree.

Parameters:
- `rotation`: New world rotation
```cpp
void SetEulerAngles(const Vector3& rotation)
```

---
### SetLocalEulerAngles
Set local rotation in degree.

Parameters:
- `rotation`: New local rotation
```cpp
void SetLocalEulerAngles(const Vector3& rotation)
```

---
### SetRotation
Set world rotation.

Parameters:
- `rotation`: New world rotation
```cpp
void SetRotation(const Quaternion& rotation)
```

---
### SetLocalRotation
Set local rotation.

Parameters:
- `rotation`: New local rotation
```cpp
void SetLocalRotation(const Quaternion& rotation)
```

---
### GetScale
Get scale.
```cpp
const Vector3& GetScale() const
```

---
### GetLocalScale
Get local scale.
```cpp
const Vector3& GetLocalScale() const
```

---
### SetLocalScale
Set local scale.

Parameters:
- `scale`: New local scale
```cpp
void SetLocalScale(const Vector3& scale)
```

---
### GetForward
Get forward direction.
```cpp
Vector3 GetForward() const
```

---
### GetBackward
Get backward direction.
```cpp
Vector3 GetBackward() const
```

---
### GetLeft
Get left direction.
```cpp
Vector3 GetLeft() const
```

---
### GetRight
Get right direction.
```cpp
Vector3 GetRight() const
```

---
### GetUp
Get up direction.
```cpp
Vector3 GetUp() const
```

---
### GetDown
Get down direction.
```cpp
Vector3 GetDown() const
```

---
### GetTransformationMatrix
Set transformation matrix.
```cpp
const glm::mat4& GetTransformationMatrix() const
```

---
### SetTransformationMatrix
Set transformation matrix.

Parameters:
- `matrix`: New matrix
```cpp
void SetTransformationMatrix(const glm::mat4& matrix)
```

---
### GetGameObject
Get GameObject.
```cpp
std::shared_ptr<GameObject> GetGameObject() const
```

---
### GetOnTransformUpdated
Get the event that is called when the transform is updated (new position, or new rotation or new scale).
```cpp
Event<>& GetOnTransformUpdated()
```

---
### GetOnTransformScaled
Get the event that is called when the transform is scaled.
```cpp
Event<>& GetOnTransformScaled()
```