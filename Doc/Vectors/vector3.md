# Vector3

## Constructors
| Definition | Description |
|-|-|
Vector3() | Create default Vector2
Vector3(const float x, const float y, const float z) | Create a Vector3 with X, Y and Z values
Vector3(const float fillValue) | Create a Vector3 with one value for X, Y and Z
Vector3(Vector2Int vect) |   Create a Vector3 from a Vector2Int's X and Y values
Vector3(Vector2 vect) |  Create a Vector3 from a Vector2's X and Y values

## Methods
| Definition | Description |
|-|-|
static Vector3 LookAt(const Vector3& from, const Vector3& to) | Get the look rotation in degrees between two points
Vector3 Normalized() | Get the Vector3 with normalised values
float Magnitude() | Get magnitude
static float Distance(const Vector3& a, const Vector3& b) | Distance between two Vector3
static Vector3 Lerp(const Vector3& a, const Vector3& b, const float t) | Lerp between two Vector3

## Members
| Name | Type |
|-|-|
x | float |
y | float |
z | float |
