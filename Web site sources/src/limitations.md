# Limitations

Here is a list of every limitations of the game engine. These can be limitations for the user or for the game engine developpers.

## For users

### Editor UI

Multi selection is not supported.

### In game UI

There is no image and button component.

### Lighting

Lighting does not work great on PsVita and PS3. A shader variant system could fix this.
 - Performances are not great,
 - Lights on PS3 are limited by 1 on each type.

Objects with a big scale will cause lags if they are moving (Problem caused the light world partitionner).

### Audio

Audio works "fine" with 44100Hz but on PS3 you need to use 48000Hz audio or the audio will sound weird.

### Rendering

The engine is mono thread, so performances are not great with lot of objects.

There is not split screen support.

### 2D

2D performances are not great.

### Networking

Network sockets get data only every frame. (Limitation easy to fix).

There is no way to know if a socket has been closed or not.

### Reflection

The reflection does not support many types.

### Physics

There is no layer system. So no system to disable collisions between some colliders.

There is no 2D physics system.

## For Xenity maintainers

### Rendering

The graphics API interface is OpenGL like, so it's harder to add an new API that is different from OpenGL.

On PSP there is a graphical bug and I'm not able to fix it, looks like it visible when disabling vsync but not sure.

### Reflection

The reflection is a little bit hard to maintain.

### Other

The overuse of "friend class" is a problem in the engine in general. It's used to hide internal functions to the user.