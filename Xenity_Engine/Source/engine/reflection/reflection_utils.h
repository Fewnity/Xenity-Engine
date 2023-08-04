#pragma once

#include <json.hpp>
#include <unordered_map>
#include "reflection.h"

class ReflectionUtils
{
public:
	static void JsonToReflection(nlohmann::json j, Reflection& component);
	static nlohmann::json ReflectiveToJson(Reflection& relection);
	static nlohmann::json MapToJson(std::unordered_map<std::string, Variable> theMap, nlohmann::json json);
	static void JsonToMap(std::unordered_map<std::string, Variable>, nlohmann::json json);
};

