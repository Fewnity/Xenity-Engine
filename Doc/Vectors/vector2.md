# Vector2

## Constructors
| Definition | Description |
|-|-|
Vector2() | Create default Vector2
Vector2(const float x, const float y) | Create a Vector2 with X and Y values
Vector2(const float fillValue) | Create a Vector2 with one value for X and Y
Vector2(const Vector3& vect3) | Create a Vector2 from a Vector3's X and Y values
Vector2(const Vector2Int& vect2Int) |  Create a Vector2 from a Vector2Int

## Methods
| Definition | Description |
|-|-|
Vector2 Normalized() | Get the Vector2 with normalised values
float Magnitude() | Get magnitude
static float Distance(const Vector2& a, const Vector2& b) | Distance between two Vector2
static Vector2 Lerp(const Vector2& a, const Vector2& b, const float t) | Lerp between two Vector2

## Members
| Name | Type |
|-|-|
x | float |
y | float |
