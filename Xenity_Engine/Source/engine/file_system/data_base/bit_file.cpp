#include "bit_file.h"

#include <engine/file_system/file_system.h>
#include <engine/file_system/file.h>
#include <engine/assertions/assertions.h>
#include <engine/debug/stack_debug_object.h>

void BitFile::Create(const std::string& path)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	FileSystem::s_fileSystem->Delete(path);
	m_file = FileSystem::MakeFile(path);
	const bool openResult = m_file->Open(FileMode::WriteCreateFile);
	XASSERT(openResult, "[BitFile::Create] Failed to create bit file" + path);

	if (openResult) 
	{
		m_file->Close();
	}

	m_fileSize = 0;
}

void BitFile::Open(const std::string& path)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	m_file = FileSystem::MakeFile(path);
	const bool openResult = m_file->Open(FileMode::ReadOnly);
	XASSERT(openResult, "[BitFile::Open] Failed to open bit file" + path);
}

size_t BitFile::AddData(const std::vector<uint8_t>& data)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	const size_t dataSize = data.size();
	const bool openResult = m_file->Open(FileMode::WriteOnly);
	XASSERT(openResult, "[BitFile::AddData] Failed to open bit file");
	m_file->Write(reinterpret_cast<const unsigned char*>(data.data()), dataSize);
	m_file->Close();

	const size_t dataOffset = m_fileSize;
	m_fileSize += dataSize;

	return dataOffset;
}

size_t BitFile::AddData(const unsigned char* data, size_t size)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	const bool openResult = m_file->Open(FileMode::WriteOnly);
	XASSERT(openResult, "[BitFile::AddData] Failed to open bit file");
	m_file->Write(data, size);
	m_file->Close();

	const size_t dataOffset = m_fileSize;
	m_fileSize += size;

	return dataOffset;
}

unsigned char* BitFile::ReadBinary(size_t offset, size_t size)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	unsigned char* data = m_file->ReadBinary(offset, size);
	return data;
}
