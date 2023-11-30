#include "async_file_loading.h"

#include <engine/file_system/file_reference.h>

std::vector<std::shared_ptr<FileReference>> AsyncFileLoading::threadLoadedFiles;
std::mutex AsyncFileLoading::threadLoadingMutex;

void AsyncFileLoading::FinishThreadedFileLoading()
{
	threadLoadingMutex.lock();
	size_t threadFileCount = threadLoadedFiles.size();
	for (size_t i = 0; i < threadFileCount; i++)
	{
		if (!threadLoadedFiles[i]->isLoading)
		{
			threadLoadedFiles[i]->OnLoadFileReferenceFinished();
			threadLoadedFiles.erase(threadLoadedFiles.begin() + i);
			threadFileCount--;
			i--;
		}
	}

	threadLoadingMutex.unlock();
}

void AsyncFileLoading::AddFile(std::shared_ptr<FileReference> file)
{
	threadLoadingMutex.lock();
	threadLoadedFiles.push_back(file);
	threadLoadingMutex.unlock();
}
