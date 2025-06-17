# VertexElement

Add this in your code:
```cpp
#include <engine/graphics/3d_graphics/vertex_descriptor.h>
```

## Description

Enum of all different vertex element types.

## Values

---
| Name | Description |
|-|-|
NONE | Default value, no selected element
POSITION_32_BITS | Float position
POSITION_16_BITS | Short position, PSP only, Not tested, may not work
POSITION_8_BITS | Byte position, PSP only, Not tested, may not work
NORMAL_32_BITS | Float normal
NORMAL_16_BITS | Short normal, PSP only, Not tested, may not work
NORMAL_8_BITS | Byte normal, PSP only, Not tested, may not work
UV_32_BITS | Float UV
UV_16_BITS | Short UV, PSP only, Not tested, may not work
UV_8_BITS | Byte UV, PSP only, Not tested, may not work
COLOR_4_FLOATS | Float color
COLOR_32_BITS_UINT | Unsigned int32 color, PSP only