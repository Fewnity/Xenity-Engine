# WrapMode Enum

Add this in your code:
```cpp
#include <engine/graphics/texture.h>
```

## Description

Enum that defines how texture coordinates outside the [0, 1] range are handled when sampling a texture.

## Values

---
| Name | Description |
|-|-|
ClampToEdge | Clamps texture coordinates to the nearest edge, causing the texture’s edge pixels to stretch
Repeat | Wraps texture coordinates around, repeating the texture pattern infinitely