#pragma once
#include <string>
#include <vector>
#include <stdint.h>
#include <memory>

class File;

class BitFile
{
public:
	void Create(const std::string& path);
	void Open(const std::string& path);

	size_t AddData(const std::vector<uint8_t>& data);
	size_t AddData(const unsigned char* data, size_t size);
	unsigned char* ReadBinary(int offset, int size);

	std::shared_ptr<File> file;
	size_t fileSize;
};

