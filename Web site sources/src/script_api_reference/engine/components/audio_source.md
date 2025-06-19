# AudioSource

Add this in your code:
```cpp
#include <engine/audio/audio_source.h>
```

## Description

Component to play audio clips.

## Public methods

---
### Play
Play audio.
```cpp
void Play()
```

---
### Resume
Resume audio.
```cpp
void Resume()
```

---
### Pause
Pause audio.
```cpp
void Pause()
```

---
### Stop
Stop audio.
```cpp
void Stop()
```

---
### SetVolume
Set volume.

Parameters:
- `volume`: New volume value
```cpp
void SetVolume(float volume)
```

---
### SetPanning
Set panning.

Parameters:
- `panning`: New panning value
```cpp
void SetPanning(float panning)
```

---
### SetPanning
Set is looping.

Parameters:
- `isLooping`: New is looping value
```cpp
void SetLoop(bool isLooping)
```

---
### GetVolume
Get volume.
```cpp
float GetVolume() const
```

---
### GetPanning
Get panning.
```cpp
float GetPanning() const
```

---
### IsPlaying
Get is playing.
```cpp
bool IsPlaying() const
```

---
### IsLooping
Get is looping.
```cpp
bool IsLooping() const
```

---
### GetAudioClip
Get audio clip.
```cpp
const std::shared_ptr<AudioClip>& GetAudioClip()
```

---
### SetAudioClip
Set audio clip.
```cpp
void SetAudioClip(const std::shared_ptr<AudioClip>& audioClip)
```