#pragma once

/**
 * [Internal]
 */

#include <engine/api.h>

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
	bool Open(FileMode fileMode) override;

protected:
	std::fstream file;
};