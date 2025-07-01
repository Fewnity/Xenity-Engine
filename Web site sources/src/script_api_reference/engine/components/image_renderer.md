# ImageRenderer

Add this in your code:
```cpp
#include <engine/graphics/ui/image_renderer.h>
```

## Description

Component that renders an image on the screen.

## Variables

---
| Name | Type | Description |
|-|-|-|
color | `Color` | Color of the image

## Public methods

---
### GetImage
Get the image.
```cpp
const std::shared_ptr<Texture>& GetImage() const
```

---
### SetImage
Set the image.

Parameters:
- `image`: Image to use
```cpp
void SetImage(const std::shared_ptr<Texture>& image)
```