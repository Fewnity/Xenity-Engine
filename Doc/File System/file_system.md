# File System

## Methods
| Definition | Description |
|-|-|
void CreateDirectory(const std::string& path) | Create a directory
void DeleteFile(const std::string& path) | Delete a file
bool Rename(const std::string& path, const std::string& newPath) | Rename a file
static std::shared_ptr<File> MakeFile(const std::string& path) | Create a new file
