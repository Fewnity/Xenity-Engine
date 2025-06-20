# Application

Add this in your code:
```cpp
#include <engine/application.h>
```

## Description

Class used to manage the application and get information about it.

## Static methods

---
### OpenURL
Opens the link in a web browser (Only for Windows).
```cpp
void OpenURL(const std::string& url)
```
Code sample:
```cpp
Application::OpenURL("https://google.com");
```

---
### Quit
Quits the application.
```cpp
void Quit()
```
Code sample:
```cpp
Application::Quit();
```

---
### GetPlatform
Returns the current platform the application is running on.
```cpp
Platform GetPlatform()
```
Code sample:
```cpp
if(Application::GetPlatform() == Platform::P_PSP)
{
    Debug::Print("Running on PSP!");
}
```

---
### GetXenityVersion
Returns the current version of Xenity Engine.
```cpp
std::string GetXenityVersion()
```
Code sample:
```cpp
if(Application::GetXenityVersion() == "1.0.0")
{
    // Special behavior for a specific version of the engine
}
else
{
    // ...
}
```

---
### GetGameName
Returns the name of the game.
```cpp
std::string GetGameName()
```
Code sample:
```cpp
Debug::Print("Game name: " + Application::GetGameName());
```

---
### GetCompanyName
Returns the name of the company.
```cpp
std::string GetCompanyName()
```
Code sample:
```cpp
Debug::Print("Company name: " + Application::GetCompanyName());
```

---
### IsInEditor
Returns if the application is running in the editor.
```cpp
bool IsInEditor()
```
Code sample:
```cpp
if(Application::IsInEditor())
{
    Debug::Print("Running in editor!");
}
else
{
    Debug::Print("Not running in editor!");
}

// For something at compile time
#if defined(EDITOR)
    Debug::Print("Running in editor!");
#else
    Debug::Print("Not running in editor!");
#endif
```

---
### GetGameDataFolder
Return the game folder (folder where you can write data).
```cpp
bool GetGameDataFolder()
```
Code sample:
```cpp
std::string fileName = Application::GetGameDataFolder() + "my_file.txt";
std::shared_ptr<File> myFile = FileSystem::MakeFile(fileName);
bool isOpened = myFile->Open(FileMode::WriteCreateFile);
```

---
### GetGameFolder
Returns the game folder (folder where the game is located, may be read-only).
```cpp
bool GetGameFolder()
```
Code sample:
```cpp
std::string fileName = Application::GetGameFolder() + "my_file.txt";
std::shared_ptr<File> myFile = FileSystem::MakeFile(fileName);
bool isOpened = myFile->Open(FileMode::ReadOnly);
```