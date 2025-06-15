# SkyBox

Add this in your code:
```cpp
#include <engine/graphics/skybox.h>
```

## Description

Asset that references all textures that compose a skybox.

Can be used in the [Lighting Settigs](../../../manual/editor_ui/lighting_settigs.md) menu.

## Variables

---
| Name | Type | Description |
|-|-|-|
front | `std::shared_ptr<Texture>` | Front skybox texture
back | `std::shared_ptr<Texture>` | Back skybox texture
up | `std::shared_ptr<Texture>` | Up skybox texture
down | `std::shared_ptr<Texture>` | Down skybox texture
left | `std::shared_ptr<Texture>` | Left skybox texture
right | `std::shared_ptr<Texture>` | Right skybox texture