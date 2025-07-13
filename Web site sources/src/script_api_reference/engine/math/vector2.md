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

## Public methods

### Max
Get the biggest value of the vector.
```cpp
float Max()
```
Code sample:
```cpp
Vector2 vec = Vector2(1.0f, 3.0f);
float maxValue = vec.Max(); // = 3.0f
```

### Min
Get the smallest value of the vector.
```cpp
float Min()
```
Code sample:
```cpp
Vector2 vec = Vector2(1.0f, 3.0f);
float maxValue = vec.Min(); // = 1.0f
```

### Normalized
Get a copy of this vector with a magnitude of 1 (Does not change vector values).
```cpp
Vector2 Normalized() const
```
Code sample:
```cpp
Vector2 vec = Vector2(1.0f, 2.0f);
Vector2 normalizedVec = vec.Normalized();
```

### Normalize
Makes this vector have a magnitude of 1 (Changes vector values).
```cpp
Vector2 Normalize()
```
Code sample:
```cpp
Vector2 vec = Vector2(1.0f, 2.0f);
vec.Normalize();
```

### Magnitude
Get the length of this vector.
```cpp
float Magnitude() const
```
Code sample:
```cpp
Vector2 vec = Vector2(1.0f, 2.0f);
float magnitude vec.Magnitude();
```

### SquaredMagnitude
Get the squared length of this vector.
```cpp
float SquaredMagnitude() const
```
Code sample:
```cpp
Vector2 vec = Vector2(1.0f, 2.0f);
float squaredMagnitude vec.SquaredMagnitude();
```

### HasInvalidValues
Return True is the vector has invalid values (NaN or Inf).
```cpp
bool HasInvalidValues() const
```
Code sample:
```cpp
Vector2 vec = Vector2(std::nanf(""), std::numeric_limits<float>::infinity()); // This will raise an assert in debug to avoid using nan or inf values
bool valid = vec->HasInvalidValues();
```

### ToString
Return a string representation of the vector like "{x:1.0, y:2.0}"
```cpp
bool ToString() const
```
Code sample:
```cpp
Vector2 vec = Vector2(1.0f, 2.0f);
Debug::Print(vec->ToString()); // Prints "{x:1.0, y:2.0}"
```

## Static methods

### Lerp
Linearly interpolates between vectors.

Parameters:
- `a`: First vector
- `b`: Second vector
- `t`: Interpolation factor (0.0f to 1.0f)
```cpp
Vector2 Lerp(const Vector2& a, const Vector2& b, const float t)
```
Code sample:
```cpp
Vector2 a = Vector2(1.0f, 3.0f);
Vector2 b = Vector2(2.0f, 2.0f);
Vector2 mix = Vector2::Lerp(a, b, 0.5f); // Mix of A and B
Vector2 lerp0 = Vector2::Lerp(a, b, 0.0f); // Equals A
Vector2 lerp1 = Vector2::Lerp(a, b, 1.0f); // Equals B
```

### Distance
Distance between two vectors.

Parameters:
- `a`: First vector
- `b`: Second vector
```cpp
float Distance(const Vector2& a, const Vector2& b)
```
Code sample:
```cpp
Vector2 a = Vector2Int(1.0f, 3.0f);
Vector2 b = Vector2Int(2.0f, 2.0f);
float distance = Vector2::Distance(a, b);
```

## Operators

```cpp
Vector2 operator+(const Vector2& left, const Vector2& right)
Vector2 operator-(const Vector2& left, const Vector2& right)
Vector2 operator-(const Vector2& vec)
Vector2 operator*(const float value, const Vector2& vec)
Vector2 operator*(const Vector2& left, const Vector2& right)
Vector2 operator*(const Vector2& vec, const float value)
Vector2 operator*(const Vector2& left, const Vector2Int& right);
Vector2 operator*(const Vector2Int& left, const Vector2& right);
Vector2 operator/(const float value, const Vector2& vec)
Vector2 operator/(const Vector2& vec, const float value)
Vector2 operator/(const Vector2& left, const Vector2& right)
Vector2& operator+=(Vector2& vec, const float value)
Vector2& operator-=(Vector2& vec, const float value)
Vector2& operator*=(Vector2& vec, const float value)
Vector2& operator/=(Vector2& vec, const float value)
Vector2& operator+=(Vector2& vec, const Vector2& vecRight)
Vector2& operator-=(Vector2& vec, const Vector2& vecRight)
Vector2& operator*=(Vector2& vec, const Vector2& vecRight)
Vector2& operator/=(Vector2& vec, const Vector2& vecRight)
bool operator==(const Vector2& left, const Vector2& right)
bool operator!=(const Vector2& left, const Vector2& right)
```