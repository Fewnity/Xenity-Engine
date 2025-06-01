# Preprocessors

A list of preprocessors can be used in your code.
A preprocessor can be used to change your code at compile time.

Sample:
```cpp
void MyFunction()
{
#if defined(EDITOR)
    Debug::Print("Running in the editor!");
#else

#if defined(__PSP__)
    Debug::Print("Running on PSP!");
#elif defined(__vita__)
    Debug::Print("Running on PSP!");
#else
    Debug::Print("Running on something else!");
#endif

#endif
}
```

## Platforms

- `__PSP__` is defined when compiling for PlayStation Portable
- `__vita__` is defined when compiling for PlayStation Vita
- `__PS3__` is defined when compiling for PlayStation 3
- `_WIN32` & `_WIN64` is defined when compiling for Windows
- `__LINUX__` is defined when compiling for Linux

## Editor

- `EDITOR` is defined when compiling to play in the editor

## Debug

- `DEBUG` is defined when compiling in debug mode
- `USE_PROFILER` is defined when compiling with the profiler enabled