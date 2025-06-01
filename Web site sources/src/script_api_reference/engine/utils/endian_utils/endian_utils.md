# EndianUtils

Add this in your code:
```cpp
#include <engine/tools/endian_utils.h>
```

## Description

Utility class for endian conversion.<br>
Usefull when using binary data on different platforms.

## Static methods

---
### IsBigEndian
Checks if the system is big-endian.
For examples PC use little-endian, but PlayStation 3 use big-endian.
```cpp
bool IsBigEndian()
```
Code sample:
```cpp
// True on PlayStation 3
// False on most PC, PSP and PsVita
bool isBigEndian = EndianUtils::IsBigEndian();
```

---
### SwapEndian
Converts an type from little-endian to big-endian or vice versa.<br>
Only use simple types like int32_t, uint16_t, etc. Do not use this for complex types like structs or classes.

Parameters:
- `value`: Value to swap endian to
```cpp
template <typename T>
T SwapEndian(T value)
```
Code sample:
```cpp
uint32_t value = 123;
uint32_t newValue = EndianUtils::SwapEndian(value); // = 2063597568
```