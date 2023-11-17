# Color

> [!NOTE]
> `r` is Red<br>
> `b` is Blue<br>
> `g` is Green<br>
> `a` is Alpha<br>
> When using ints, values are between [0 and 255]<br>
> When using floats, values are between [0.0 and 1.0]<br>

## Methods
| Definition | Description |
|-|-|
static void CreateFromRGB(int r, int g, int b) | Set color from ints
static Color CreateFromRGBFloat(float r, float g, float b) | Set color from floats 
static Color CreateFromRGBA(int r, int g, int b, int a) | Set color with alpha information from ints
static Color CreateFromRGBAFloat(float r, float g, float b, float a) | Set color with alpha information from floats 
void SetFromRGBA(int r, int g, int b, int a) | Set color with alpha information from ints
void SetFromRGBAfloat(float r, float g, float b, float a) | Set color with alpha information from floats 
RGBA GetRGBA() | Get RGBA
unsigned int GetUnsignedIntRGBA() | Get RGBA value as an unsigned int
unsigned int GetUnsignedIntABGR() | Get ABGR value as an unsigned int
