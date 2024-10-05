#pragma once

#include <string>
#include <vector>
#include <stdint.h>
#include <memory>

class File;

// Class that hold game's binary data
class BitFile
{
public:
	BitFile() = default;
	BitFile(const BitFile& other) = delete;
	BitFile& operator=(const BitFile&) = delete;

	/**
	* Create the file at the given path
	*/
	void Create(const std::string& path);

	/**
	* Open the file at the given path
	*/
	void Open(const std::string& path);

	/**
	* Add binary data at the end of the file
	*/
	size_t AddData(const std::vector<uint8_t>& data);

	/**
	* Add binary data at the end of the file
	*/
	size_t AddData(const unsigned char* data, size_t size);

	/**
	* Read binary data
	*/
	unsigned char* ReadBinary(size_t offset, size_t size);

private:
	std::shared_ptr<File> m_file;
	size_t m_fileSize = 0;
};

