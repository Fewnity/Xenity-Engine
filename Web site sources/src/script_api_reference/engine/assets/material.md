# Material

Add this in your code:
```cpp
#include <engine/graphics/material.h>
```

## Description

Handles the visual appearance of rendered objects, including shaders and textures.<br>
The Material class allows you to configure how an object should look when drawn on screen.

## Inspector settings

---
| Name | Type | Description |
|-|-|-|
Shader | Shader | Set the material shader. Standard to use lighting, Unlit to disable lighting.
Rendering mode | Enum | Set the rendering mode of the material.
Texture | Texture | Set the material texture.
Color | Color | Set the color of the material.
Offset | Vector2 | Set the texture offset.
Tiling | Vector2 | Set the texture tiling.
Use lighting | CheckBox | Check this if you are using Standard shader, uncheck this if you are using Unlit shader.

## Public methods

---
### SetAttribute 
Set a value to an attribute in the shader when this material is used.

To use only when you are using custom shaders.

Parameters:
- `attribute`: Attribute name
- `value`: New attribute value
```cpp
void SetAttribute(const char* attribute, const Vector2& value)
void SetAttribute(const char* attribute, const Vector3& value)
void SetAttribute(const char* attribute, const Vector4& value)
void SetAttribute(const char* attribute, const float value)
void SetAttribute(const char* attribute, const int value)
```
Code sample:
```cpp
std::shared_ptr<Material> material; // Filled variable
material->SetAttribute("MyAttribute", Vector3(1, 0.5f, 1));
```

---
### SetShader 
Set the material shader.

Parameters:
- `shader`: Shader to use
```cpp
void SetShader(const std::shared_ptr<Shader>& shader)
```
Code sample:
```cpp
std::shared_ptr<Material> material; // Filled variable
std::shared_ptr<Shader> shader; // Filled variable
material->SetShader(shader);
```

---
### SetTexture 
Set the material texture.

Parameters:
- `texture`: Texture to use
```cpp
void SetTexture(const std::shared_ptr<Texture>& texture)
```
Code sample:
```cpp
std::shared_ptr<Material> material; // Filled variable
std::shared_ptr<Texture> texture; // Filled variable
material->SetTexture(texture);
```

---
### SetUseLighting 
Set the if the material uses lighting. Disable this only on Unlit shaders.

Parameters:
- `useLighting`: Does the material uses lighting?
```cpp
void SetUseLighting(const bool useLighting)
```
Code sample:
```cpp
std::shared_ptr<Material> material; // Filled variable
material->SetUseLighting(true);
```

---
### SetOffset 
Set the texture offset.

Parameters:
- `offset`: Offset of the texture
```cpp
void SetOffset(const Vector2& offset)
```
Code sample:
```cpp
std::shared_ptr<Material> material; // Filled variable
material->SetOffset(Vector2(0.5f, 0.5f));
```

---
### SetTiling 
Set the texture tiling.

Parameters:
- `tiling`: Tiling of the texture
```cpp
void SetTiling(const Vector2& tiling)
```
Code sample:
```cpp
std::shared_ptr<Material> material; // Filled variable
material->SetTiling(Vector2(2.0f, 2.0f));
```

---
### GetShader 
Get the shader of the material.
```cpp
 const std::shared_ptr<Shader>& GetShader() const
```
Code sample:
```cpp
std::shared_ptr<Material> material; // Filled variable
std::shared_ptr<Shader> shader = material->GetShader();
```

---
### GetTexture 
Get the texture of the material.
```cpp
const std::shared_ptr<Texture>& GetTexture() const
```
Code sample:
```cpp
std::shared_ptr<Material> material; // Filled variable
std::shared_ptr<Texture> texture = material->GetTexture();
```

---
### GetUseLighting 
Get if the material uses lighting.
```cpp
bool GetUseLighting() const
```
Code sample:
```cpp
std::shared_ptr<Material> material; // Filled variable
bool isUsingLighting = material->GetUseLighting();
```

---
### GetRenderingMode 
Get the rendering mode of the material.
```cpp
MaterialRenderingMode GetRenderingMode() const
```
Code sample:
```cpp
std::shared_ptr<Material> material; // Filled variable
MaterialRenderingMode mode = material->GetRenderingMode();
```

---
### GetOffset 
Get the texture offset of the material.
```cpp
const Vector2& GetOffset() const
```
Code sample:
```cpp
std::shared_ptr<Material> material; // Filled variable
const Vector2& offset = material->GetOffset();
```

---
### GetTiling 
Get the texture tiling of the material.
```cpp
const Vector2& GetTiling() const
```
Code sample:
```cpp
std::shared_ptr<Material> material; // Filled variable
const Vector2& tiling = material->GetTiling();
```

---
### GetColor 
Get the color of the material.
```cpp
const Color& GetColor() const
```
Code sample:
```cpp
std::shared_ptr<Material> material; // Filled variable
const Color& color = material->GetColor();
```

---
### SetColor 
Set the color of the material.

Parameters:
- `color`: Color of the material
```cpp
void SetColor(const Color& color)
```
Code sample:
```cpp
std::shared_ptr<Material> material; // Filled variable
material->SetColor(Color::CreateFromRGBAFloat(1, 0.5f, 0.5f, 1));
```

---
### SetAlphaCutoff 
Set the alpha cutoff of the material.

Parameters:
- `alphaCutoff`: New alpha cutoff value [0.0f;1.0f]
```cpp
void SetAlphaCutoff(float alphaCutoff)
```
Code sample:
```cpp
std::shared_ptr<Material> material; // Filled variable
material->SetAlphaCutoff(0.5f);
```

---
### GetAlphaCutoff 
Get the alpha cutoff of the material.
```cpp
float GetAlphaCutoff() const
```
Code sample:
```cpp
std::shared_ptr<Material> material; // Filled variable
float alphaCutoff = material->GetAlphaCutoff();
```