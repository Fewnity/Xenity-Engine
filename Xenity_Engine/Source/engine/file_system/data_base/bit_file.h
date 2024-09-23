#pragma once

#include <string>
#include <vector>
#include <stdint.h>
#include <memory>

class File;

// Class that hold binary data
class BitFile
{
public:
	void Create(const std::string& path);
	void Open(const std::string& path);

	size_t AddData(const std::vector<uint8_t>& data);
	size_t AddData(const unsigned char* data, size_t size);
	unsigned char* ReadBinary(int offset, int size);

private:
	std::shared_ptr<File> m_file;
	size_t m_fileSize = 0;
};

