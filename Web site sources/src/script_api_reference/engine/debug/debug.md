# Debug

Add this in your code:
```cpp
#include <engine/debug/debug.h>
```

## Description

Class used to print text in the console and in file or remotely to a server (Server part is removed for now)

You can find logs in the `xenity_engine_debug.txt` file.<br>
The file will be created in the same folder of the executable file.<br>
For PsVita the file will be created in `ux0:data/xenity_engine/`.

## Static methods

---
### Print
Print a text

Parameters:
- `text`: Text to print
- `hideInEditorConsole`: If true, the text will not be printed in the editor's console
```cpp
void Print(const std::string& text, bool hideInEditorConsole = false)
```
Code sample:
```cpp
Debug::Print("Hello World!"); // Printed in both console and file

Debug::Print("Hello file!", true); // Only printed in the file
```

---
### PrintWarning
Print a warning

Parameters:
- `text`: Text to print
- `hideInEditorConsole`: If true, the text will not be printed in the editor's console
```cpp
void PrintWarning(const std::string& text, bool hideInEditorConsole = false)
```
Code sample:
```cpp
Debug::PrintWarning("I'm a warning.");

// Prints: "[WARNING] I'm a warning."
```

---
### PrintError
Print an error

Parameters:
- `text`: Text to print
- `hideInEditorConsole`: If true, the text will not be printed in the editor's console
```cpp
void PrintError(const std::string& text, bool hideInEditorConsole = false)
```
Code sample:
```cpp
Debug::PrintError("You can't do that!");

// Prints: "[ERROR] You can't do that!"
```

---
### GetOnDebugLogEvent
Get the event when a debug message is printed
```cpp
Event<const std::string&, DebugType>& GetOnDebugLogEvent()
```
Code sample:
```cpp
void OnLog(const std::string& text, DebugType debugType)
{
    // ...
}

Debug::GetOnDebugLogEvent().Bind(&OnLog);
Debug::PrintWarning("I'm a warning");
Debug::GetOnDebugLogEvent().Unbind(&OnLog);
// OnLog will be called with "I'm a warning" and DebugType::Warning as parameters
```