# Filter Enum

Add this in your code:
```cpp
#include <engine/graphics/texture.h>
```

## Description

Enum to define how a texture should be filtered.

## Values

---
| Name | Description |
|-|-|
Point | Point filtering selects the nearest texel without interpolation, giving a sharp, pixelated look
Bilinear | Bilinear filtering smooths textures by interpolating between the four nearest texels, resulting in a softer, more blended appearance