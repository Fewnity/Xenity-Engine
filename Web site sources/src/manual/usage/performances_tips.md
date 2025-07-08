# Performances tips

Performances on Xenity can be affected for many reasons.

A great way to analyze performances is to use the integrated profiler in the editor.

## Physics

On small systems like the PlayStation Portable, performances can be very low with too many rigidbodies. Try to reduce the amount of activated rigidbody in the scene.

## Audio

Streaming is very slow on game console, try to avoid having more than one audio source using an audio clip from the storage memory. Only musics should be streamed from the storage memory. Short audio should be in RAM.
See the `Loaded In Memory` option in [Audio Clip](../../script_api_reference/engine/assets/audio_clip.md)
The `Loaded In Memory` option consumes a large amount of memory, that why you should not enable this option for every audio clips.

## Rendering

Lighting is extremely heavy on Xenity, try to reduce as much as possible the point/spot light count in you scene. Try to use smaller range for your lights.

Transparency is heavy on PS3 and PsVita, try to avoid them by using Cutout on materials instead.