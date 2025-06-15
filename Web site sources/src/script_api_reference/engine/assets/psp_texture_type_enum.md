# PSPTextureType Enum

Add this in your code:
```cpp
#include <engine/graphics/texture.h>
```

## Description

Enum that specifies the pixel format used for textures on the PSP, defining how color data is stored.

## Values

---
| Name | Description |
|-|-|
RGBA_8888 | Stores 8 bits per channel, offering full 32-bit color with high precision for each component
RGBA_5551 | Uses 5 bits for red, green, and blue, and 1 bit for alpha, providing a compact 16-bit format with limited transparency support
RGBA_5650 | Allocates 5 bits for red, 6 for green, and 5 for blue, with no alpha channel, optimizing color depth over transparency
RGBA_4444 | Provides 4 bits per channel, balancing color and transparency in a compact 16-bit format