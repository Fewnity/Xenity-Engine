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

## Public methods

### Max
Get the biggest value of the vector.
```cpp
float Max()
```
Code sample:
```cpp
Vector4 vec = Vector4(1.0f, 3.0f, 4.0f, 2.0f);
float maxValue = vec.Max(); // = 4.0f
```

### Min
Get the smallest value of the vector.
```cpp
float Min()
```
Code sample:
```cpp
Vector4 vec = Vector4(1.0f, 3.0f, 4.0f, 2.0f);
float maxValue = vec.Min(); // = 1.0f
```

### Normalized
Get a copy of this vector with a magnitude of 1 (Does not change vector values).
```cpp
Vector4 Normalized() const
```
Code sample:
```cpp
Vector4 vec = Vector4(1.0f, 2.0f, 3.0f, 4.0f);
Vector4 normalizedVec = vec.Normalized();
```

### Normalize
Makes this vector have a magnitude of 1 (Changes vector values).
```cpp
Vector4 Normalize()
```
Code sample:
```cpp
Vector4 vec = Vector4(1.0f, 2.0f, 3.0f, 4.0f);
vec.Normalize();
```

### Magnitude
Get the length of this vector.
```cpp
float Magnitude() const
```
Code sample:
```cpp
Vector4 vec = Vector4(1.0f, 2.0f, 3.0f, 4.0f);
float magnitude vec.Magnitude();
```

### SquaredMagnitude
Get the squared length of this vector.
```cpp
float SquaredMagnitude() const
```
Code sample:
```cpp
Vector4 vec = Vector4(1.0f, 2.0f, 3.0f, 4.0f);
float squaredMagnitude vec.SquaredMagnitude();
```

### HasInvalidValues
Return True is the vector has invalid values (NaN or Inf).
```cpp
bool HasInvalidValues() const
```
Code sample:
```cpp
Vector4 vec = Vector4(std::nanf(""), std::numeric_limits<float>::infinity(), 3.0f, 4.0f); // This will raise an assert in debug to avoid using nan or inf values
bool valid = vec->HasInvalidValues();
```

### ToString
Return a string representation of the vector like "{x:1.0, y:2.0, z:3.0, w:4.0}"
```cpp
bool ToString() const
```
Code sample:
```cpp
Vector4 vec = Vector4(1.0f, 2.0f, 3.0f, 4.0f);
Debug::Print(vec->ToString()); // Prints "{x:1.0, y:2.0, z:3.0, w:4.0}"
```

## Static methods

### Lerp
Linearly interpolates between vectors.

Parameters:
- `a`: First vector
- `b`: Second vector
- `t`: Interpolation factor (0.0f to 1.0f)
```cpp
Vector4 Lerp(const Vector4& a, const Vector4& b, const float t)
```
Code sample:
```cpp
Vector4 a = Vector4(1.0f, 3.0f, 2.0f, 6.0f);
Vector4 b = Vector4(2.0f, 2.0f, 4.0f, 1.0f);
Vector4 mix = Vector4::Lerp(a, b, 0.5f); // Mix of A and B
Vector4 lerp0 = Vector4::Lerp(a, b, 0.0f); // Equals A
Vector4 lerp1 = Vector4::Lerp(a, b, 1.0f); // Equals B
```

## Operators

```cpp
Vector4 operator+(const Vector4& left, const Vector4& right)
Vector4 operator-(const Vector4& left, const Vector4& right)
Vector4 operator-(const Vector4& vec)
Vector4 operator*(const float value, const Vector4& vec)
Vector4 operator*(const Vector4& left, const Vector4& right)
Vector4 operator*(const Vector4& vec, const float value)
Vector4 operator/(const float value, const Vector4& vec)
Vector4 operator/(const Vector4& vec, const float value)
Vector4 operator/(const Vector4& left, const Vector4& right)
Vector4& operator+=(Vector4& vec, const float value)
Vector4& operator-=(Vector4& vec, const float value)
Vector4& operator*=(Vector4& vec, const float value)
Vector4& operator/=(Vector4& vec, const float value)
Vector4& operator+=(Vector4& vec, const Vector4& vecRight)
Vector4& operator-=(Vector4& vec, const Vector4& vecRight)
Vector4& operator*=(Vector4& vec, const Vector4& vecRight)
Vector4& operator/=(Vector4& vec, const Vector4& vecRight)
bool operator==(const Vector4& left, const Vector4& right)
bool operator!=(const Vector4& left, const Vector4& right)
```