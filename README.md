# Xenity Engine

Xenity Engine is a crossplatform game engine based on GameObjects and Components.

[Xenity Documentation](https://github.com/Fewnity/Xenity-Engine/blob/crossplatform/Doc/README.md)

### Supported platforms:<br>
✔️ Supported<br>
🚧 Work In Progress<br>
❌ Not Supported Yet<br>

| Status | Platform |
|-|-|
✔️ | Windows
✔️ | PlayStation Portable
✔️ | PlayStation Vita
🚧 | PlayStation 2
❌ | PlayStation 3


Features:

- [X] Basic 2D
- [X] Basic 3D
- [X] 2D Audio
- [ ] 3D Audio
- [ ] Material (Not finished)
- [X] Skyboxes
- [X] Scenes
- [ ] Animations
- [ ] Physics (Needs to be changed by the Bullet physics engine)
- [X] C++ Hot Reloading

> [!WARNING]
> The engine is far from finished. So there are many problems you may encounter during game development:
> - A project can be corrupted if a bug append during file read/write (maybe caused by the write access is enabled even if we want only to read a file).
  Meta files maybe be blank, so keep a save of your project.
> - If you compile the engine in Visual Studio, Hot Reloading will not be available. The engine will not be able to load DLLs, so the game code should be in Visual Studio project instead of the game project folder.
> - Deleting Meta files will have repercussions on your project. Variables that reference the files will be emptied.
> - Do not duplicate a meta file, each meta file has a unique file id in it.

> [!NOTE]
> If you want to compile a game, you have to change the project path and the compiler path in the Engine Settings menu. You can set the PPSSPP location for Build And Run for PSP builds.

## How to compile without Visual Studio
Run build_engine.bat

A "Xenity Editor" folder will be created with the editor in it.

## How the engine works

When a project/game is loaded, the engine will make a list of all existing files. Each file has an unique ID.

## Compatible files
Images: .png .jpg .bmp

Audio: .wav .mp3

Mesh: .obj (Triangulate)

Font: .ttf

## Used libraries
Stb (used: stb_image 2.23): https://github.com/nothings/stb

### Windows specific: 
FreeType (2.12.1): https://github.com/ubawurinna/freetype-windows-binaries/

ImGui (1.89.5): https://github.com/ocornut/imgui

SDL2 (2.26.2): https://github.com/libsdl-org/SDL

Glad (4.6): https://glad.dav1d.de/ (filled: https://glad.dav1d.de/#profile=core&language=c&specification=gl&loader=on&api=gl%3D4.6)

### PsVita specific: 
VitaGL: (Compiled with HAVE_GLSL_SUPPORT=1 flag enabled) https://github.com/Rinnegatamante/vitaGL
