# Texture (File)

Image file.

## Methods
| Definition | Description |
|-|-|
void SetFilter(const Filter filter) | Set texture filter
void SetWrapMode(const WrapMode mode) | Set texture wrap mode
void SetPixelPerUnit(int value) | Set texture pixel per unit
int GetWidth() | Get texture width
int GetHeight() | Get texture height
int GetPixelPerUnit() | Get texture pixel per unit
int GetChannelCount() | Get texture channel count
bool GetUseMipmap() | Get if the texture is using mipmap
Texture::Filter GetFilter() | Get texture filter
Texture::WrapMode GetWrapMode() |  Get texture wrap mode
