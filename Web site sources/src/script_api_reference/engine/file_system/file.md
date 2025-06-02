# File

Add this in your code:
```cpp
#include <engine/file_system/file.h>
```

## Description

Class to manage a file (Create, open, read, write).

## Public methods

---
### Open
Open the file.<br>
Return true if the file is opened successfully.

Parameters:
- `fileMode`: The mode to open the file
```cpp
bool Open(FileMode fileMode)
```
Code sample:
```cpp
std::shared_ptr<File> file = FileSystem::MakeFile("C:/my_folder/my_file.txt");

if(file->Open(FileMode::ReadOnly))
{
}
```

---
### CheckIfExist
Check if the file exists.<br>
Return true if the file exists.
```cpp
bool CheckIfExist()
```
Code sample:
```cpp
std::shared_ptr<File> file = FileSystem::MakeFile("C:/my_folder/my_file.txt");

if(file->CheckIfExist())
{
}
```

---
### Close
Close file.

```cpp
bool Close()
```
Code sample:
```cpp
std::shared_ptr<File> file = FileSystem::MakeFile("C:/my_folder/my_file.txt");

if(file->Open(FileMode::ReadOnly))
{
    // ...
    file->Close();
}
```

---
### Write (String)
Write string data to the file.

Parameters:
- `data`: The data to write
```cpp
void Write(const std::string& data)
```
Code sample:
```cpp
std::shared_ptr<File> file = FileSystem::MakeFile("my_file.txt");

if(file->Open(FileMode::WriteCreateFile))
{
    // ...
    file->Write("Hello World!");
}
```

---
### Write (Binary)
Write binary data to the file.

Parameters:
- `data`: The data to write
- `size`: The size of the data in byte
```cpp
void Write(const unsigned char* data, size_t size)
```
Code sample:
```cpp
std::shared_ptr<File> file = FileSystem::MakeFile("my_file.bin");


if(file->Open(FileMode::WriteCreateFile))
{
    uint32_t value = 44;
    // ...
    file->Write(&value, sizeof(uint32_t));
}
```

---
### ReadAll
Read all the content of the file as a string.
```cpp
std::string ReadAll()
```
Code sample:
```cpp
std::shared_ptr<File> file = FileSystem::MakeFile("my_file.txt");

if(file->Open(FileMode::ReadOnly))
{
    std::string content = file->ReadAll();
}
```

---
### ReadAllBinary
Read all the content of the file as a binary (Need to free the pointer after).

Parameters:
- `size`: Output: The size of the binary in byte
```cpp
unsigned char* ReadAllBinary(size_t& size)
```
Code sample:
```cpp
std::shared_ptr<File> file = FileSystem::MakeFile("my_file.bin");

if(file->Open(FileMode::ReadOnly))
{
    size_t size = 0;
    unsigned char* content = file->ReadAllBinary(size);
    // ...
    free(content);
}
```

---
### ReadBinary
Read a part of the content of the file as a binary (Need to free the pointer after).

Parameters:
- `offset`: Read offset in byte
- `size`: The size to read in byte
```cpp
unsigned char* ReadBinary(size_t offset, size_t size)
```
Code sample:
```cpp
std::shared_ptr<File> file = FileSystem::MakeFile("my_file.bin");

if(file->Open(FileMode::ReadOnly))
{
    unsigned char* content = file->ReadBinary(4, 30);
    // ...
    free(content);
}
```

---
### GetPath
Get file path.
```cpp
const std::string& GetPath() const
```
Code sample:
```cpp
std::shared_ptr<File> file = FileSystem::MakeFile("C:/my_folder/my_file.txt");

std::string filePath = file->GetPath(); // = C:/my_folder/my_file.txt
```

---
### GetFolderPath
Get file folder path.
```cpp
std::string GetFolderPath() const
```
Code sample:
```cpp
std::shared_ptr<File> file = FileSystem::MakeFile("C:/my_folder/my_file.txt");

std::string filePath = file->GetFolderPath(); // = C:/my_folder/
```

---
### GetFileName
Get file name.
```cpp
const std::string& GetFileName() const
```
Code sample:
```cpp
std::shared_ptr<File> file = FileSystem::MakeFile("C:/my_folder/my_file.txt");

std::string filePath = file->GetFileName(); // = my_file
```

---
### GetFileExtension
Get file extension (dot included).
```cpp
const std::string& GetFileExtension() const
```
Code sample:
```cpp
std::shared_ptr<File> file = FileSystem::MakeFile("C:/my_folder/my_file.txt");

std::string filePath = file->GetFileExtension(); // = .txt
```