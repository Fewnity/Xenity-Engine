#pragma once

/**
 * [Internal] Maybe not? 
 *	Maybe create a Json Class to be able to make json from objects
 */

#include <json.hpp>
#include <unordered_map>
#include "reflection.h"
#include <engine/tools/template_utils.h>

class File;

class ReflectionUtils
{
public:

#pragma region Fill variables

	static void ReflectiveToReflective(Reflective& fromReflective, Reflective& toReflective);

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
	static void JsonToReflectiveData(const nlohmann::json& json, const ReflectiveData& dataList);

#pragma endregion

#pragma region Fill json

	/**
	* Create Json data from Reflective object
	* @param reflection Reflective object
	*/
	static nlohmann::json ReflectiveToJson(Reflective& reflective);

	/**
	* Create Json data from Reflective data list
	* @param theMap The Reflective data list
	*/
	static nlohmann::json ReflectiveDataToJson(const ReflectiveData& dataList);

#pragma endregion

#pragma region IO

	/**
	* Read a file to fill a Reflective data list
	* @param file File to read
	*/
	static bool FileToReflectiveData(std::shared_ptr<File> file, const ReflectiveData& dataList);

	/**
	* Write Reflective data in a list
	* @param file File to write in
	*/
	static bool ReflectiveDataToFile(const ReflectiveData& dataList, std::shared_ptr<File> file);

#pragma endregion

private:
	/**
	* Fill a file reference variable
	*/
	template <typename T>
	static void FillFileReference(const uint64_t fileId, const std::reference_wrapper<std::shared_ptr<T>>* valuePtr);

	/**
	* Fill a vector of file reference
	*/
	template <typename T>
	static void FillVectorFileReference(const nlohmann::json& kvValue, const std::reference_wrapper<std::vector<std::shared_ptr<T>>>* valuePtr);

#pragma region Fill variables

	template<typename T>
	std::enable_if_t<std::is_base_of<FileReference, T>::value, void>
	static JsonToVariable(const nlohmann::json& jsonValue, const std::reference_wrapper<std::vector<std::shared_ptr<T>>>* valuePtr);

	template<typename T>
	std::enable_if_t<std::is_base_of<GameObject, T>::value || std::is_base_of<Transform, T>::value || std::is_base_of<Component, T>::value || std::is_base_of<Collider, T>::value, void>
	static JsonToVariable(const nlohmann::json& jsonValue, const std::reference_wrapper<std::vector<std::weak_ptr<T>>>* valuePtr);

	template<typename T>
	std::enable_if_t<std::is_base_of<FileReference, T>::value, void>
	static JsonToVariable(const nlohmann::json& jsonValue, const std::reference_wrapper<std::shared_ptr<T>>* valuePtr);

	template<typename T>
	std::enable_if_t<std::is_base_of<GameObject, T>::value || std::is_base_of<Transform, T>::value || std::is_base_of<Component, T>::value || std::is_base_of<Collider, T>::value, void>
	static JsonToVariable(const nlohmann::json& jsonValue, const std::reference_wrapper<std::weak_ptr<T>>* valuePtr);

	static void JsonToVariable(const nlohmann::json& jsonValue, const std::reference_wrapper<std::vector<Reflective>>* valuePtr);

	static void JsonToVariable(const nlohmann::json& jsonValue, const std::reference_wrapper<Reflective>* valuePtr);

	template<typename T>
	std::enable_if_t<!std::is_base_of<Reflective, T>::value && !is_shared_ptr<T>::value && !is_weak_ptr<T>::value && !is_vector<T>::value, void>
	static JsonToVariable(const nlohmann::json& jsonValue, const std::reference_wrapper<T>* valuePtr);

#pragma endregion

#pragma region Fill json

	template<typename T>
	std::enable_if_t<!std::is_base_of<Reflective, T>::value && !is_shared_ptr<T>::value && !is_weak_ptr<T>::value && !is_vector<T>::value, void>
	static VariableToJson(nlohmann::json& jsonValue, const std::string& key, const std::reference_wrapper<T>* valuePtr);

	static void VariableToJson(nlohmann::json& jsonValue, const std::string& key, const std::reference_wrapper<Reflective>* valuePtr);
	static void VariableToJson(nlohmann::json& jsonValue, const std::string& key, const std::reference_wrapper<std::vector<Reflective>>* valuePtr);

	template<typename T>
	std::enable_if_t<std::is_base_of<GameObject, T>::value || std::is_base_of<Transform, T>::value || std::is_base_of<Component, T>::value || std::is_base_of<Collider, T>::value, void>
	static VariableToJson(nlohmann::json& jsonValue, const std::string& key, const std::reference_wrapper<std::weak_ptr<T>>* valuePtr);

	template<typename T>
	std::enable_if_t<std::is_base_of<FileReference, T>::value, void>
	static VariableToJson(nlohmann::json& jsonValue, const std::string& key, const std::reference_wrapper<std::shared_ptr<T>>* valuePtr);

	template<typename T>
	std::enable_if_t<std::is_base_of<GameObject, T>::value || std::is_base_of<Transform, T>::value || std::is_base_of<Component, T>::value || std::is_base_of<Collider, T>::value, void>
	static VariableToJson(nlohmann::json& jsonValue, const std::string& key, const std::reference_wrapper<std::vector<std::weak_ptr<T>>>* valuePtr);

	template<typename T>
	std::enable_if_t<std::is_base_of<FileReference, T>::value, void>
	static VariableToJson(nlohmann::json& jsonValue, const std::string& key, const std::reference_wrapper<std::vector<std::shared_ptr<T>>>* valuePtr);

#pragma endregion
};

