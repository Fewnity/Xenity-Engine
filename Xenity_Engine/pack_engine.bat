@echo off

mkdir "Xenity Editor"

Rem ------ Copy folders
xcopy "shaders" "Xenity Editor\shaders" /i /e /Y
xcopy "engine_assets" "Xenity Editor\engine_assets" /i /e /Y
xcopy "materials" "Xenity Editor\materials" /i /e /Y
xcopy "icons" "Xenity Editor\icons" /i /e /Y

Rem ------ Copy Executable
xcopy "editor.exe" "Xenity Editor\" /Y

Rem ------ Copy Engine DLLs
xcopy "engine_editor.dll" "Xenity Editor\" /Y
xcopy "engine_game.dll" "Xenity Editor\" /Y

Rem ------ Copy other libraries DLLs
xcopy "glfw3.dll" "Xenity Editor\" /Y
xcopy "freetype.dll" "Xenity Editor\" /Y
xcopy "SDL2.dll" "Xenity Editor\" /Y

Rem ------ Copy font
xcopy "Roboto Regular.ttf" "Xenity Editor\" /Y