#pragma once

#include <engine/reflection/reflection.h>
#include <engine/tools/template_utils.h>
#include <string>
#include <vector>

class FileReference;

class FileReferenceFinder
{
public:
	template<typename T>
	static bool GetFileRefId(const T& var, std::vector <uint64_t>& ids);

	template<typename T>
	std::enable_if_t<std::is_base_of<FileReference, T>::value, bool>
	static GetFileRefId(const std::reference_wrapper<std::vector<std::shared_ptr<T>>>* valuePtr, std::vector <uint64_t>& ids);

	template<typename T>
	std::enable_if_t<std::is_base_of<FileReference, T>::value, bool>
	static GetFileRefId(const std::reference_wrapper<std::shared_ptr<T>>* valuePtr, std::vector<uint64_t>& ids);

	static void GetUsedFilesInReflectiveData(std::vector<uint64_t>& usedFilesIds, const ReflectiveData& reflectiveData);
};

