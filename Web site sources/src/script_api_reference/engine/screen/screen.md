# Screen

Add this in your code:
```cpp
#include <engine/ui/screen.h>
```

## Description

Class to get informations and to interact with the screen

## Static methods

---
### GetWidth
Get the width of the screen in pixels
```cpp
int GetWidth()
```
Code sample:
```cpp
int width = Screen::GetWidth();
```

---
### GetHeight
Get the height of the screen in pixels
```cpp
int GetHeight()
```
Code sample:
```cpp
int height = Screen::GetHeight();
```

---
### SetFullScreen
Set if the window should be in fullscreen mode
Only for Windows and Linux, no effect on other platforms
Does not affect the editor window

Parameters:
- `useFullScreenMode`: True to enable fullscreen, false to disable
```cpp
void SetFullScreen(bool useFullScreenMode)
```
Code sample:
```cpp
Screen::SetFullScreen(true);
```

---
### SetVSync
Set if the window should use VSync (May cause graphical glitch on PSP)

Parameters:
- `useVSync`: True to enable VSync, false to disable
```cpp
void SetVSync(bool useVSync)
```
Code sample:
```cpp
Screen::SetVSync(true);
```

---
### IsVSyncEnabled
Get if VSync is enabled
```cpp
bool IsVSyncEnabled()
```
Code sample:
```cpp
Screen::SetVSync(true);

bool isEnabled = Screen::IsVSyncEnabled(); // Returns true
```