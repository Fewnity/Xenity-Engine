#pragma once

/**
 * [Internal]
 */

#include <string>

class FileHandler
{
public:

	/**
	* Check if code files have changed
	*/
	static bool HasCodeChanged(const std::string& folderPath);

	/**
	* Check if files have changed or added
	*/
	static bool HasFileChangedOrAdded(const std::string& folderPath);
private:

	/**
	* Check if files have changed or added recursively
	*/
	static bool HasFileChangedOrAddedRecursive(const std::string& folderPath);

	static uint64_t lastModifiedCodeFileTime;
	static uint64_t lastModifiedFileTime;
	static uint32_t lastFileCount;
	static uint32_t tempFileCount;
};

