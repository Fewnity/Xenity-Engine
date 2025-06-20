# RGBA

Add this in your code:
```cpp
#include <engine/graphics/color/color.h>
```

## Description

This class stores information about a color.

## Constructors

---
### RGBA
Parameters:
- `r`: Red level [0.0f;1.0f]
- `g`: Green level [0.0f;1.0f]
- `b`: Blue level [0.0f;1.0f]
- `a`: Alpha level [0.0f;1.0f]
```cpp
constexpr RGBA(float r, float g, float b, float a)
```
Code sample:
```cpp
RGBA myRGBA = RGBA(1.0f, 0.6f, 0.3f, 0.1f);
```

## Public methods

---
### ToVector4
Get RGBA as a Vector4 [0.0f;1.0f] x = red, y = green, z = blue, w = alpha.
```cpp
Vector4 ToVector4() const
```
Code sample:
```cpp
RGBA myRGBA = RGBA(1.0f, 0.6f, 0.3f, 0.1f);
Vector4 rgbaAsVector4 = myRGBA.ToVector4();
```

## Variables

---
| Name | Type | Description |
|-|-|-|
r | `float` | Red information [0.0f;1.0f]
g | `float` | Green information [0.0f;1.0f]
b | `float` | Blue information [0.0f;1.0f]
a | `float` | Alpha information [0.0f;1.0f]