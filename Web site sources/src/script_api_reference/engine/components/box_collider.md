# BoxCollider

Add this in your code:
```cpp
#include <engine/physics/box_collider.h>
```

## Description

Component to add a cube-shaped collider to a GameObject.

## Public methods

---
### SetSize
Set the size of the box collider.

Parameters:
- `size`: New size value
```cpp
void SetSize(const Vector3& size)
```

---
### SetOffset
Set the offset position of the box collider.

Parameters:
- `offset`: New offset value
```cpp
void SetOffset(const Vector3& offset)
```

---
### GetSize
Get the size of the box collider.
```cpp
const Vector3& GetSize() const
```

---
### GetOffset
Get the offset position of the box collider.
```cpp
const Vector3& GetOffset() const
```

---
### ToString
Get the size and offset of the box collider in the form of a string.
```cpp
std::string ToString()
```