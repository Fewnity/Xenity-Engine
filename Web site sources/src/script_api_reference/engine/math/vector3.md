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

## Public methods

### Max
Get the biggest value of the vector.
```cpp
float Max()
```
Code sample:
```cpp
Vector3 vec = Vector3(1.0f, 3.0f, 2.0f);
float maxValue = vec.Max(); // = 3.0f
```

### Min
Get the smallest value of the vector.
```cpp
float Min()
```
Code sample:
```cpp
Vector3 vec = Vector3(1.0f, 3.0f, 2.0f);
float maxValue = vec.Min(); // = 1.0f
```

### Normalized
Get a copy of this vector with a magnitude of 1 (Does not change vector values).
```cpp
Vector3 Normalized() const
```
Code sample:
```cpp
Vector3 vec = Vector3(1.0f, 2.0f, 3.0f);
Vector3 normalizedVec = vec.Normalized();
```

### Normalize
Makes this vector have a magnitude of 1 (Changes vector values).
```cpp
Vector3 Normalize()
```
Code sample:
```cpp
Vector3 vec = Vector3(1.0f, 1.0f, 3.0f);
vec.Normalize();
```

### Magnitude
Get the length of this vector.
```cpp
float Magnitude() const
```
Code sample:
```cpp
Vector3 vec = Vector3(1.0f, 2.0f, 3.0f);
float magnitude vec.Magnitude();
```

### SquaredMagnitude
Get the squared length of this vector.
```cpp
float SquaredMagnitude() const
```
Code sample:
```cpp
Vector3 vec = Vector3(1.0f, 2.0f, 3.0f);
float squaredMagnitude vec.SquaredMagnitude();
```

### Dot
Get the dot product of this vector with another vector.

Parameters:
- `v`: Other vector
```cpp
float Dot(const Vector3& v) const
```
Code sample:
```cpp
Vector3 vec = Vector3(1.0f, 2.0f, 3.0f);
Vector3 vec2 = Vector3(2.0f, 2.0f, 4.0f);
float dotResult vec.Dot(vec2);
```

### Cross
Get the cross product of this vector with another vector.

Parameters:
- `v`: Other vector
```cpp
Vector3 Cross(const Vector3& v) const
```
Code sample:
```cpp
Vector3 vec = Vector3(1.0f, 2.0f, 3.0f);
Vector3 vec2 = Vector3(2.0f, 2.0f, 4.0f);
Vector3 crossResult vec.Cross(vec2);
```

### HasInvalidValues
Return True is the vector has invalid values (NaN or Inf).
```cpp
bool HasInvalidValues() const
```
Code sample:
```cpp
Vector3 vec = Vector3(std::nanf(""), std::numeric_limits<float>::infinity(), 3.0f); // This will raise an assert in debug to avoid using nan or inf values
bool valid = vec->HasInvalidValues();
```

### ToString
Return a string representation of the vector like "{x:1.0, y:2.0, z:3.0}"
```cpp
bool ToString() const
```
Code sample:
```cpp
Vector3 vec = Vector3(1.0f, 2.0f, 3.0f);
Debug::Print(vec->ToString()); // Prints "{x:1.0, y:2.0, z:3.0}"
```

## Static methods

### Dot
Get the dot product of two vectors.

Parameters:
- `a`: First vector
- `b`: Sedond vector
```cpp
float Dot(const Vector3& a, const Vector3& b)
```
Code sample:
```cpp
Vector3 vec = Vector3(1.0f, 2.0f, 3.0f);
Vector3 vec2 = Vector3(2.0f, 2.0f, 4.0f);
float dotResult Vector3::Dot(vec, vec2);
```

### Cross
Get the cross product of two vectors.

Parameters:
- `a`: First vector
- `b`: Sedond vector
```cpp
Vector3 Cross(const Vector3& a, const Vector3& b)
```
Code sample:
```cpp
Vector3 vec = Vector3(1.0f, 2.0f, 3.0f);
Vector3 vec2 = Vector3(2.0f, 2.0f, 4.0f);
Vector3 crossResult Vector3::Cross(vec, vec2);
```

### Lerp
Linearly interpolates between vectors.

Parameters:
- `a`: First vector
- `b`: Second vector
- `t`: Interpolation factor (0.0f to 1.0f)
```cpp
Vector3 Lerp(const Vector3& a, const Vector3& b, const float t)
```
Code sample:
```cpp
Vector3 a = Vector3(1.0f, 2.0f, 3.0f);
Vector3 b = Vector3(2.0f, 2.0f, 4.0f);
Vector3 mix = Vector3::Lerp(a, b, 0.5f); // Mix of A and B
Vector3 lerp0 = Vector3::Lerp(a, b, 0.0f); // Equals A
Vector3 lerp1 = Vector3::Lerp(a, b, 1.0f); // Equals B
```

### Distance
Distance between two vectors.

Parameters:
- `a`: First vector
- `b`: Second vector
```cpp
float Distance(const Vector3& a, const Vector3& b)
```
Code sample:
```cpp
Vector3 a = Vector3(1.0f, 2.0f, 3.0f);
Vector3 b = Vector3(2.0f, 2.0f, 4.0f);
float distance = Vector3::Distance(a, b);
```

### LookAt
Get the look rotation in degrees between two vectors.

Parameters:
- `from`: First vector
- `to`: Target position
```cpp
Vector3 LookAt(const Vector3& from, const Vector3& to)
```
Code sample:
```cpp
Vector3 from = Vector3(1.0f, 2.0f, 3.0f);
Vector3 to = Vector3(2.0f, 2.0f, 4.0f);
Vector3 rotation = Vector3::LookAt(a, b);
```

## Operators

```cpp
Vector3 operator+(const Vector3& left, const Vector3& right)
Vector3 operator-(const Vector3& left, const Vector3& right)
Vector3 operator-(const Vector3& vec)
Vector3 operator*(const float value, const Vector3& vec)
Vector3 operator*(const Vector3& left, const Vector3& right)
Vector3 operator*(const Vector3& vec, const float value)
Vector3 operator*(const Quaternion& left, const Vector3& right)
Vector3 operator/(const float value, const Vector3& vec)
Vector3 operator/(const Vector3& vec, const float value)
Vector3 operator/(const Vector3& left, const Vector3& right)
Vector3& operator+=(Vector3& vec, const float value)
Vector3& operator-=(Vector3& vec, const float value)
Vector3& operator*=(Vector3& vec, const float value)
Vector3& operator/=(Vector3& vec, const float value)
Vector3& operator+=(Vector3& vec, const Vector3& vecRight)
Vector3& operator-=(Vector3& vec, const Vector3& vecRight)
Vector3& operator*=(Vector3& vec, const Vector3& vecRight)
Vector3& operator/=(Vector3& vec, const Vector3& vecRight)
bool operator==(const Vector3& left, const Vector3& right)
bool operator!=(const Vector3& left, const Vector3& right)
```