# Button

Add this in your code:
```cpp
#include <engine/graphics/ui/button.h>
```

## Description

Component that renders a button on the screen.

## Variables

---
| Name | Type | Description |
|-|-|-|
color | `Color` | Color of the sprite

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

---
### GetOnClickEvent
Get the onClick event, called when the user clicks on the button.
```cpp
Event<>& GetOnClickEvent()
```
Code sample:
```cpp
class MyClass
{
public:
    void OnClick() { }
};

std::shared_ptr<Button> button; // Filled variable
MyClass myClassInstance = MyClass();

button->GetOnClickEvent().Bind(&MyClass::MyStaticFunction, &myClassInstance);
```