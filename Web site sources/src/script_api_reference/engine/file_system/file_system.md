# FileSystem

Add this in your code:
```cpp
#include <engine/file_system/file_system.h>
```

## Description

This class provides methods to manipulate files and directories.

## Static methods

---
### CreateFolder
Create a directory.<br>
Create parents directories if they do not exist.
Returns true on success.

Parameters:
- `path`: Path of the folder to create
```cpp
bool CreateFolder(const std::string& path)
```
Code sample:
```cpp
FileSystem::CreateFolder("C:/my_folder/");
```

---
### Delete
Delete a file or a directory.<br>

Parameters:
- `path`: Path of file or folder
```cpp
void Delete(const std::string& path)
```
Code sample:
```cpp
FileSystem::Delete("C:/my_folder/my_file.txt");
FileSystem::Delete("C:/my_folder/");
```

---
### Rename
Rename a file or a directory (Not currently supported on PSP and PS3).<br>
Returns true on success.

Parameters:
- `path`: File path to rename
- `newPath`: New file path (should be in the same directory)
```cpp
bool Rename(const std::string& path, const std::string& newPath)
```
Code sample:
```cpp
FileSystem::Rename(
    "C:/my_folder/my_file.txt", 
    "C:/my_folder/my_renamed_file.txt");
```

---
### CopyFile
Copy a file to a new path (Not currently supported on PS3).

Parameters:
- `path`: File path to copy
- `newPath`: File destination path
- `replace`: If true, replace the file if it already exists
```cpp
CopyFileResult CopyFile(const std::string& path, const std::string& newPath, bool replace)
```
Code sample:
```cpp
CopyFileResult result = FileSystem::CopyFile(
    "C:/my_folder/my_file.txt", 
    "C:/my_folder/my_copied_file.txt"
    false); // Result is Success

// Do it again, this time the result will be "FileAlreadyExists"
CopyFileResult result = FileSystem::CopyFile(
    "C:/my_folder/my_file.txt", 
    "C:/my_folder/my_copied_file.txt"
    false);
```

---
### MakeFile
Create a File object from a path (Do not create the file on the system).

Parameters:
- `path`: File path (Relative or absolute)
```cpp
std::shared_ptr<File> MakeFile(const std::string& path)
```
Code sample:
```cpp
std::shared_ptr<File> file = FileSystem::MakeFile("C:/my_folder/my_file.txt");

if(file->Open(FileMode::WriteCreateFile))
{
    // File created thanks to the WriteCreateFile option 
}
```

---
### MakeDirectory
Create a Directory object from a path (Do not create the directory on the system).

Parameters:
- `path`: Directory path (Relative or absolute)
```cpp
std::shared_ptr<Directory> MakeDirectory(const std::string& path)
```
Code sample:
```cpp
std::shared_ptr<Directory> directory = FileSystem::MakeDirectory("C:/my_folder/");
```

---
### ConvertWindowsPathToBasicPath
Converts all backslashes ('\\') to forward slashes ('/') in the given path.

Parameters:
- `path`: Path to convert
```cpp
std::string ConvertWindowsPathToBasicPath(const std::string& path)
```
Code sample:
```cpp
Debug::Print(FileSystem::ConvertWindowsPathToBasicPath("C:\\my_folder\\"));
// Prints "C:/my_folder/"
```

---
### ConvertBasicPathToWindowsPath
Converts all forward slashes ('/') to backslashes ('\\') in the given path.

Parameters:
- `path`: Path to convert
```cpp
std::string ConvertBasicPathToWindowsPath(const std::string& path)
```
Code sample:
```cpp
Debug::Print(FileSystem::ConvertBasicPathToWindowsPath("C:/my_folder/"));
// Prints "C:\my_folder\"
```