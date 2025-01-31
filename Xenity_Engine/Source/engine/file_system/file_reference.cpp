// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "file_reference.h"
#include <engine/file_system/file.h>
#include <engine/file_system/data_base/file_data_base.h>
#include <engine/asset_management/project_manager.h>

std::string FileReference::ReadString() const
{
	std::string string = "";
#if defined(EDITOR)
	bool loadResult = m_file->Open(FileMode::ReadOnly);
	if (loadResult)
	{
		string = m_file->ReadAll();
		m_file->Close();
	}
#else
	unsigned char* binData = ProjectManager::fileDataBase.GetBitFile().ReadBinary(m_filePosition, m_fileSize);
	string = std::string(reinterpret_cast<const char*>(binData), m_fileSize);
	free(binData);
#endif

	return string;
}

unsigned char* FileReference::ReadBinary(size_t& size) const
{
	unsigned char* fileData = nullptr;
#if defined(EDITOR)
	bool openResult = m_file->Open(FileMode::ReadOnly);
	if (openResult)
	{
		size_t fileBufferSize = 0;
		fileData = m_file->ReadAllBinary(fileBufferSize);
		m_file->Close();
		size = fileBufferSize;
	}
#else
	fileData = ProjectManager::fileDataBase.GetBitFile().ReadBinary(m_filePosition, m_fileSize);
	size = m_fileSize;
#endif

	return fileData;
}
