# Debug

Each print call will show a message in the console, write the message in the debug file and send to the online console if enabled.

> [!NOTE]
> A debug file "xenity_engine_debug.txt" is automatically created when the engine is starting.

## Methods
| Definition | Description |
|-|-|
static void Print(const std::string& text) | Print a normal message
static void PrintWarning(const std::string& text) | Print a warning
static void PrintError(const std::string& text) | Print an error
