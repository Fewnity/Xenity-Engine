@echo off
cls

set /p copyrelease="Copy release DLLs? N for Debug DLLs: Y/N "

if %copyrelease%==n (
set copyrelease=N
)
if %copyrelease%==y (
set copyrelease=Y
)

Rem ------ Copy Engine DLLs
if %copyrelease%==N (
echo [32mCoping Debug Engine DLLs...[0m
xcopy "x64\Debug Editor\Xenity_Editor.dll" "Xenity_Engine_Executable\" /Y
xcopy "x64\Debug Editor\Xenity_Editor.lib" "Xenity_Engine_Executable\" /Y
xcopy "x64\Debug Engine\Xenity_Engine.dll" "Xenity_Engine_Executable\" /Y
xcopy "x64\Debug Engine\Xenity_Engine.lib" "Xenity_Engine_Executable\" /Y
)else if %copyrelease%==Y (
echo [32mCoping Release Engine DLLs...[0m
xcopy "x64\Release Editor\Xenity_Editor.dll" "Xenity_Engine_Executable\" /Y
xcopy "x64\Release Editor\Xenity_Editor.lib" "Xenity_Engine_Executable\" /Y
xcopy "x64\Release Engine\Xenity_Engine.dll" "Xenity_Engine_Executable\" /Y
xcopy "x64\Release Engine\Xenity_Engine.lib" "Xenity_Engine_Executable\" /Y
)

PAUSE