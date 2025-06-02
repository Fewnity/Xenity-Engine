# Vector4

Add this in your code:
```cpp
#include <engine/math/vector4.h>
```

## Description

Contains 4D coordinates.

## Variables

---
| Name | Type | Description |
|-|-|-|
x | `float` | The x component of the vector
y | `float` | The y component of the vector
z | `float` | The z component of the vector
w | `float` | The w component of the vector

## Constructors

### Vector4
Empty vector.

```cpp
Vector4()
```
Code sample:
```cpp
Vector4 vec = Vector4();
// X = Y = Z = W = 0.0f
```

### Vector4 (X,Y,Z,W)
Parameters:
- `x`: X value
- `y`: Y value
- `z`: Z value
- `w`: W value
```cpp
Vector4(float x, float y, float z,  float w)
```
Code sample:
```cpp
Vector4 vec = Vector4(1.0f, 2.0f, 3.0f, 4.0f);
```

### Vector4 (Single value)
Fill the vector with a single value.

Parameters:
- `fillValue`: Fill value
```cpp
Vector4(float fillValue)
```
Code sample:
```cpp
Vector4 vec = Vector4(1.0f);
// X = Y = Z = W = 1.0f
```

### Vector4 (Vector3)
Parameters:
- `vect`: 3D vector
```cpp
Vector4(const Vector3& vect)
```
Code sample:
```cpp
Vector3 vec3 = Vector3(1.0f, 2.0f, 3.0f);
Vector4 vec = Vector4(vec3);
// X = 1.0f
// Y = 2.0f
// Z = 3.0f
// W = 0.0f
```

### Vector4 (Vector2)
Parameters:
- `vect`: 2D vector
```cpp
Vector4(const Vector2& vect)
```
Code sample:
```cpp
Vector2 vec2 = Vector2(1.0f, 2.0f);
Vector4 vec = Vector4(vec2);
// X = 1.0f
// Y = 2.0f
// Z = 0.0f
// W = 0.0f
```

### Vector4 (Vector2Int)
Parameters:
- `vect`: 2D integer vector
```cpp
Vector4(const Vector2Int& vect)
```
Code sample:
```cpp
Vector2Int vec2int = Vector2Int(1, 2);
Vector4 vec = Vector4(vec2int);
// X = 1.0f
// Y = 2.0f
// Z = 0.0f
// W = 0.0f
```