# EditorIcons

Add this in your code:
```cpp
#include <editor/ui/editor_icons.h>
```

## Description

Class used to get icon textures of the editor.<br>
Can be used for Gizmo.

## Static methods

---
### GetIcons
Get the list of all editor icons
```cpp
const std::vector<std::shared_ptr<Texture>>& GetIcons()
```
Code sample:
```cpp
const std::vector<std::shared_ptr<Texture>>& icons = EditorIcons::GetIcons();

const std::shared_ptr<Texture>& cameraIcon = icons[IconName::Icon_Camera];
```

---
### GetIcon
Get an icon by its name enum

Parameters:
- `text`: Text to send
```cpp
const std::vector<std::shared_ptr<Texture>>& GetIcons(IconName iconName)
```
Code sample:
```cpp
const std::shared_ptr<Texture>& cameraIcon = EditorIcons::GetIcon(IconName::Icon_Camera);
```