# BillboardRenderer

Add this in your code:
```cpp
#include <engine/graphics/2d_graphics/billboard_renderer.h>
```

## Description

Component to render a billboard sprite (2D sprite that always faces the camera).

## Public methods

---
### SetColor
Set the color of the sprite.

Parameters:
- `Color`: New sprite color
```cpp
void SetColor(const Color& color)
```

---
### SetMaterial
Set the material of the sprite.

Parameters:
- `material`: New sprite material
```cpp
void SetMaterial(const std::shared_ptr<Material>& material)
```

---
### SetTexture
Set the texture of the sprite.

Parameters:
- `texture`: New sprite texture
```cpp
void SetTexture(const std::shared_ptr<Texture>& texture)
```

---
### GetMaterial
Get the material of the sprite.
```cpp
const std::shared_ptr<Material>& GetMaterial()
```

---
### GetTexture
Get the texture of the sprite.
```cpp
const std::shared_ptr<Texture>& GetTexture()
```