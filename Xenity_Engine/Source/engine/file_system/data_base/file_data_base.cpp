#include "file_data_base.h"
#include <engine/file_system/file_system.h>
#include <engine/reflection/reflection_utils.h>

using ordered_json = nlohmann::ordered_json;

ReflectiveData FileDataBaseEntry::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, p, "p", true);
	Reflective::AddVariable(reflectedVariables, id, "id", true);
	Reflective::AddVariable(reflectedVariables, po, "po", true);
	Reflective::AddVariable(reflectedVariables, s, "s", true);
	Reflective::AddVariable(reflectedVariables, mpo, "mpo", true);
	Reflective::AddVariable(reflectedVariables, ms, "ms", true);
	Reflective::AddVariable(reflectedVariables, t, "t", true);
	return reflectedVariables;
}

ReflectiveData FileDataBase::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, fileList, "fl", true);
	return reflectedVariables;
}

void FileDataBase::AddFile(FileDataBaseEntry* file)
{
	fileList.push_back(file);
}

void FileDataBase::Clear()
{
	const size_t listSize = fileList.size();
	for (size_t i = 0; i < listSize; i++)
	{
		delete fileList[i];
	}
	fileList.clear();
}

void FileDataBase::SaveToFile(const std::string& path)
{
	ordered_json j;
	j["Values"] = ReflectionUtils::ReflectiveDataToJson(GetReflectiveData());

	std::vector<uint8_t> binaryFileDataBase;
	ordered_json::to_msgpack(j, binaryFileDataBase);
	std::string stringBinary(binaryFileDataBase.begin(), binaryFileDataBase.end());

	FileSystem::fileSystem->Delete(path);
	std::shared_ptr<File> file = FileSystem::MakeFile(path);
	file->Open(FileMode::WriteCreateFile);
	file->Write(stringBinary);
	file->Close();
}

void FileDataBase::LoadFromFile(const std::string& path)
{
	Clear();

	std::shared_ptr<File> file = FileSystem::MakeFile(path);
	std::vector<uint8_t> binaryFileDataBase;
	file->Open(FileMode::ReadOnly);
	int dataSize = 0;
	unsigned char * data = file->ReadAllBinary(dataSize);
	file->Close();
	binaryFileDataBase.resize(dataSize);
	memcpy(binaryFileDataBase.data(), data, dataSize);
	free(data);

	ordered_json j;
	j = ordered_json::from_msgpack(binaryFileDataBase);

	ReflectionUtils::JsonToReflectiveData(j, GetReflectiveData());
}
