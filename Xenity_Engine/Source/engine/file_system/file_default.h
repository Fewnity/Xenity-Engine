#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include "file.h"

#include <string>
#include <fstream>
#include <vector>

class Directory;
class File;

class API FileDefault : public File
{
public:
	FileDefault() = delete;
	FileDefault(std::string _path);

	void Close() override;
	void Write(const std::string& data) override;
	std::string ReadAll() override;
	unsigned char* ReadAllBinary(int& size) override;
	bool CheckIfExist() override;
	bool Open(bool createFileIfNotFound) override;

protected:
	std::fstream file;
};