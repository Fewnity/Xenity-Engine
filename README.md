# Xenity Engine

Xenity Engine is a crossplatform game engine based on GameObjects and Components for PlayStation Portable, PsVita, PS3 and Windows.

[Xenity Documentation](https://github.com/Fewnity/Xenity-Engine/blob/crossplatform/Doc/README.md)

The game engine is a work in progress, so any pull requests are welcome!<br>
New features, refactor, bug fixes, optimizations or documentations everything is good to take!

![image](https://github.com/Fewnity/Xenity-Engine/assets/39272935/df59e12e-b989-4b0d-93b1-4f95fac7c91f)


### Supported platforms:<br>
✔️ Supported<br>
🚧 Work In Progress<br>
❌ Not Supported Yet<br>

| Status | Platform |
|-|-|
✔️ | Windows
✔️ | PlayStation Portable
✔️ | PlayStation Vita
✔️ | PlayStation 3
🚧 | Linux (Missing audio and build system)
❌ | PlayStation 2

Features:

- [X] Basic 2D
- [X] Basic 3D
- [X] 2D Audio
- [ ] 3D Audio
- [X] Skyboxes
- [X] Scenes
- [ ] Animations
- [X] 3D Physics
- [ ] 2D Physics
- [X] C++ Hot Reloading

> [!WARNING]
> The engine is not finished. So you may encounter problems during game development like:
> - Deleting Meta files will have repercussions on your project. Variables that reference the files will be emptied.
> - Duplicating a meta file will cause problems, each meta file has a unique file id in it.

> [!NOTE]
> If you want to compile a game in the editor, you have to change set `Compiler path` in the `Engine Settings` menu (Window->Engine Settings). You can set the `PPSSPP location` for Build And Run for PSP builds.
> `Compiler path` is by default `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build`, change this if your Visual Studio installation/version is different.

## How to use the editor
### How to play the game in the editor
- Compile the game in "Game->Compile Game"
- Click on the play button
### How to build the game
- Open the Build Settings menu in "File->Build Settings"
- Select the platform
- Click on build
(For PSP, PS3 and PsVita, you will need Docker Desktop installed and configured).

## Docker Setup
Docker Desktop is needed to compile PSP, PS3 and PsVita games.<br>
Download Docker Desktop: https://www.docker.com/products/docker-desktop/

Then finish the docker install in Xenity. Go in "Window->Docker Configuration", the process can take several minutes especially if you have a slow computer or a slow internet connection.

## Compatible files
Images: `.png .jpg .bmp`

Audio: `.wav .mp3` (Please use 44100Hz audio except on PS3, you have to use 48000Hz audio)

Mesh: `.obj .fbx`

Font: `.ttf`

## How to compile the engine in Visual Studio
- Set the project `Xenity_Engine_Executable` as startup project.
- Compile `Engine` configuration then `Editor` to generate all dll files.

## How the engine internally works

When a project/game is loaded, the engine will make a list of all existing files. Each file has an unique Id.
Classes that are originally a file like `Texture`, `Audio Clip`, `Mesh Data` and more, inherit from `File Reference` to have a pointer to their file.

Some classes like `Component`, `GameObject`, `Vector3` and more, inherit from `Reflective`. This allow to access their variables without knowing the base class. This is used by the Inspector to see and modify data and used to save/load their data in/from a file.

## Contact
If you need help, you can open an Issue or contact me.

Email: `fewnity@gmail.com`<br>
Discord: `fewnity`

## License

The code of this repository is under the MIT license.

## Used libraries
Stb (used: stb_image 2.23 (later versions do not work on PSP), stb_image_resize 0.97, stb_image_write 1.16): https://github.com/nothings/stb

drmp3 v0.6.40 & drwav v0.13.17: https://github.com/mackron/dr_libs

JSON for Modern C++ v3.11.3: https://github.com/nlohmann/json

GLM (slightly modified for PS2): https://github.com/g-truc/glm

Bullet 2.82 (slightly modified to fix compilation): https://code.google.com/archive/p/bullet/downloads

### Windows specific:
Assimp (79d451b): https://github.com/assimp/assimp/
<br>`cmake CMakeLists.txt -DASSIMP_BUILD_ZLIB=ON -DASSIMP_NO_EXPORT=ON -DASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT=OFF -DASSIMP_BUILD_TESTS=OFF -DASSIMP_BUILD_ASSIMP_VIEW=OFF -DASSIMP_INJECT_DEBUG_POSTFIX=OFF -DASSIMP_INSTALL=OFF -DASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT=OFF -DASSIMP_BUILD_OBJ_IMPORTER=ON -DASSIMP_BUILD_FBX_IMPORTER=ON -DCMAKE_BUILD_TYPE=Release`
`cmake --build . --config Release`

FreeType (v2.13.2): https://github.com/ubawurinna/freetype-windows-binaries/

ImGui Docking branch (1.91.8 WIP): https://github.com/ocornut/imgui

SDL (3.2): https://github.com/libsdl-org/SDL

Glad v2.0.8 (gl 4.6 compatibilty mode): https://gen.glad.sh/

### PsVita specific: 
VitaGL (029ddbe): (Compiled with HAVE_GLSL_SUPPORT=1 flag enabled, NO_DEBUG=1 for release and HAVE_SHARK_LOG=1 LOG_ERRORS=2 for debug) https://github.com/Rinnegatamante/vitaGL
