# Vector3

Add this in your code:
```cpp
#include <engine/math/vector3.h>
```

## Description

Contains 3D coordinates.

## Variables

---
| Name | Type | Description |
|-|-|-|
x | `float` | The x component of the vector
y | `float` | The y component of the vector
z | `float` | The z component of the vector

## Constructors

### Vector3
Empty vector.

```cpp
Vector3()
```
Code sample:
```cpp
Vector3 vec = Vector3();
// X = Y = Z = 0.0f
```

### Vector3 (X,Y,Z)
Parameters:
- `x`: X value
- `y`: Y value
- `z`: Z value
```cpp
Vector3(float x, float y, float z)
```
Code sample:
```cpp
Vector3 vec = Vector3(1.0f, 2.0f, 3.0f);
```

### Vector3 (Single value)
Fill the vector with a single value.

Parameters:
- `fillValue`: Fill value
```cpp
Vector3(float fillValue)
```
Code sample:
```cpp
Vector3 vec = Vector3(1.0f);
// X = Y = Z = 1.0f
```

### Vector3 (Vector2)
Parameters:
- `vect`: 2D vector
```cpp
Vector3(const Vector2& vect)
```
Code sample:
```cpp
Vector2 vec2 = Vector2(1.0f, 2.0f);
Vector3 vec = Vector3(vec2);
// X = 1.0f
// Y = 2.0f
// Z = 0.0f
```

### Vector3 (Vector2Int)
Parameters:
- `vect`: 2D integer vector
```cpp
Vector3(const Vector2Int& vect)
```
Code sample:
```cpp
Vector2Int vec2int = Vector2Int(1, 2);
Vector3 vec = Vector3(vec2int);
// X = 1.0f
// Y = 2.0f
// Z = 0.0f
```