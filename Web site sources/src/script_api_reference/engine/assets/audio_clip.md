# AudioClip

Add this in your code:
```cpp
#include <engine/audio/audio_clip.h>
```

## Description

Asset that stores audio like a sound effect or a music.

Used by [Audio Source](../components/audio_source.md).

## Inspector settings

---
| Name | Type | Description |
|-|-|-|
Loaded In Memory | CheckBox | Defines if the audio should be fully loaded in memory.<br>When enabled, it's faster to play the audio but consumes a lot of memory.<br>Recommended for sound effects but not for musics.