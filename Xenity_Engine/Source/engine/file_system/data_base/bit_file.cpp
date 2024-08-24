#include "bit_file.h"

#include <engine/file_system/file_system.h>
#include <engine/file_system/file.h>

void BitFile::Create(const std::string& path)
{
	FileSystem::s_fileSystem->Delete(path);
	file = FileSystem::MakeFile(path);
	file->Open(FileMode::WriteCreateFile);
	file->Close();

	fileSize = 0;
}

void BitFile::Open(const std::string& path)
{
	file = FileSystem::MakeFile(path);
	file->Open(FileMode::ReadOnly);
}

size_t BitFile::AddData(const std::vector<uint8_t>& data)
{
	const size_t dataSize = data.size();
	file->Open(FileMode::WriteOnly);
	file->Write(reinterpret_cast<const unsigned char*>(data.data()), dataSize);
	file->Close();

	const size_t dataOffset = fileSize;
	fileSize += dataSize;

	return dataOffset;
}

size_t BitFile::AddData(const unsigned char* data, size_t size)
{
	file->Open(FileMode::WriteOnly);
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
