#pragma once

/**
 * @brief [Internal] Maybe not? 
 * @brief Maybe create a Json Class to be able to make json from objects
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

	/**
	* @brief Fill a Reflective object with another Reflective object
	*/
	static void ReflectiveToReflective(Reflective& fromReflective, Reflective& toReflective);

	/**
	* @brief Fill Reflective object from Json data
	* @param j Json data
	* @param reflection Reflective object
	*/
	static void JsonToReflective(const nlohmann::json& j, Reflective& reflective);

	/**
	* @brief Fill Reflective data list from Json data
	* @param json Json data
	* @param theMap The Reflective data list to fill
	*/
	static void JsonToReflectiveData(const nlohmann::json& json, const ReflectiveData& dataList);

#pragma endregion

#pragma region Fill json

	/**
	* @brief Create Json data from Reflective object
	* @param reflection Reflective object
	* @return Json data
	*/
	static nlohmann::json ReflectiveToJson(Reflective& reflective);

	/**
	* @brief Create Json data from Reflective data list
	* @param theMap The Reflective data list
	* @return Json data
	*/
	static nlohmann::json ReflectiveDataToJson(const ReflectiveData& dataList);

#pragma endregion

#pragma region IO

	/**
	* @brief Read a file to fill a Reflective data list
	* @param file File to read
	* @param dataList Reflective data list to fill
	* @return True if the file has been read successfully
	*/
	static bool FileToReflectiveData(std::shared_ptr<File> file, const ReflectiveData& dataList);

	/**
	* @brief Write Reflective data in a list
	* @param dataList Reflective data list
	* @param file File to write in
	* @return True if the data has been written successfully
	*/
	static bool ReflectiveDataToFile(const ReflectiveData& dataList, std::shared_ptr<File> file);

#pragma endregion

private:
	/**
	* @brief Find, load and fill a file reference variable
	* @param fileId File id
	* @param valuePtr Variable to fill
	*/
	template <typename T>
	static void FillFileReference(const uint64_t fileId, const std::reference_wrapper<std::shared_ptr<T>>* valuePtr);

	/**
	* @brief Find, load and fill a vector of file reference
	* @param kvValue Json data
	* @param valuePtr Variable to fill
	*/
	template <typename T>
	static void FillVectorFileReference(const nlohmann::json& kvValue, const std::reference_wrapper<std::vector<std::shared_ptr<T>>>* valuePtr);

#pragma region Fill variables

	/**
	* @brief Fill a vector variable with a json value (basic type)
	* @param jsonValue Json value
	* @param valuePtr Variable to fill
	* @param entry Reflective entry
	*/
	template<typename T>
	std::enable_if_t<std::is_base_of<FileReference, T>::value, void>
	static JsonToVariable(const nlohmann::json& jsonValue, const std::reference_wrapper<std::vector<std::shared_ptr<T>>>* valuePtr, const ReflectiveEntry& entry);

	/**
	* @brief Fill a vector variable with a json value (GameObject, Transform, Component, Collider)
	* @param jsonValue Json value
	* @param valuePtr Variable to fill
	* @param entry Reflective entry
	*/
	template<typename T>
	std::enable_if_t<std::is_base_of<GameObject, T>::value || std::is_base_of<Transform, T>::value || std::is_base_of<Component, T>::value || std::is_base_of<Collider, T>::value, void>
	static JsonToVariable(const nlohmann::json& jsonValue, const std::reference_wrapper<std::vector<std::weak_ptr<T>>>* valuePtr, const ReflectiveEntry& entry);

	/**
	* @brief Fill a variable with a json value (file reference)
	* @param jsonValue Json value
	* @param valuePtr Variable to fill
	* @param entry Reflective entry
	*/
	template<typename T>
	std::enable_if_t<std::is_base_of<FileReference, T>::value, void>
	static JsonToVariable(const nlohmann::json& jsonValue, const std::reference_wrapper<std::shared_ptr<T>>* valuePtr, const ReflectiveEntry& entry);

	/**
	* @brief Fill a variable with a json value (GameObject, Transform, Component, Collider)
	* @param jsonValue Json value
	* @param valuePtr Variable to fill
	* @param entry Reflective entry
	*/
	template<typename T>
	std::enable_if_t<std::is_base_of<GameObject, T>::value || std::is_base_of<Transform, T>::value || std::is_base_of<Component, T>::value || std::is_base_of<Collider, T>::value, void>
	static JsonToVariable(const nlohmann::json& jsonValue, const std::reference_wrapper<std::weak_ptr<T>>* valuePtr, const ReflectiveEntry& entry);

	/**
	* @brief Fill a vector variable with a json value (reflective)
	* @param jsonValue Json value
	* @param valuePtr Variable to fill
	* @param entry Reflective entry
	*/
	static void JsonToVariable(const nlohmann::json& jsonValue, const std::reference_wrapper<std::vector<Reflective*>>* valuePtr, const ReflectiveEntry& entry);

	/**
	* @brief Fill a variable with a json value (reflective)
	* @param jsonValue Json value
	* @param valuePtr Variable to fill
	* @param entry Reflective entry
	*/
	static void JsonToVariable(const nlohmann::json& jsonValue, const std::reference_wrapper<Reflective>* valuePtr, const ReflectiveEntry& entry);

	/**
	* @brief Fill a variable with a json value (basic type)
	* @param jsonValue Json value
	* @param valuePtr Variable to fill
	* @param entry Reflective entry
	*/
	template<typename T>
	std::enable_if_t<!std::is_base_of<Reflective, T>::value && !is_shared_ptr<T>::value && !is_weak_ptr<T>::value && !is_vector<T>::value, void>
	static JsonToVariable(const nlohmann::json& jsonValue, const std::reference_wrapper<T>* valuePtr, const ReflectiveEntry& entry);

#pragma endregion

#pragma region Fill json

	/**
	* @brief Fill a json value with a variable (basic type)
	* @param jsonValue Json value
	* @param key Key
	* @param valuePtr Variable to fill
	*/
	template<typename T>
	std::enable_if_t<!std::is_base_of<Reflective, T>::value && !is_shared_ptr<T>::value && !is_weak_ptr<T>::value && !is_vector<T>::value, void>
	static VariableToJson(nlohmann::json& jsonValue, const std::string& key, const std::reference_wrapper<T>* valuePtr);

	/**
	* @brief Fill a json value with a variable (reflective)
	* @param jsonValue Json value
	* @param key Key
	* @param valuePtr Variable to fill
	*/
	static void VariableToJson(nlohmann::json& jsonValue, const std::string& key, const std::reference_wrapper<Reflective>* valuePtr);

	/**
	* @brief Fill a json value with a vector variable (reflective)
	* @param jsonValue Json value
	* @param key Key
	* @param valuePtr Variable to fill
	*/
	static void VariableToJson(nlohmann::json& jsonValue, const std::string& key, const std::reference_wrapper<std::vector<Reflective*>>* valuePtr);

	/**
	* @brief Fill a json value with a vector variable (GameObject, Transform, Component, Collider)
	* @param jsonValue Json value
	* @param key Key
	* @param valuePtr Variable to fill
	*/
	template<typename T>
	std::enable_if_t<std::is_base_of<GameObject, T>::value || std::is_base_of<Transform, T>::value || std::is_base_of<Component, T>::value || std::is_base_of<Collider, T>::value, void>
	static VariableToJson(nlohmann::json& jsonValue, const std::string& key, const std::reference_wrapper<std::weak_ptr<T>>* valuePtr);

	/**
	* @brief Fill a json value with a vector variable (file reference)
	* @param jsonValue Json value
	* @param key Key
	* @param valuePtr Variable to fill
	*/
	template<typename T>
	std::enable_if_t<std::is_base_of<FileReference, T>::value, void>
	static VariableToJson(nlohmann::json& jsonValue, const std::string& key, const std::reference_wrapper<std::shared_ptr<T>>* valuePtr);

	/**
	* @brief Fill a json value with a vector variable (GameObject, Transform, Component, Collider)
	* @param jsonValue Json value
	* @param key Key
	* @param valuePtr Variable to fill
	*/
	template<typename T>
	std::enable_if_t<std::is_base_of<GameObject, T>::value || std::is_base_of<Transform, T>::value || std::is_base_of<Component, T>::value || std::is_base_of<Collider, T>::value, void>
	static VariableToJson(nlohmann::json& jsonValue, const std::string& key, const std::reference_wrapper<std::vector<std::weak_ptr<T>>>* valuePtr);


	/**
	* @brief Fill a json value with a vector variable (file reference)
	* @param jsonValue Json value
	* @param key Key
	* @param valuePtr Variable to fill
	*/
	template<typename T>
	std::enable_if_t<std::is_base_of<FileReference, T>::value, void>
	static VariableToJson(nlohmann::json& jsonValue, const std::string& key, const std::reference_wrapper<std::vector<std::shared_ptr<T>>>* valuePtr);

#pragma endregion
};

