# SpriteRenderer

Add this in your code:
```cpp
#include <engine/graphics/2d_graphics/sprite_renderer.h>
```

## Description

Component that renders a sprite with a texture and material

## Public Methods

---
### GetColor
Get the color of the sprite.
```cpp
const Color& GetColor() const
```

---
### SetColor
Set the color of the sprite.

Parameters:
- `color`: New color
```cpp
void SetColor(const Color& color)
```

---
### GetMaterial
Get the material of the sprite.
```cpp
const std::shared_ptr<Material>& GetMaterial() const
```

---
### SetMaterial
Set the material of the sprite.

Parameters:
- `material`: New material
```cpp
void SetMaterial(const std::shared_ptr<Material>& material)
```

---
### GetTexture
Get the texture of the sprite.
```cpp
const std::shared_ptr<Texture>& GetTexture() const
```

---
### SetTexture
Set the texture of the sprite.

Parameters:
- `texture`: New texture
```cpp
void SetTexture(const std::shared_ptr<Texture>& texture)
```