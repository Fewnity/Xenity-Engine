#pragma once

#include <string>

#include <engine/reflection/reflection.h>
#include <engine/file_system/file_type.h>
#include "bit_file.h"

class FileDataBaseEntry : public Reflective
{
public:
	std::string p; // Path
	uint64_t id; // Unique id
	uint64_t po; // Position in the binary file
	uint64_t s; // Size
	uint64_t mpo; // Meta position in the binary file
	uint64_t ms; // Meta Size
	FileType t; // Type

private:
	ReflectiveData GetReflectiveData() override;
};

class FileDataBase : public Reflective
{
public:
	ReflectiveData GetReflectiveData() override;
	void AddFile(FileDataBaseEntry* file);
	void Clear();
	void SaveToFile(const std::string& path);
	void LoadFromFile(const std::string& path);
	std::vector<FileDataBaseEntry*> fileList;
	BitFile bitFile;

private:
};
