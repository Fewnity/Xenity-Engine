# TextMesh

Add this in your code:
```cpp
#include <engine/graphics/ui/text_mesh.h>
```

## Description

Component for rendering text in 3D space.

## Public methods

---
### GetOrderInLayer
Get the order in layer, higher number, higher priority.
```cpp
int GetOrderInLayer() const
```

---
### SetOrderInLayer
Set the order in layer.

Parameters:
- `orderInLayer`: Order in layer, higher number, higher priority
```cpp
void SetOrderInLayer(int orderInLayer)
```

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
- `Color`: New sprite color
```cpp
void SetColor(const Color& color)
```

---
### GetText
Get text.
```cpp
const std::string& GetText() const
```

---
### SetText
Set text.

Parameters:
- `text`: New text
```cpp
void SetText(const std::string& text)
```

---
### GetFont
Get text font.
```cpp
const std::shared_ptr<Font>& GetFont() const
```

---
### SetFont
Set text font.

Parameters:
- `font`: New font
```cpp
void SetFont(const std::shared_ptr<Font>& font)
```

---
### GetMaterial
Get the material of the sprite.
```cpp
const std::shared_ptr<Material>& GetMaterial()
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
### GetFontSize
Get the font size.
```cpp
float GetFontSize() const
```

---
### SetFontSize
Set the font size.

Parameters:
- `fontSize`: New font size
```cpp
void SetFontSize(float fontSize)
```

---
### GetLineSpacing
Get the line spacing.
```cpp
float GetLineSpacing() const
```

---
### SetLineSpacing
Set the line spacing.

Parameters:
- `lineSpacing`: New line spacing
```cpp
void SetLineSpacing(float lineSpacing)
```

---
### GetCharacterSpacing
Get the character spacing.
```cpp
float GetCharacterSpacing() const
```

---
### SetCharacterSpacing
Set the character spacing.

Parameters:
- `characterSpacing`: New character spacing
```cpp
void SetCharacterSpacing(float characterSpacing)
```

---
### GetVerticalAlignment
Get the vertical alignment.
```cpp
VerticalAlignment GetVerticalAlignment() const
```

---
### SetVerticalAlignment
Set the vertical alignment.

Parameters:
- `verticalAlignment`: New vertical alignment
```cpp
void SetVerticalAlignment(VerticalAlignment verticalAlignment)
```

---
### GetHorizontalAlignment
Get the horizontal alignment.
```cpp
HorizontalAlignment GetHorizontalAlignment() const
```

---
### SetHorizontalAlignment
Set the horizontal alignment.

Parameters:
- `horizontalAlignment`: New horizontal alignment
```cpp
void SetHorizontalAlignment(HorizontalAlignment horizontalAlignment)
```