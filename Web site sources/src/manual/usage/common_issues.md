# Common issues

## Rendering issues

### Black screen

You probably don't have a camera in the scene.

### Disapearing triangles on PSP

The hardware clipping on the PSP is not great. There are two ways to reduce this problem:
- Increase the near plane value of the camera
- Reduce the size of the triangles of affected meshes (generally by increasing triangle count with subdivision)

## Audio issues

If the audio sounds weird on PlayStation 3, make sure to use 48000Hz audio files. But on other platforms you should use 44000Hz audio files.

## Crash

On Windows, if the game crash, you should find a new file next to the game executable called crash_dump.txt containing the call stack of the crash. This will shows you where the crash has happened.

On game consoles, there is no easy way to understand a crash. You can add debug logs in your code to see where logs work and where they stop working. If it's the engine that crashes and not your code, [create an issue on the github page](https://github.com/Fewnity/Xenity-Engine/issues) of the engine to explain how to reproduce the crash. You can send me by email your project to help me finding the bug. You can make a [pull request](https://github.com/Fewnity/Xenity-Engine/pulls) if you can fix the bug yourself.