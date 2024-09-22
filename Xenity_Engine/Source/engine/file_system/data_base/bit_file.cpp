#include "bit_file.h"

#include <engine/file_system/file_system.h>
#include <engine/file_system/file.h>
#include <engine/assertions/assertions.h>

void BitFile::Create(const std::string& path)
{
	FileSystem::s_fileSystem->Delete(path);
	file = FileSystem::MakeFile(path);
	const bool openResult = file->Open(FileMode::WriteCreateFile);
	XASSERT(openResult, "[BitFile::Create] Failed to create bit file" + path);

	if (openResult) 
	{
		file->Close();
	}

	fileSize = 0;
}

void BitFile::Open(const std::string& path)
{
	file = FileSystem::MakeFile(path);
	const bool openResult = file->Open(FileMode::ReadOnly);
	XASSERT(openResult, "[BitFile::Open] Failed to open bit file" + path);
}

size_t BitFile::AddData(const std::vector<uint8_t>& data)
{
	const size_t dataSize = data.size();
	const bool openResult = file->Open(FileMode::WriteOnly);
	XASSERT(openResult, "[BitFile::AddData] Failed to open bit file");
	file->Write(reinterpret_cast<const unsigned char*>(data.data()), dataSize);
	file->Close();

	const size_t dataOffset = fileSize;
	fileSize += dataSize;

	return dataOffset;
}

size_t BitFile::AddData(const unsigned char* data, size_t size)
{
	const bool openResult = file->Open(FileMode::WriteOnly);
	XASSERT(openResult, "[BitFile::AddData] Failed to open bit file");
	file->Write(data, size);
	file->Close();

	const size_t dataOffset = fileSize;
	fileSize += size;

	return dataOffset;
}

unsigned char* BitFile::ReadBinary(int offset, int size)
{
	unsigned char* data = file->ReadBinary(offset, size);
	return data;
}
