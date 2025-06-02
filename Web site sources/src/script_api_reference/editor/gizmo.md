# Gizmo

Add this in your code:
```cpp
#include <editor/rendering/gizmo.h>
```

## Description

This class is used to draw debug shapes or sprites.

Gizmos can be renderer in `OnDrawGizmos` and `OnDrawGizmosSelected` events.<br>
> [!WARNING]
> Do not create/delete/modify GameObjects or Components in these functions.<br>
> Do not call Gizmo functions outsize the events.

### How to use Gizmos events:

Override functions you want to use:
```cpp
class MyComponent : public Component
{
public:
    ...
private:
    void OnDrawGizmos() override;
    void OnDrawGizmosSelected() override;
}
```
Then implement the functions:

```cpp
void MyComponent::OnDrawGizmos()
{
#if defined(EDITOR)
    Gizmo::DrawLine(Vector3(0, 0, 0), Vector3(1, 1, 1));
#endif
}

void MyComponent::OnDrawGizmosSelected()
{
#if defined(EDITOR)
    Gizmo::DrawLine(Vector3(1, 1, 1), Vector3(0, 0, 0));
#endif
}
```

The use of `#if defined(EDITOR)` is needed because the `Gizmo` class is not visible in game builds.

## Static methods

---
### SetColor 
Set Gizmo draw color (does not affect sprites).

Parameters:
- `newColor`: New color to use
```cpp
void SetColor(const Color& newColor)
```
Code sample:
```cpp
const Color redColor = Color::CreateFromRGBAFloat(1, 0, 0, 1);
Gizmo::SetColor(redColor);
```

---
### DrawLine 
Draw a simple line from A to B.

Parameters:
- `a`: Start point
- `b`: End point
```cpp
void DrawLine(const Vector3& a, const Vector3& b)
```
Code sample:
```cpp
Gizmo::DrawLine(Vector3(0, 0, 0), Vector3(1, 1, 1));
```

---
### DrawBillboard 
Draw a billboard sprite.

Parameters:
- `position`: Position of the sprite
- `scale`: Scale of the sprite
- `texture`: Texture to draw
- `color`: Color of the sprite
```cpp
void DrawBillboard(
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
    Color::CreateFromRGBAFloat(1, 1, 1, 0.5f));
```

---
### DrawSphere 
Draw a wired sphere.

Parameters:
- `position`: Position of the sphere
- `rotation`: Rotation of the sphere
- `radius`: Radius of the sphere
```cpp
void DrawSphere(
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