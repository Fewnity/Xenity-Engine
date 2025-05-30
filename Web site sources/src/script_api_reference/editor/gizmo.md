# Gizmo

Add this in your code:
```cpp
#include <editor/rendering/gizmo.h>
```

## Static methods

---
### SetColor 
Set Gizmo draw color (does not affect sprites)
```cpp
SetColor(const Color& newColor)
```
Code sample:
```cpp
const Color redColor = Color::CreateFromRGBAFloat(1, 0, 0, 1);
Gizmo::SetColor(redColor);
```

---
### DrawLine 
Draw a simple line from A to B
```cpp
DrawLine(const Vector3& a, const Vector3& b)
```
Code sample:
```cpp
Gizmo::DrawLine(Vector3(0,0,0), Vector3(1,1,1));
```

---
### DrawBillboard 
Draw a billboard sprite
```cpp
DrawBillboard(
    const Vector3& position, 
    const Vector2& scale, 
    const std::shared_ptr<Texture>& texture, 
    const Color& color)
```
Code sample:
```cpp
Gizmo::DrawBillboard(
    GetTransform()->GetPosition(),
    Vector2(0.2f),
    EditorIcons::GetIcon(IconName::Icon_Audio_Source),
    Color::CreateFromRGBAFloat(1, 1, 1, 1));
```

---
### DrawSphere 
Draw a wired sphere
```cpp
DrawSphere(
    const Vector3& position, 
    const Quaternion& rotation, 
    const float radius)
```
Code sample:
```cpp
Gizmo::DrawSphere(
    GetTransform()->GetPosition(), 
    GetTransform()->GetRotation(), 
    2.0f);
```