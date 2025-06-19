# Camera

Add this in your code:
```cpp
#include <engine/graphics/camera.h>
```

## Description

Component for rendering the scene.

## Public methods

---
### SetFov
Set field of view (Perspective mode).

Parameters:
- `fov`: Field of view angle (in degrees)
```cpp
void SetFov(const float fov)
```

---
### GetFov
Get field of view in degrees.
```cpp
float GetFov() const
```

---
### SetProjectionSize
Set projection size (Orthographic mode).

Parameters:
- `value`: Projection size
```cpp
void SetProjectionSize(const float value)
```

---
### GetProjectionSize
Get projection size.
```cpp
float GetProjectionSize() const
```

---
### SetNearClippingPlane
Set near clipping plane.

Parameters:
- `value`: Near clipping plane
```cpp
void SetNearClippingPlane(float value)
```

---
### GetNearClippingPlane
Get near clipping plane.
```cpp
float GetNearClippingPlane() const
```

---
### SetFarClippingPlane
Set far clipping plane.

Parameters:
- `value`: far clipping plane
```cpp
void SetFarClippingPlane(float value)
```

---
### GetFarClippingPlane
Get far clipping plane.
```cpp
float GetFarClippingPlane() const
```

---
### ScreenTo2DWorld
Get 2D world position from pixel coordinate.

Parameters:
- `x`: X pixel position
- `y`: Y pixel position
```cpp
Vector2 ScreenTo2DWorld(int x, int y)
```

---
### MouseTo2DWorld
Get 2D world position from mouse's position.
```cpp
ector2 MouseTo2DWorld()
```

---
### SetProjectionType
Set projection type.

Parameters:
- `type`: Projection type
```cpp
void SetProjectionType(const ProjectionType type)
```

---
### GetProjectionType
Get projection type.
```cpp
ProjectionType GetProjectionType() const
```

---
### GetMouseRay
Get mouse normalized ray direction.
```cpp
Vector3 GetMouseRay()
```

---
### GetWidth
Get view width in pixel.
```cpp
int GetWidth() const
```

---
### GetHeight
Get view height in pixel.
```cpp
int GetHeight() const
```

---
### GetAspectRatio
Get view aspect ratio.
```cpp
float GetAspectRatio() const
```

---
### GetUseMultisampling
Get if the camera is using multisampling (Windows Only).
```cpp
bool GetUseMultisampling() const
```

---
### SetUseMultisampling
Set if the camera is using multisampling (Windows Only).

Parameters:
- `useMultisampling`: True to enable Multisampling
```cpp
void SetUseMultisampling(bool useMultisampling)
```

---
### GetRawFrameBuffer
Get a copy of the frame buffer.<br>
Heavy operation, use with caution.
```cpp
std::unique_ptr<uint8_t[]> GetRawFrameBuffer()
```