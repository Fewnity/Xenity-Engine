# Texture

Add this in your code:
```cpp
#include <engine/graphics/texture/texture.h>
```

## Description

Asset that stores an image.

A texture can be created dynamically.

## General settings

---
| Name | Type | Description |
|-|-|-|
Resolution | Enum | Max texture solution
Filter | Enum | Texture filter
WrapMode | Enum | Texture wrap mode
UseMipMap | CheckBox | If enabled, smaller versions of the texture will be generated. Improves rendering but consumes more memory (Not working on PS3).
PixelPerUnit | Number | Number of pixel per world unit (Used for sprites).

## PSP additional settings

---
| Name | Type | Description |
|-|-|-|
Type | Enum | Type of compression
TryPutInVram | CheckBox | If true, the texture will try to be stored in video ram instead of ram. (video ram is faster than ram but way smaller)

## PS3 additional settings

---
| Name | Type | Description |
|-|-|-|
Type | Enum | Type of compression

## Static methods

---
### CreateTexture
Dynamically create a texture.

Parameters:
- `params`: Parameters for the texture creation
```cpp
std::shared_ptr<Texture> CreateTexture(const TextureConstructorParams& params)
```
Code sample:
```cpp
TextureConstructorParams params;
params.width = 128;
params.height = 128;
params.filter = Filter::Bilinear;
params.wrapMode = WrapMode::Repeat;
params.pspTextureType = PSPTextureType::RGBA_4444;
params.ps3TextureType = PS3TextureType::ARGB_4444;
params.hasAlpha = true;

std::shared_ptr<Texture> newTexture = Texture::CreateTexture(params);
```

## Public methods

---
### SetFilter
Set texture filter.

Parameters:
- `filter`: Filtering mode
```cpp
void SetFilter(const Filter filter)
```
Code sample:
```cpp
std::shared_ptr<Texture> texture; // Filled variable
texture->SetFilter(Filter::Point);
texture->SetFilter(Filter::Bilinear);
```

---
### SetWrapMode
Set texture wrap mode.

Parameters:
- `mode`: Wraping mode
```cpp
void SetWrapMode(const WrapMode mode)
```
Code sample:
```cpp
std::shared_ptr<Texture> texture; // Filled variable
texture->SetWrapMode(WrapMode::Point);
texture->SetWrapMode(WrapMode::Bilinear);
```

---
### GetWidth
Get texture width.
```cpp
int GetWidth() const
```
Code sample:
```cpp
std::shared_ptr<Texture> texture; // Filled variable
int width = texture->GetWidth();
```

---
### GetHeight
Get texture height.
```cpp
int GetHeight() const
```
Code sample:
```cpp
std::shared_ptr<Texture> texture; // Filled variable
int height = texture->GetHeight();
```

---
### SetPixelPerUnit
Set texture pixel per unit.
```cpp
void SetPixelPerUnit(int value)
```
Code sample:
```cpp
std::shared_ptr<Texture> texture; // Filled variable
texture->SetPixelPerUnit(100);
```

---
### GetPixelPerUnit
Get texture pixel per unit.
```cpp
int GetPixelPerUnit() const
```
Code sample:
```cpp
std::shared_ptr<Texture> texture; // Filled variable
int pixelPerUnit = texture->GetPixelPerUnit();
```

---
### GetUseMipmap
Get if the texture is using mipmap.
```cpp
bool GetUseMipmap() const
```
Code sample:
```cpp
std::shared_ptr<Texture> texture; // Filled variable
bool useMipMap = texture->GetUseMipmap();
```

---
### GetWrapMode
Get texture wrap mode.
```cpp
WrapMode GetWrapMode() const
```
Code sample:
```cpp
std::shared_ptr<Texture> texture; // Filled variable
WrapMode wrapMode = texture->GetWrapMode();
```

---
### SetData
Set texture data.<br>
Can be called multiple times.
```cpp
virtual void SetData(const unsigned char* data) = 0
```
Code sample:
```cpp
size_t width = 128;
size_t height = 128;
unsigned char* rawData = new unsigned char[width * height * 4];

struct Pixel
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

// Create a noise texture
for (size_t y = 0; y < height; y++)
{
    for (size_t x = 0; x < width; x++)
    {
        Pixel& pixel = reinterpret_cast<Pixel*>(rawData)[x + (y * width)];
        pixel.r = rand() % 255;
        pixel.g = rand() % 255;
        pixel.b = rand() % 255;
        pixel.a = 255;
    }
}

TextureConstructorParams params;
params.width = width;
params.height = height;
params.filter = Filter::Bilinear;
params.wrapMode = WrapMode::Repeat;
params.pspTextureType = PSPTextureType::RGBA_4444;
params.ps3TextureType = PS3TextureType::ARGB_4444;
params.hasAlpha = true;

std::shared_ptr<Material> material; // Filled variable
std::shared_ptr<Texture> newTexture = Texture::CreateTexture(params);
newTexture->SetData(rawData);
material->SetTexture(newTexture);

delete[] rawData;
```