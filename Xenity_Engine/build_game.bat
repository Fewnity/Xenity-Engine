@echo off
cls
call C:\VisualStudio\VC\Auxiliary\Build\vcvarsamd64_x86.bat

timeout /t 4 /nobreak > nul

echo [32m[0m
echo [32m--------------------------[0m
echo [32mCompiling game lib     ...[0m
echo [32m--------------------------[0m
echo [32m[0m

cl /std:c++20 /MP /EHsc /DIMPORT /DEDITOR -I "include" /LD Source/game_dungeon/*.cpp engine_editor.lib /link /out:game_editor.dll /implib:game_editor.lib

pause>nul