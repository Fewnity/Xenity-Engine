# Color

Add this in your code:
```cpp
#include <engine/graphics/color/color.h>
```

## Description

This class stores informations about a color.

## Static methods

---
### CreateFromRGB
Create a color from 3 uint8_t (alpha = 255).

Parameters:
- `r`: Red level [0;255]
- `g`: Green level [0;255]
- `b`: Blue level [0;255]
```cpp
Color CreateFromRGB(uint8_t r, uint8_t g, uint8_t b)
```
Code sample:
```cpp
Color red = Color::CreateFromRGB(255, 0, 0);
```

---
### CreateFromRGBA
Create a color from 4 uint8_t.

Parameters:
- `r`: Red level [0;255]
- `g`: Green level [0;255]
- `b`: Blue level [0;255]
- `a`: Alpha level [0;255]
```cpp
Color CreateFromRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
```
Code sample:
```cpp
Color semiTransparentRed = Color::CreateFromRGBA(255, 0, 0, 128);
```

---
### CreateFromRGBFloat
Create a color from 3 floats (alpha = 1).

Parameters:
- `r`: Red level [0.0f;1.0f]
- `g`: Green level [0.0f;1.0f]
- `b`: Blue level [0.0f;1.0f]
```cpp
Color CreateFromRGBFloat(float r, float g, float b)
```
Code sample:
```cpp
Color red = Color::CreateFromRGBFloat(1.0f, 0.0f, 0.0f);
```

---
### CreateFromRGBAFloat
Create a color from 4 floats.

Parameters:
- `r`: Red level [0.0f;1.0f]
- `g`: Green level [0.0f;1.0f]
- `b`: Blue level [0.0f;1.0f]
- `a`: Alpha level [0.0f;1.0f]
```cpp
Color CreateFromRGBAFloat(float r, float g, float b, float a)
```
Code sample:
```cpp
Color semiTransparentRed = Color::CreateFromRGBAFloat(1.0f, 0.0f, 0.0f, 0.5f);
```

## Public methods

---
### SetFromRGBA
Set color from 4 uint8_t.

Parameters:
- `r`: Red level [0;255]
- `g`: Green level [0;255]
- `b`: Blue level [0;255]
- `a`: Alpha level [0;255]
```cpp
Color SetFromRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
```
Code sample:
```cpp
Color myColor = Color();
myColor.SetFromRGBA(255, 0, 0, 128);
myColor.SetFromRGBA(255, 255, 255, 255);
```

---
### SetFromRGBAFloat
Set color from 4 float.

Parameters:
- `r`: Red level [0.0f;1.0f]
- `g`: Green level [0.0f;1.0f]
- `b`: Blue level [0.0f;1.0f]
- `a`: Alpha level [0.0f;1.0f]
```cpp
Color SetFromRGBAFloat(float r, float g, float b, float a)
```
Code sample:
```cpp
Color myColor = Color();
myColor.SetFromRGBAFloat(1.0f, 0, 0, 0.5f);
myColor.SetFromRGBAFloat(1.0f, 1.0f, 1.0f, 1.0f);
```

---
### GetRGBA
Get RGBA.
```cpp
const RGBA& GetRGBA() const
```
Code sample:
```cpp
Color red = Color::CreateFromRGBFloat(1.0f, 0.0f, 0.0f);
RGBA rgba = red.GetRGBA();
const RGBA& rgbaRef = red.GetRGBA(); // Avoid a copy
```

---
### GetUnsignedIntRGBA
Get RGBA values as an unsigned int in the format RGBA (0xRRGGBBAA).
```cpp
unsigned int GetUnsignedIntRGBA() const
```
Code sample:
```cpp
Color myColor = Color::CreateFromRGBAFloat(1.0f, 0.6f, 0.3f, 0.1f);
unsigned int rgba = myColor.GetUnsignedIntRGBA(); // 0xff994c19
```

---
### GetUnsignedIntABGR
Get RGBA values as an unsigned int in the format ABGR (0xAABBGGRR).
```cpp
unsigned int GetUnsignedIntABGR() const
```
Code sample:
```cpp
Color myColor = Color::CreateFromRGBAFloat(1.0f, 0.6f, 0.3f, 0.1f);
unsigned int abgr = myColor.GetUnsignedIntABGR(); // 0x194c99ff
```

---
### GetUnsignedIntARGB
Get RGBA values as an unsigned int in the format ARGB (0xAARRGGBB).
```cpp
unsigned int GetUnsignedIntARGB() const
```
Code sample:
```cpp
Color myColor = Color::CreateFromRGBAFloat(1.0f, 0.6f, 0.3f, 0.1f);
unsigned int argb = myColor.GetUnsignedIntARGB(); // 0x19ff994c
```

---
### ToString
Return a string representation of the color.
```cpp
std::string ToString() const
```
Code sample:
```cpp
Color myColor = Color::CreateFromRGBAFloat(1.0f, 0.6f, 0.3f, 0.1f);
Debug::Print(myColor.ToString());
// Prints: "{R:1 G:0.6 B:0.3 A:0.1 | RGBA Hex:#ff994c19 | ARGB Hex:#19ff994c | ABGR Hex:#194c99ff}"
```