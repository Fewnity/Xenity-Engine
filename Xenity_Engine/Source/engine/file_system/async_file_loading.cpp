// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Grégory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "async_file_loading.h"

#include <engine/file_system/file_reference.h>
#include <engine/graphics/graphics.h>
#include <engine/assertions/assertions.h>

std::vector<std::shared_ptr<FileReference>> AsyncFileLoading::threadLoadedFiles;
std::mutex AsyncFileLoading::threadLoadingMutex;

void AsyncFileLoading::FinishThreadedFileLoading()
{
	threadLoadingMutex.lock();
	size_t threadFileCount = threadLoadedFiles.size();
	for (size_t i = 0; i < threadFileCount; i++)
	{
		if (!threadLoadedFiles[i]->GetIsLoading())
		{
			threadLoadedFiles[i]->OnLoadFileReferenceFinished();
			threadLoadedFiles.erase(threadLoadedFiles.begin() + i);
			Graphics::isRenderingBatchDirty = true; // Move this in a better location ???
			threadFileCount--;
			i--;
		}
	}

	threadLoadingMutex.unlock();
}

void AsyncFileLoading::AddFile(std::shared_ptr<FileReference> file)
{
	DXASSERT(file != nullptr, "[AsyncFileLoading::AddFile] file is nullptr")

	threadLoadingMutex.lock();
	threadLoadedFiles.push_back(file);
	threadLoadingMutex.unlock();
}
