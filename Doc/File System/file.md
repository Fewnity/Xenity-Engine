# File

## Methods
| Definition | Description |
|-|-|
void Write(const std::string& data) | Write data into the file
std::string ReadAll() | Read all the file
unsigned char* ReadAllBinary(int& size) | Read all the file in binary mode (Need to free the pointer after)
bool CheckIfExist() | Check if the file exists
bool Open(bool createFileIfNotFound) | Open the file
void Close() | Close file
std::string GetPath() | Get file's path
std::string GetFolderPath() | Get file's folder path
std::string GetFileName() | Get file's name
std::string GetFileExtension() | Get file's extension (dot included)
