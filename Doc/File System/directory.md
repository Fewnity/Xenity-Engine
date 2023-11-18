# Directory

## Methods
| Definition | Description |
|-|-|
bool CheckIfExist() | Check if the directory exists
std::vector\<std::shared_ptr\<File>> GetAllFiles(bool recursive) | Get all the files of the directory (can be very slow)
std::string GetPath() | Get directory's path

## Members
| Name | Type | Description |
|-|-|-|
files | std::vector\<std::shared_ptr\<File>> | Directory's files list (Filled with GetAllFiles(...))
subdirectories | std::vector\<std::shared_ptr\<Directory>> | Directory's subdirectories list (Filled with GetAllFiles(...))
