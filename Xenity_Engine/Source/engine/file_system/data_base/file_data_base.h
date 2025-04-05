#pragma once

#include <string>

#include <engine/reflection/reflection.h>
#include <engine/file_system/file_type.h>
#include <engine/file_system/data_base/integrity_state.h>
#include "bit_file.h"

/*
* @brief Class that hold data about a binarized file (Location in the bit file, size, type, etc)
* @brief Use small names to reduce the size of the json
*/
class FileDataBaseEntry : public Reflective
{
public:
	std::string p = ""; // Path
	uint64_t id = 0; // Unique id
	uint64_t po = 0; // Position in the binary file in byte
	uint64_t s = 0; // Size in byte
	uint64_t mpo = 0; // Meta position in the binary file in byte
	uint64_t ms = 0; // Meta Size in byte
	FileType t = FileType::File_Other; // Type

private:
	ReflectiveData GetReflectiveData() override;
};

// Class that hold data about binarized files in the binary file
class FileDataBase : public Reflective
{
public:
	FileDataBase() = default;
	FileDataBase(const FileDataBase& other) = delete;
	FileDataBase& operator=(const FileDataBase&) = delete;

	[[nodiscard]] ReflectiveData GetReflectiveData() override;

	/**
	* @brief Add a infos about a file to the list
	*/
	void AddFile(FileDataBaseEntry* file);

	/**
	* @brief Clear all files infos
	*/
	void Clear();

	/**
	* @brief Save infos to a file at the given path
	*/
	void SaveToFile(const std::string& path);

	/**
	* @brief Load infos from a file at the given path
	*/
	[[nodiscard]] bool LoadFromFile(const std::string& path);

	/*
	* @brief Get the files list
	*/
	[[nodiscard]] const std::vector<FileDataBaseEntry*>& GetFileList() const
	{
		return m_fileList;
	}

	/**
	* @brief Get the bit file
	*/
	[[nodiscard]] BitFile& GetBitFile()
	{
		return m_bitFile;
	}

	[[nodiscard]] IntegrityState CheckIntegrity();

private:
	// Pointer list because the reflection system do not support basic objects in lists
	std::vector<FileDataBaseEntry*> m_fileList;
	BitFile m_bitFile;
};
