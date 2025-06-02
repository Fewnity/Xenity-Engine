# Quaternion

Add this in your code:
```cpp
#include <engine/math/quaternion.h>
```

## Description

Class representing rotations in 3D space

## Variables

---
| Name | Type | Description |
|-|-|-|
x | `float` | The x component of the quaternion
y | `float` | The y component of the quaternion
z | `float` | The z component of the quaternion
w | `float` | The w component of the quaternion

## Constructors

### Quaternion
```cpp
Quaternion()
```
Code sample:
```cpp
Quaternion q = Quaternion();
// Same as q = Quaternion::Identity();
```

### Quaternion (X,Y,Z,W)
Normalized.

Parameters:
- `x`: X value
- `y`: Y value
- `z`: Z value
- `w`: W value
```cpp
Quaternion(float x, float y, float z, float w)
```
Code sample:
```cpp
Quaternion q = Quaternion(0.6860413f, 0.3493627f, -0.4916947f, -0.4068529f);
```

### Quaternion (Vector4)
Normalized.

Parameters:
- `vector`: X, Y, Z, W values
```cpp
Quaternion(const Vector4& vector)
```
Code sample:
```cpp
Quaternion q = Quaternion(Vector4(0.6860413f, 0.3493627f, -0.4916947f, -0.4068529f));
```

## Static methods

---
### Identity
Returns a quaternion representing no rotation (identity quaternion).
```cpp
Quaternion Identity()
```
Code sample:
```cpp
Quaternion identity = Quaternion::Identity();
```

---
### Euler
Create a quaternion from Euler angles (in degrees).

Parameters:
- `x`: X angle in degrees
- `y`: Y angle in degrees
- `z`: Z angle in degrees
```cpp
Quaternion Euler(const float x, const float y, const float z)
```
Code sample:
```cpp
Quaternion q = Quaternion::Euler(10, 20, 30);
```

---
### AngleAxis
Make a quaternion from an angle and axis.

Parameters:
- `angle`: angle in degrees
- `axis`: Axis of the rotation
```cpp
Quaternion AngleAxis(float angle, const Vector3& axis)
```
Code sample:
```cpp
Quaternion q = Quaternion::AngleAxis(10, Vector3(1, 0, 0));
```

---
### Inverse
Get the inverse of the quaternion.

Parameters:
- `q`: Quaternion to inverse
```cpp
Quaternion Inverse(const Quaternion& q)
```
Code sample:
```cpp
Quaternion q = Quaternion::Euler(10, 20, 30);
Quaternion inverseQ = Quaternion::Inverse(q);
```

---
### Lerp
Linearly interpolates between quaternions.

Parameters:
- `a`: First quaternion
- `b`: Second quaternion
- `t`: Interpolation factor (0.0f to 1.0f)
```cpp
Quaternion Lerp(const Quaternion& a, const Quaternion& b, const float t)
```
Code sample:
```cpp
Quaternion a = Quaternion::Euler(10, 20, 30);
Quaternion b = Quaternion::Euler(90, 120, 10);
Quaternion mix = Quaternion::Lerp(a, b, 0.5f); // Mix of A and B
Quaternion lerp0 = Quaternion::Lerp(a, b, 0.0f); // Equals A
Quaternion lerp1 = Quaternion::Lerp(a, b, 1.0f); // Equals B
```

---
### Dot
Get the dot product of two quaternions.

Parameters:
- `q1`: First quaternion
- `q2`: Second quaternion
```cpp
float Dot(const Quaternion& q1, const Quaternion& q2)
```
Code sample:
```cpp
Quaternion a = Quaternion::Euler(10, 20, 30);
Quaternion b = Quaternion::Euler(90, 120, 10);
Quaternion result = Quaternion::Dot(a, b);
```

## Public methods

---
### ToEuler
Converts the quaternion to Euler angles (in degrees).
```cpp
Vector3 ToEuler() const
```
Code sample:
```cpp
Quaternion a = Quaternion::Euler(10, 20, 30);
Vector3 euler = a.ToEuler();
```

---
### Set
Set the quaternion values (Normalizes the quaternion).

Parameters:
- `x`: X value
- `y`: Y value
- `z`: Z value
- `w`: W value
```cpp
void Set(const float x, const float y, const float z, const float w)
```
Code sample:
```cpp
Quaternion a = Quaternion::Identity();
a.Set(0.6860413f * 2, 0.3493627f * 2, -0.4916947f * 2, -0.4068529f * 2);
// a = (0.6860413f, 0.3493627f, -0.4916947f, -0.4068529f)
```

---
### Normalize
Normalize the quaternion.
```cpp
void Normalize()
```
Code sample:
```cpp
Quaternion a = Quaternion::Identity();
a.Set(0.6860413f, 0.3493627f, -0.4916947f, -0.4068529f);
a.Normalize();
```

---
### Normalized
Returns a normalized copy of the quaternion.
```cpp
Quaternion Normalized() const
```
Code sample:
```cpp
Quaternion a = Quaternion::Identity();
a.Set(0.6860413f, 0.3493627f, -0.4916947f, -0.4068529f);
Quaternion normalizedA = a.Normalized();
```

---
### GetForward
Get the forward direction vector of the quaternion.
```cpp
Vector3 GetForward() const
```
Code sample:
```cpp
Quaternion a = Quaternion::Identity();
a.Set(0.6860413f, 0.3493627f, -0.4916947f, -0.4068529f);
Vector3 forward = a.GetForward();
```

---
### GetUp
Get the up direction vector of the quaternion.
```cpp
Vector3 GetUp() const
```
Code sample:
```cpp
Quaternion a = Quaternion::Identity();
a.Set(0.6860413f, 0.3493627f, -0.4916947f, -0.4068529f);
Vector3 up = a.GetUp();
```

---
### GetRight
Get the right direction vector of the quaternion.
```cpp
Vector3 GetRight() const
```
Code sample:
```cpp
Quaternion a = Quaternion::Identity();
a.Set(0.6860413f, 0.3493627f, -0.4916947f, -0.4068529f);
Vector3 right = a.GetRight();
```

---
### ToString
Return a string representation of the quaternion like "{x:0.0 y:0.0 z:0.0 w:1.0}"
```cpp
 std::string ToString() const
```
Code sample:
```cpp
Quaternion a = Quaternion::Identity();
a.Set(0.6860413f, 0.3493627f, -0.4916947f, -0.4068529f);
Debug::Print(a.ToString());
// Prints "{x:0.6860413 y:0.3493627 z:0.4916947 w:0.4068529}"
```