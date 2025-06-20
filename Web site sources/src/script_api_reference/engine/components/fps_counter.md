# FpsCounter

Add this in your code:
```cpp
#include <engine/tool/fps_counter.h>
```

## Description

Component to display the FPS counter in the game.

## Variables

---
| Name | Type | Description |
|-|-|-|
showAverage | bool | Should should average fps?

## Public methods

---
### GetTextRenderer
Get the text renderer used for displaying the FPS.
```cpp
const std::weak_ptr<TextRenderer>& GetTextRenderer()
```

---
### SetTextRenderer
Set the text renderer to use for displaying the FPS.

Parameters:
- `textRenderer`: True to enable Multisampling
```cpp
void SetTextRenderer(const std::shared_ptr<TextRenderer>& textRenderer)
```