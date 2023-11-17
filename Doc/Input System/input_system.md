# Input System

Read inputs.

## Methods
| Definition | Description |
|-|-|
static bool GetKeyDown(const KeyCode keyCode) | Return true if the key has just been pressed
static bool GetKey(const KeyCode keyCode) | Return true if the key is held
static bool GetKeyUp(const KeyCode keyCode) | Return true if the key has just been released
static int GetTouchScreenCount() | Get how many touch screens the device has
static int GetTouchCount(const int screenIndex) | Get how many touch inputs the screen has
static Touch GetTouch(const int touchIndex, const int screenIndex) | Get touch data
static void HideMouse() | Hide mouse
static void ShowMouse() | Show mouse

## Members
| Name | Type | Description |
|-|-|-|
leftJoystick | Vector2 | Value of the left joystick (normalised)
rightJoystick | Vector2 | Value of the right joystick (normalised)
mousePosition | Vector2 | Mouse position
mouseSpeed | Vector2 | Mouse speed
mouseSpeedRaw | Vector2 | Raw mouse speed (in pixel)
mouseWheel | float | Mouse wheel speed

