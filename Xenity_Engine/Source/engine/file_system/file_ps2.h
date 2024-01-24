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

	void Close() override;
	void Write(const std::string &data) override;
	std::string ReadAll() override;
	unsigned char *ReadAllBinary(int &size) override;
	bool CheckIfExist() override;
	bool Open(FileMode fileMode) override;

protected:
	int fileId;
};

#endif