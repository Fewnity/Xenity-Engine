@echo off
mkdir buildoutput

cls
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsamd64_x86.bat"

echo [32m[0m
echo [32m--------------------------[0m
echo [32mCompiling Engine Game Libary...[0m
echo [32m--------------------------[0m
echo [32m[0m

cl /Fobuildoutput\ /std:c++20 /MP /EHsc /DEXPORT /LD -I "include" "lib_sdl2\x86\SDL2.lib" "lib_sdl2\x86\SDL2main.lib" "lib-vc2022\glfw3dll.lib" "lib-vc2022\freetype.lib" kernel32.lib opengl32.lib user32.lib gdi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib winspool.lib Dwmapi.lib winmm.lib Source/engine/engine.cpp Source/engine/component.cpp Source/engine/engine_settings.cpp Source/engine/monobehaviour.cpp Source/engine/test_component.cpp Source/engine/asset_management/*.cpp Source/engine/audio/*.cpp Source/engine/debug/*.cpp Source/engine/file_system/*.cpp Source/engine/file_system/mesh_loader/*.cpp Source/engine/game_elements/*.cpp Source/engine/graphics/*.cpp Source/engine/graphics/2d_graphics/*.cpp Source/engine/graphics/3d_graphics/*.cpp Source/engine/graphics/color/*.cpp Source/engine/graphics/renderer/*.cpp Source/engine/graphics/ui/*.cpp Source/engine/inputs/*.cpp Source/engine/lighting/*.cpp Source/engine/network/*.cpp Source/engine/noise/*.cpp Source/engine/pathfinding/*.cpp Source/engine/time/*.cpp Source/engine/tools/*.cpp Source/engine/ui/*.cpp Source/engine/vectors/*.cpp Source/*.c Source/windows/*.cpp Source/windows/inputs/*.cpp Source/engine/dynamic_lib/*.cpp Source/engine/class_registry/*.cpp Source/engine/reflection/*.cpp Source/engine/physics/*.cpp Source/engine/unique_id/*.cpp Source/engine/scene_management/*.cpp Source/unit_tests/*.cpp include/imgui/*.cpp /link /out:engine_game.dll /implib:engine_game.lib /NODEFAULTLIB:MSVCRT

pause>nul