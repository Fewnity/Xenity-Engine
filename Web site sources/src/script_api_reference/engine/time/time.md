# Time

Add this in your code:
```cpp
#include <engine/time/time.h>
```

## Description

Class to get time information (Delta time, elapsed time).

Values are updated every frame.

## Static methods

---
### GetTime
Get total scaled elapsed time.<br>
Start at 0 when the game starts.

```cpp
float GetTime()
```
Code sample:
```cpp
float timeA = Time::GetTime(); // = 0.0f
Time::SetTimeScale(0.5f);

// 2 seconds later...

float timeB = Time::GetTime(); // = 1.0f
```

---
### GetUnscaledTime
Get total unscaled elapsed time (not affected by time scale).<br>
Start at 0 when the game starts.<br>

```cpp
float GetUnscaledTime()
```
Code sample:
```cpp
float timeA = Time::GetUnscaledTime(); // = 0.0f
Time::SetTimeScale(0.5f);

// 2 seconds later...

float timeB = Time::GetUnscaledTime(); // = 2.0f
```

---
### GetDeltaTime
Get scaled delta time, which is the time elapsed since the last frame * time scale.

```cpp
float GetDeltaTime()
```
Code sample:
```cpp
// Game running at 60 fps with a time scale of 1.0f
float timeA = Time::GetDeltaTime(); // = 0.0166f

// Game running at 60 fps with a time scale of 0.5f
float timeA = Time::GetDeltaTime(); // = 0.0083f
```

---
### GetUnscaledDeltaTime
Get unscaled delta time which is the time elapsed since the last frame (not affected by time scale).

```cpp
float GetUnscaledDeltaTime()
```
Code sample:
```cpp
// Game running at 60 fps with a time scale of 1.0f
float timeA = Time::GetUnscaledDeltaTime(); // = 0.0166f

// Game running at 60 fps with a time scale of 0.5f
float timeA = Time::GetUnscaledDeltaTime(); // = 0.0166f
```

---
### GetTimeScale
Get time scale (Speed of the game).

```cpp
float GetTimeScale()
```
Code sample:
```cpp
Time::SetTimeScale(0.4f);
float timeScale = Time::GetTimeScale(); // = 0.4f
```

---
### SetTimeScale
Get time scale (Speed of the game).

Parameters:
- `timeScale`: Time scale (minium 0)
```cpp
void SetTimeScale(float timeScale)
```
Code sample:
```cpp
Time::SetTimeScale(0.4f);
```