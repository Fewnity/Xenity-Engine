# Profiler

Add this in your code:
```cpp
#include <engine/debug/profiler.h>
```

## Description

Class used to use the profiler in game builds.

## Public methods

---
### GetReflectiveData
Save profiling data to a file at the end of the frame.<br>
Return true if the data can be saved, false if the profiler is disabled.

Parameters:
- `name`: Name of the profiling data file without the extension, if empty, the name will be "profiler" + the date and time
```cpp
static bool SaveProfilingData(const std::string& name = "")
```
Code sample:
```cpp
Profiler::SaveProfilingData(); // Will be saved in "profiler 14h 37m 42s 9d 7m 2025y.xenp"
Profiler::SaveProfilingData("scene_0"); // Will be saved in "scene_0.xenp"
```