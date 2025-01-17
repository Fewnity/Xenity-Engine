#pragma once

#include <string>
#include <vector>
#include <stdint.h>
#include <memory>

class File;
class FileReference;

// Class that hold game's binary data
class BitFile
{
public:
	BitFile() = default;
	BitFile(const BitFile& other) = delete;
	BitFile& operator=(const BitFile&) = delete;

	/**
	* @brief Create the file at the given path
	*/
	void Create(const std::string& path);

	/**
	* @brief Open the file at the given path
	*/
	void Open(const std::string& path);

	/**
	* @brief Close the file
	*/
	void Close();

	/**
	* @brief Add binary data at the end of the file
	* 
	*/
	size_t AddData(const std::vector<uint8_t>& data);

	/**
	* @brief Add binary data at the end of the file
	*/
	size_t AddData(const unsigned char* data, size_t size);

	/**
	* @brief Read binary data
	*/
	unsigned char* ReadBinary(size_t offset, size_t size);

	/**
	* @brief Read binary data
	*/
	unsigned char* ReadBinary(const FileReference& fileReference);

	std::shared_ptr<File> GetFile() const { return m_file; }

private:
	std::shared_ptr<File> m_file;
	size_t m_fileSize = 0;
};

