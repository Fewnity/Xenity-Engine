// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "async_file_loading.h"

#include <engine/file_system/file_reference.h>
#include <engine/graphics/graphics.h>
#include <engine/assertions/assertions.h>

std::vector<std::shared_ptr<FileReference>> AsyncFileLoading::s_threadLoadedFiles;
std::mutex AsyncFileLoading::s_threadLoadingMutex;

void AsyncFileLoading::FinishThreadedFileLoading()
{
	s_threadLoadingMutex.lock();
	size_t threadFileCount = s_threadLoadedFiles.size();
	for (size_t i = 0; i < threadFileCount; i++)
	{
		if (s_threadLoadedFiles[i]->GetFileStatus() != FileStatus::FileStatus_Loading)
		{
			s_threadLoadedFiles[i]->OnLoadFileReferenceFinished();
			s_threadLoadedFiles.erase(s_threadLoadedFiles.begin() + i);
			Graphics::isRenderingBatchDirty = true; // Move this in a better location ???
			threadFileCount--;
			i--;
		}
	}

	s_threadLoadingMutex.unlock();
}

void AsyncFileLoading::AddFile(const std::shared_ptr<FileReference>& file)
{
	XASSERT(file != nullptr, "[AsyncFileLoading::AddFile] file is nullptr");

	s_threadLoadingMutex.lock();
	s_threadLoadedFiles.push_back(file);
	s_threadLoadingMutex.unlock();
}
