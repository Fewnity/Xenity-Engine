#pragma once

/**
 * [Internal]
 */

#include <mutex>
#include <memory>
#include <vector>

class FileReference;

class AsyncFileLoading
{
public:

	/**
	* @brief Finish file loading when files are loaded with a thread
	*/
	static void FinishThreadedFileLoading();

	/**
	* @brief Add a file loading with a thread
	* @param file File to load
	*/
	static void AddFile(std::shared_ptr<FileReference> file);

private:
	static std::vector<std::shared_ptr<FileReference>> threadLoadedFiles;
	static std::mutex threadLoadingMutex;
};

