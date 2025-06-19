# Presentation of Xenity Engine

Xenity Engine is a crossplatform game engine based on GameObjects and Components for PlayStation Portable, PsVita, PS3 and Windows. Made in C++ 17.

[GitHub page here!](https://github.com/Fewnity/Xenity-Engine)

![image](images/editor_screenshot.png)
_It's pink but you can change the color!_

### Supported platforms:<br>
✔️ Supported<br>
🚧 Work In Progress<br>

| Status | Platform |
|-|-|
✔️ | Windows
✔️ | PlayStation Portable
✔️ | PlayStation Vita
✔️ | PlayStation 3
🚧 | Linux (The UI is a bit glitchy, missing audio and build system)

### Features:

- [X] Basic 2D Rendering (Not great for 2D games)
- [X] 3D Rendering
- [X] 2D Audio
- [ ] 3D Audio
- [X] Skyboxes
- [X] Scenes
- [ ] Animations
- [X] 3D Physics
- [ ] 2D Physics
- [X] C++ Hot Reloading
- [ ] C++ Plugins (Started but not finished)

> [!WARNING]
> The engine is not perfect. So you may encounter problems during game development like:
> - Deleting Meta files will have repercussions on your project. Variables that reference the files will be emptied.
> - Duplicating a meta file will cause problems, each meta file has a unique file id in it.
> - Audio works "fine" with 44100Hz but on PS3 you need to use 48000Hz audio.

You can see all limitations [here](./limitations.md).

### Contact:

**Discord:** fewnity

**Email:** fewnity@gmail.com

**LinkedIn:** [https://www.linkedin.com/in/gregory-machefer-grimaud/](https://www.linkedin.com/in/gregory-machefer-grimaud/)