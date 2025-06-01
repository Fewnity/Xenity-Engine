# Directory

Add this in your code:
```cpp
#include <engine/file_system/directory.h>
```

## Description

Class to manage a directory (Get all files of a folder)

## Variables

---
| Name | Type | Description |
|-|-|-|
subdirectories | `std::vector<std::shared_ptr<Directory>>` | List of sub directories (filled with GetAllFiles)
files | `std::vector<std::shared_ptr<File>>` | List of files (filled with GetAllFiles)

## Public methods

---
### GetAllFiles
Get all the files of the directory (can be very slow and not currently supported on PS3).<br>

Parameters:
- `recursive`: If true, get all the files of the subdirectories
```cpp
std::vector<std::shared_ptr<File>> GetAllFiles(bool recursive)
```
Code sample:
```cpp
std::shared_ptr<Directory> directory = FileSystem::MakeDirectory("C:/my_folder/");

std::vector<std::shared_ptr<File>> files = directory->GetAllFiles(true);
```

---
### CheckIfExist
Check if the directory exists.
```cpp
bool CheckIfExist() const
```
Code sample:
```cpp
std::shared_ptr<Directory> directory = FileSystem::MakeDirectory("C:/my_folder/");

bool directoryExists = directory->CheckIfExist();
```

---
### GetPath
Get directory path.
```cpp
const std::string& GetPath() const
```
Code sample:
```cpp
std::shared_ptr<Directory> directory = FileSystem::MakeDirectory("C:/my_folder/");

const std::string& path = directory->GetPath(); // = "C:/my_folder/"
```