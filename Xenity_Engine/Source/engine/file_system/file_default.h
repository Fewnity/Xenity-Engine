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

	/**
	* Open the file
	* @param fileMode The mode to open the file
	* @return True if the file is opened successfully
	*/
	bool Open(FileMode fileMode) override;

	/**
	* Close the file
	*/
	void Close() override;
	
	/**
	* Check if the file exists
	* @return True if the file exists
	*/
	bool CheckIfExist() override;

	/**
	* Read all the content of the file as a string
	*/
	std::string ReadAll() override;

	/**
	* Read all the content of the file as a binary
	* @param size Output: The size of the binary
	* @return The binary data
	*/
	unsigned char* ReadAllBinary(int& size) override;
	
	/**
	* Write string data to the file
	* @param data The data to write
	*/
	void Write(const std::string& data) override;

protected:
	std::fstream file;
};