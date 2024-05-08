#pragma once

/**
 * [Internal]
 */

#if defined(_EE)
#include <engine/api.h>

#include "file.h"

#include <string>
#include <vector>

class Directory;
class File;

class API FilePS2 : public File
{
public:
	FilePS2() = delete;
	FilePS2(std::string _path);
	~FilePS2();

	/**
	* @brief Open the file
	* @param fileMode The mode to open the file
	* @return True if the file is opened successfully
	*/
	bool Open(FileMode fileMode) override;

	/**
	* @brief Close the file
	*/
	void Close() override;

	/**
	* @brief Check if the file exists
	* @return True if the file exists
	*/
	bool CheckIfExist() override;

	/**
	* @brief Read all the content of the file as a string
	*/
	std::string ReadAll() override;

	/**
	* @brief Read all the content of the file as a binary
	* @param size Output: The size of the binary
	* @return The binary data
	*/
	unsigned char* ReadAllBinary(int& size) override;

	/**
	* @brief Write string data to the file
	* @param data The data to write
	*/
	void Write(const std::string& data) override;

protected:
	int fileId;
};

#endif