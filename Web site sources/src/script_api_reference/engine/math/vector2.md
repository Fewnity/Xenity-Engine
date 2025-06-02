# Vector2

Add this in your code:
```cpp
#include <engine/math/vector2.h>
```

## Description

Contains 2D coordinates.

## Variables

---
| Name | Type | Description |
|-|-|-|
x | `float` | The x component of the vector
y | `float` | The y component of the vector

## Constructors

### Vector2
Empty vector.

```cpp
Vector2()
```
Code sample:
```cpp
Vector2 vec = Vector2();
// X = Y = 0.0f
```

### Vector2 (X,Y)
Parameters:
- `x`: X value
- `y`: Y value
```cpp
Vector2(float x, float y)
```
Code sample:
```cpp
Vector2 vec = Vector2(1.0f, 2.0f);
```

### Vector2 (Single value)
Fill the vector with a single value.

Parameters:
- `fillValue`: Fill value
```cpp
Vector2(float fillValue)
```
Code sample:
```cpp
Vector2 vec = Vector2(1.0f);
// X = Y = 1.0f
```

### Vector2 (Vector3)
Parameters:
- `vect`: 3D vector
```cpp
Vector2(const Vector3& vect)
```
Code sample:
```cpp
Vector3 vec3 = Vector3(1.0f, 2.0f, 3.0f);
Vector2 vec = Vector2(vec3);
// X = 1.0f
// Y = 2.0f
```

### Vector2 (Vector2Int)
Parameters:
- `vect`: 2D integer vector
```cpp
Vector2(const Vector2Int& vect)
```
Code sample:
```cpp
Vector2Int vec2int = Vector2Int(1, 2);
Vector2 vec = Vector2(vec2int);
// X = 1.0f
// Y = 2.0f
```