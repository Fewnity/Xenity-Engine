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

## Public methods

### Max
Get the biggest value of the vector.
```cpp
float Max()
```
Code sample:
```cpp
Vector2Int vec = Vector2Int(1, 3);
float maxValue = vec.Max(); // = 3.0f
```

### Min
Get the smallest value of the vector.
```cpp
float Min()
```
Code sample:
```cpp
Vector2Int vec = Vector2Int(1, 3);
float maxValue = vec.Min(); // = 1.0f
```

### Normalized
Get a copy of this vector with a magnitude of 1 (Does not change vector values).
```cpp
Vector2Int Normalized() const
```
Code sample:
```cpp
Vector2Int vec = Vector2Int(1, 2);
Vector2Int normalizedVec = vec.Normalized();
```

### Normalize
Makes this vector have a magnitude of 1 (Changes vector values).
```cpp
Vector2Int Normalize()
```
Code sample:
```cpp
Vector2Int vec = Vector2Int(1, 2);
vec.Normalize();
```

### Magnitude
Get the length of this vector.
```cpp
float Magnitude() const
```
Code sample:
```cpp
Vector2Int vec = Vector2Int(1, 2);
float magnitude vec.Magnitude();
```

### SquaredMagnitude
Get the squared length of this vector.
```cpp
float SquaredMagnitude() const
```
Code sample:
```cpp
Vector2Int vec = Vector2Int(1, 2);
float squaredMagnitude vec.SquaredMagnitude();
```

### ToString
Return a string representation of the vector like "{x:1, y:2}"
```cpp
bool ToString() const
```
Code sample:
```cpp
Vector2Int vec = Vector2Int(1, 2);
Debug::Print(vec->ToString()); // Prints "{x:1, y:2}"
```

## Static methods

### Distance
Distance between two vectors.

Parameters:
- `a`: First vector
- `b`: Second vector
```cpp
float Distance(const Vector2Int& a, const Vector2Int& b)
```
Code sample:
```cpp
Vector2Int a = Vector2Int(1, 3);
Vector2Int b = Vector2Int(2, 2);
float distance = Vector2Int::Distance(a, b);
```

## Operators

```cpp
Vector2Int operator+(const Vector2Int& left, const Vector2Int& right)
Vector2Int operator-(const Vector2Int& left, const Vector2Int& right)
Vector2Int operator-(const Vector2Int& vec)
Vector2Int operator*(const float value, const Vector2Int& vec)
Vector2Int operator*(const Vector2Int& left, const Vector2Int& right)
Vector2Int operator*(const Vector2Int& vec, const float value)
Vector2Int operator/(const float value, const Vector2Int& vec)
Vector2Int operator/(const Vector2Int& vec, const float value)
Vector2Int operator/(const Vector2Int& left, const Vector2Int& right)
Vector2Int& operator+=(Vector2Int& vec, const float value)
Vector2Int& operator-=(Vector2Int& vec, const float value)
Vector2Int& operator*=(Vector2Int& vec, const float value)
Vector2Int& operator/=(Vector2Int& vec, const float value)
Vector2Int& operator+=(Vector2Int& vec, const Vector2Int& vecRight)
Vector2Int& operator-=(Vector2Int& vec, const Vector2Int& vecRight)
Vector2Int& operator*=(Vector2Int& vec, const Vector2Int& vecRight)
Vector2Int& operator/=(Vector2Int& vec, const Vector2Int& vecRight)
bool operator==(const Vector2Int& left, const Vector2Int& right)
bool operator!=(const Vector2Int& left, const Vector2Int& right)
```