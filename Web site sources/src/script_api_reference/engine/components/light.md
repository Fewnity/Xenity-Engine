# Light

Add this in your code:
```cpp
#include <engine/lighting/lighting.h>
```

## Description

Component to produce light

## Variables

---
| Name | Type | Description |
|-|-|-|
color | Color | Color of the light

## Public methods

---
### SetupPointLight
Setup the light as a point light.

Parameters:
- `color`: Light color
- `intensity`: Light intensity
- `range`: Light Range (Greater or equals to 0)
```cpp
void SetupPointLight(const Color& color, float intensity, const float range)
```

---
### SetupDirectionalLight
Setup the light as a directional light.

Parameters:
- `color`: Light color
- `intensity`: Light intensity
```cpp
void SetupDirectionalLight(const Color& color, float intensity)
```

---
### SetupAmbientLight
Setup the light as an ambient light.

Parameters:
- `color`: Light color
- `intensity`: Light intensity
```cpp
void SetupAmbientLight(const Color& color, const float intensity)
```

---
### SetupSpotLight
Setup the light as a spot light.

Parameters:
- `color`: Light color
- `intensity`: Light intensity
- `range`: Light Range (Greater or equals to 0)
- `angle`: Spot angle [0;90]
```cpp
void SetupSpotLight(const Color& color, const float intensity, const float range, const float angle)
```

---
### SetupSpotLight
Setup the light as a spot light.

Parameters:
- `color`: Light color
- `intensity`: Light intensity
- `range`: Light Range (Greater or equals to 0)
- `angle`: Spot angle [0;90]
- `smoothness`: Spot smoothness [0;1]
```cpp
void SetupSpotLight(const Color& color, const float intensity, const float range, const float angle, const float smoothness)
```

---
### SetupSpotLight
Set light range.

Parameters:
- `value`: Light range (Greater or equals to 0)
```cpp
void SetRange(float value)
```

---
### SetSpotAngle
Set spot angle.

Parameters:
- `value`: Spot angle [0;90]
```cpp
void SetSpotAngle(float angle)
```

---
### SetSpotSmoothness
Set spot smoothness.

Parameters:
- `value`: Spot smoothness [0;1]
```cpp
void SetSpotSmoothness(float smoothness)
```

---
### GetType
Get light type.
```cpp
LightType GetType() const
```

---
### GetType
Set light type.

Parameters:
- `type`: Light type
```cpp
void SetType(LightType type)
```

---
### GetIntensity
Get light intensity.
```cpp
float GetIntensity() const
```

---
### SetIntensity
Set light intensity.

Parameters:
- `intensity`: Light intensity
```cpp
void SetIntensity(float intensity)
```

---
### GetOldConsoleCompatibility
Get if the light should simulate lighting from old consoles (To get the same behavior as PSP)y.
```cpp
bool GetOldConsoleCompatibility() const
```

---
### SetOldConsoleCompatibility
Set if the light should simulate lighting from old consoles (To get the same behavior as PSP).

Parameters:
- `value`: New value
```cpp
void SetOldConsoleCompatibility(bool value)
```