#pragma once

/**
 * [Internal] Maybe not? 
 *	Maybe create a Json Class to be able to make json from objects
 */

#include <json.hpp>
#include <unordered_map>
#include "reflection.h"

class File;

class ReflectionUtils
{
public:

	/**
	* Fill Reflective object from Json data
	* @param j Json data
	* @param reflection Reflective object
	*/
	static void JsonToReflective(const nlohmann::json& j, Reflective& reflective);

	/**
	* Fill Reflective data list from Json data
	* @param json Json data
	* @param theMap The Reflective data list to fill
	*/
	static void JsonToReflectiveData(const nlohmann::json& json, ReflectiveData dataList);

	/**
	* Create Json data from Reflective object
	* @param reflection Reflective object
	*/
	static nlohmann::json ReflectiveToJson(Reflective& reflective);

	/**
	* Create Json data from Reflective data list
	* @param theMap The Reflective data list
	*/
	static nlohmann::json ReflectiveDataToJson(ReflectiveData dataList);

	/**
	* Read a file to fill a Reflective data list
	* @param file File to read
	*/
	static bool FileToReflectiveData(std::shared_ptr<File> file, ReflectiveData dataList);

	/**
	* Write Reflective data in a list
	* @param file File to write in
	*/
	static bool ReflectiveDataToFile(ReflectiveData dataList, std::shared_ptr<File> file);

private:
	/**
	* Fill a file reference variable
	*/
	template <typename T>
	static void FillFileReference(uint64_t fileId, std::reference_wrapper<std::shared_ptr<T>>* valuePtr);

	/**
	* Fill a vector of file reference
	*/
	template <typename T>
	static void FillVectorFileReference(nlohmann::json kvValue, std::reference_wrapper<std::vector<std::shared_ptr<T>>>* valuePtr);
};

