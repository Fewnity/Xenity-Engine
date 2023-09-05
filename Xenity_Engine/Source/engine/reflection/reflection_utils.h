#pragma once

#include <json.hpp>
#include <unordered_map>
#include "reflection.h"

class ReflectionUtils
{
public:

	/**
	* Fill Reflection from Json data
	* @param j Json data
	* @param reflection Reflection 
	*/
	static void JsonToReflection(nlohmann::json j, Reflection& reflection);

	/**
	* Fill Reflection map from Json data
	* @param json Json data
	* @param theMap The Reflection map to fill
	*/
	static void JsonToMap(nlohmann::json json, std::unordered_map<std::string, ReflectionEntry> theMap);

	/**
	* Create Json data from Reflection
	* @param reflection Reflection
	*/
	static nlohmann::json ReflectionToJson(Reflection& reflection);

	/**
	* Create Json data from Reflection map
	* @param theMap The Reflection map
	*/
	static nlohmann::json MapToJson(std::unordered_map<std::string, ReflectionEntry> theMap);

private:
	template <typename T>
	static void FillFileReference(uint64_t fileId, std::reference_wrapper<std::shared_ptr<T>>* valuePtr);
};

