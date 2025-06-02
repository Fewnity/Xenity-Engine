# Vector2Int

Add this in your code:
```cpp
#include <engine/math/vector2_int.h>
```

## Description

Contains integer 2D coordinates.
## Variables

---
| Name | Type | Description |
|-|-|-|
x | `int` | The x component of the vector
y | `int` | The y component of the vector

## Constructors

### Vector2Int
Empty vector.

```cpp
Vector2Int()
```
Code sample:
```cpp
Vector2Int vec = Vector2Int();
// X = Y = 0
```

### Vector2Int (X,Y)
Parameters:
- `x`: X value
- `y`: Y value
```cpp
Vector2Int(int x, int y)
```
Code sample:
```cpp
Vector2Int vec = Vector2Int(1, 2);
```

### Vector2Int (Single value)
Fill the vector with a single value.

Parameters:
- `fillValue`: Fill value
```cpp
Vector2Int(int fillValue)
```
Code sample:
```cpp
Vector2Int vec = Vector2Int(1);
// X = Y = 1
```

### Vector2Int (Vector3)
Parameters:
- `vect`: 3D vector
```cpp
Vector2Int(const Vector3& vect)
```
Code sample:
```cpp
Vector3 vec3 = Vector3(1.0f, 2.0f, 3.0f);
Vector2Int vec = Vector2Int(vec3);
// X = 1
// Y = 2
```

### Vector2Int (Vector2)
Parameters:
- `vect`: 2D vector
```cpp
Vector2Int(const Vector2& vect)
```
Code sample:
```cpp
Vector2 vec2 = Vector2(1.0f, 2.0f);
Vector2Int vec = Vector2Int(vec2);
// X = 1
// Y = 2
```