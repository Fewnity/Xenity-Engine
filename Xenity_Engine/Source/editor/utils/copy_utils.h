#pragma once

#include <string>
#include <vector>

struct CopyEntry
{
	bool isFolder = false;
	std::string sourcePath = "";
	std::string destPath = "";
};

class CopyUtils
{
public:
	/**
	* @brief Execute all created copy entries and clear the list
	* @return True if all copy entries were successful
	*/
	static bool ExecuteCopyEntries();

	/**
	* @brief Create a new copy to be executed later
	* @param isFolder True if the source is a folder
	* @param source Source path
	* @param dest Destination path
	*/
	static void AddCopyEntry(bool isFolder, const std::string& source, const std::string& dest);

private:
	static std::vector<CopyEntry> copyEntries;
};

