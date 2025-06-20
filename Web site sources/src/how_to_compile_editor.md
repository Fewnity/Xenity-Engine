# How to compile the editor

- Install [Visual Studio 2022](https://visualstudio.microsoft.com/),
- Clone the project
- Open Xenity_Engine.vcxproj
- Set the project `Xenity_Engine_Executable` as startup project,
- Select the `Editor` configuration,
- Compile.

The engine configuration is used to make game builds from the editor itself.

All dependencies binaries are already in the repository.

Generated files are located in the folder XenityBuild.

## How to make a shipping build

- Create a new folder
- build both `Engine` and `Editor` configurations,
- Copy `Xenity_Engine_Executable.exe` (from the `Editor` configuration), `Xenity_Engine.dll`, `Xenity_Engine.lib`, `Xenity_Editor.dll`, `Xenity_Editor.lib` into the new folder,
- Rename `Xenity_Engine_Executable.exe` to `Xenity_Engine.exe`
- Copy `freetype.dll`, `SDL3.dll`, `assimp-vc143-mt.dll` into the new folder,
- Copy `Makefile.PS3`, `CMakeLists.txt`, `compile_shaders.sh`, `Dockerfile`, into the new folder,
- Copy `logo.ico`, `res.rc`, into the new folder,
- Copy `Roboto Regular.ttf`, into the new folder,
- Copy `public_engine_assets`, `engine_assets`, `vscodeSample`, `psvita_images`, `psp_images`, `icons`, into the new folder,
- Copy `Source`, `include`, `vscodeSample`, `psvita_images`, `psp_images`, `icons`, into the new folder,
- Copy `main.cpp`, into the new folder

Done!