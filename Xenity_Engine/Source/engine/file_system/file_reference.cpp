// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "file_reference.h"
#include <engine/file_system/file.h>
#include <engine/file_system/data_base/file_data_base.h>
#include <engine/asset_management/project_manager.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/debug/debug.h>

FileReference::FileReference(bool isForCooking)
{
	m_isForCooking = isForCooking;
	if (!m_isForCooking)
	{
		AssetManager::AddReflection(this);
	}
}

FileReference::~FileReference()
{
	if (!m_isForCooking)
	{
		AssetManager::RemoveReflection(this);
	}
}

std::string FileReference::ReadString() const
{
	std::string string = "";
#if defined(EDITOR)
	const bool loadResult = m_file->Open(FileMode::ReadOnly);
	if (loadResult)
	{
		string = m_file->ReadAll();
		m_file->Close();
	}
#else
	unsigned char* binData = ProjectManager::s_fileDataBase.GetBitFile().ReadBinary(m_filePosition, m_fileSize);
	string = std::string(reinterpret_cast<const char*>(binData), m_fileSize);
	free(binData);
#endif

	return string;
}

unsigned char* FileReference::ReadBinary(size_t& size) const
{
	unsigned char* fileData = nullptr;
#if defined(EDITOR)
	const bool openResult = m_file->Open(FileMode::ReadOnly);
	if (openResult)
	{
		size_t fileBufferSize = 0;
		fileData = m_file->ReadAllBinary(fileBufferSize);
		m_file->Close();
		size = fileBufferSize;
	}
#else
	fileData = ProjectManager::s_fileDataBase.GetBitFile().ReadBinary(m_filePosition, m_fileSize);
	size = m_fileSize;
#endif

	return fileData;
}
