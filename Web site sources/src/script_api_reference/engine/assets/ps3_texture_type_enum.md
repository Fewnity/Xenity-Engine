# PS3TextureType Enum

Add this in your code:
```cpp
#include <engine/graphics/texture.h>
```

## Description

Enum that specifies the pixel format used for textures on the PS3, defining how color data is stored.

## Values

---
| Name | Description |
|-|-|
ARGB_8888 | Stores 8 bits per channel, offering full 32-bit color with high precision for each component
ARGB_1555 | Uses 5 bits for red, green, and blue, and 1 bit for alpha, providing a compact 16-bit format with limited transparency support
ARGB_0565 | Allocates 5 bits for red, 6 for green, and 5 for blue, with no alpha channel, optimizing color depth over transparency
ARGB_4444 | Provides 4 bits per channel, balancing color and transparency in a compact 16-bit format