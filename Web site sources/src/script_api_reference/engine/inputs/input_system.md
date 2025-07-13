# InputSystem

Add this in your code:
```cpp
#include <engine/inputs/input_system.h>
```

## Description

Class to read inputs (GamePads, Touch Screens, Keyboard, Mouse).

## Macros

`PLAYER_1` value: 0<br>
`PLAYER_2` value: 1<br>
`PLAYER_3` value: 2<br>
`PLAYER_4` value: 3<br>
`PLAYER_5` value: 4<br>
`PLAYER_6` value: 5<br>
`PLAYER_7` value: 6<br>
`PLAYER_8` value: 7

## Static variables

---
| Name | Type | Description |
|-|-|-|
mousePosition | `Vector2` | Position of the mouse in pixels
mouseSpeed | `Vector2` | Speed of the mouse (affected by the window resolution)
mouseSpeedRaw | `Vector2` | Raw speed of the mouse
mouseWheel | `float` | Speed of the mouse wheel

## Static methods

---
### GetKeyDown
Return true if the key has just been pressed.

Parameters:
- `keyCode`: Key code to check
- `controllerIndex`: Controller index (0-7)
```cpp
bool GetKeyDown(const KeyCode keyCode, const int controllerIndex = 0)
```
Code sample:
```cpp
if (InputSystem::GetKeyDown(KeyCode::A))
{
    Debug::Print("A key pressed");
}

if (InputSystem::GetKeyDown(KeyCode::CIRCLE), PLAYER_1)
{
    Debug::Print("CIRCLE button pressed");
}
```

---
### GetKey
Return true if the key is held.

Parameters:
- `keyCode`: Key code to check
- `controllerIndex`: Controller index (0-7)
```cpp
bool GetKey(const KeyCode keyCode, const int controllerIndex = 0)
```
Code sample:
```cpp
if (InputSystem::GetKey(KeyCode::A))
{
    Debug::Print("A key held");
}

if (InputSystem::GetKey(KeyCode::CIRCLE), PLAYER_1)
{
    Debug::Print("CIRCLE button held");
}
```

---
### GetKeyUp
Return true if the key has just been released.

Parameters:
- `keyCode`: Key code to check
- `controllerIndex`: Controller index (0-7)
```cpp
bool GetKeyUp(const KeyCode keyCode, const int controllerIndex = 0)
```
Code sample:
```cpp
if (InputSystem::GetKeyUp(KeyCode::A))
{
    Debug::Print("A key released");
}

if (InputSystem::GetKeyUp(KeyCode::CIRCLE), PLAYER_1)
{
    Debug::Print("CIRCLE button released");
}
```

---
### GetLeftJoystick
Get left joystick value, values between -1.0f and 1.0f.

Parameters:
- `controllerIndex`: Controller index (0-7)
```cpp
Vector2 GetLeftJoystick(const int controllerIndex = 0)
```
Code sample:
```cpp
Vector2 left = InputSystem::GetLeftJoystick(PLAYER_1);
```

---
### GetRightJoystick
Get right joystick value, values between -1.0f and 1.0f.

Parameters:
- `controllerIndex`: Controller index (0-7)
```cpp
Vector2 GetRightJoystick(const int controllerIndex = 0)
```
Code sample:
```cpp
Vector2 right = InputSystem::GetRightJoystick(PLAYER_1);
```

---
### GetLeftTrigger
Get left trigger value (R2), values between 0.0f and 1.0f.

Parameters:
- `controllerIndex`: Controller index (0-7)
```cpp
 static float GetLeftTrigger(const int controllerIndex = 0)
```
Code sample:
```cpp
float value = InputSystem::GetLeftTrigger(PLAYER_1);
```

---
### GetRightTrigger
Get right trigger value (R2), values between 0.0f and 1.0f.

Parameters:
- `controllerIndex`: Controller index (0-7)
```cpp
static float GetRightTrigger(const int controllerIndex = 0)
```
Code sample:
```cpp
float value = InputSystem::GetRightTrigger(PLAYER_1);
```

---
### GetTouchScreenCount
Get how many touch screens the device has.
```cpp
int GetTouchScreenCount()
```
Code sample:
```cpp
int touchScreenCount = InputSystem::GetTouchScreenCount(); // = 2 on PsVita
```

---
### GetTouchCount
Get how many touch inputs the screen has.

Parameters:
- `screenIndex`: Screen index
```cpp
int GetTouchCount(const int screenIndex)
```
Code sample:
```cpp
int touchCount = InputSystem::GetTouchCount(0);
```

---
### GetTouch
Get touch data.

Parameters:
- `touchIndex`: Touch index
- `screenIndex`: Screen index
```cpp
Touch GetTouch(const int touchIndex, const int screenIndex)
```
Code sample:
```cpp
int screenIndex = 0;
int touchCount = InputSystem::GetTouchCount(screenIndex);
if (touchCount >= 1)
{
    Touch touch = InputSystem::GetTouch(1, screenIndex);
}
```

---
### HideMouse
Hide mouse.
```cpp
void HideMouse()
```
Code sample:
```cpp
InputSystem::HideMouse();
```

---
### ShowMouse
Show mouse.
```cpp
void ShowMouse()
```
Code sample:
```cpp
InputSystem::ShowMouse();
```