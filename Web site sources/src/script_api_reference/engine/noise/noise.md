# Noise

Add this in your code:
```cpp
#include <engine/noise/noise.h>
```

## Description

Class to generate 1D, 2D, 3D and 4D Perlin noise

## Static methods

---
### Noise1D
Get 1D Perlin noise

Parameters:
- `x`:  The x coordinate
```cpp
float Noise1D(float x)
```
Code sample:
```cpp
float value = Noise::Noise1D(0.5f);
```

---
### Noise2D
Get 2D Perlin noise

Parameters:
- `x`:  The x coordinate
- `y`:  The y coordinate
```cpp
float Noise2D(float x, float y)
```
Code sample:
```cpp
float value = Noise::Noise2D(0.5f, 0.1f);
```

---
### Noise3D
Get 3D Perlin noise

Parameters:
- `x`:  The x coordinate
- `y`:  The y coordinate
- `z`:  The z coordinate
```cpp
float Noise3D(float x, float y, float z)
```
Code sample:
```cpp
float value = Noise::Noise3D(0.5f, 0.1f, 0.3f);
```

---
### Noise4D
Get 4D Perlin noise

Parameters:
- `x`:  The x coordinate
- `y`:  The y coordinate
- `z`:  The z coordinate
- `w`:  The w coordinate
```cpp
float Noise4D(float x, float y, float z, float w)
```
Code sample:
```cpp
float value = Noise::Noise4D(0.5f, 0.1f, 0.3f, 0.8f);
```