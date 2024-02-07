#pragma once
#include <engine/api.h>

#include <unordered_map>
#include <map>
#include <string>
#include <memory>
#include <variant>
#include <vector>
#include <optional>
#include <iostream>

class GameObject;
class Transform;
class Color;
class Texture;
class MeshData;
class MeshRenderer;
class Reflective;
class Component;
class FileReference;
class AudioClip;
class Scene;
class SkyBox;
class Font;
class Shader;
class Material;
class Collider;

typedef std::variant <
	std::reference_wrapper<std::weak_ptr<Component>>,
	std::reference_wrapper<std::weak_ptr<Collider>>,
	std::reference_wrapper<int>,
	std::reference_wrapper<uint64_t>,
	std::reference_wrapper<double>,
	std::reference_wrapper<float>,
	std::reference_wrapper<bool>,
	std::reference_wrapper<std::string>,
	std::reference_wrapper<Reflective>,
	std::reference_wrapper<std::weak_ptr<GameObject>>,
	std::reference_wrapper<std::weak_ptr<Transform>>,
	std::reference_wrapper<std::shared_ptr<Texture>>,
	std::reference_wrapper<std::shared_ptr<MeshData>>,
	std::reference_wrapper<std::shared_ptr<AudioClip>>,
	std::reference_wrapper<std::shared_ptr<Scene>>,
	std::reference_wrapper<std::shared_ptr<SkyBox>>,
	std::reference_wrapper<std::shared_ptr<Font>>,
	std::reference_wrapper<std::shared_ptr<Shader>>,
	std::reference_wrapper<std::shared_ptr<Material>>,

	std::reference_wrapper<std::vector<Reflective>>,
	//std::reference_wrapper<std::vector<int>>,
	std::reference_wrapper<std::vector<std::shared_ptr<Texture>>>,
	std::reference_wrapper<std::vector<std::shared_ptr<MeshData>>>,
	std::reference_wrapper<std::vector<std::shared_ptr<AudioClip>>>,
	std::reference_wrapper<std::vector<std::shared_ptr<Scene>>>,
	std::reference_wrapper<std::vector<std::shared_ptr<SkyBox>>>,
	std::reference_wrapper<std::vector<std::shared_ptr<Font>>>,
	std::reference_wrapper<std::vector<std::shared_ptr<Shader>>>,
	std::reference_wrapper<std::vector<std::shared_ptr<Material>>>,

	std::reference_wrapper<std::vector<std::weak_ptr<GameObject>>>,
	std::reference_wrapper<std::vector<std::weak_ptr<Transform>>>,
	std::reference_wrapper<std::vector<std::weak_ptr<Component>>>> VariableReference;

class EnumValueName 
{
public:
	int value;
	std::string name;
};

/**
* Class to get the list of enum strings lists
* initialise enumStringsLists before calling main()
*/
class EnumHelper
{
public:
	static std::map<uint64_t, std::vector<EnumValueName>>& GetEnumStringsLists()
	{
		static std::map<uint64_t, std::vector<EnumValueName>> enumStringsLists;
		return enumStringsLists;
	}
};

/**
* Create a map with the int value as key and the second value as the enum name
*/
static std::vector<EnumValueName> ConvertEnumToMap(std::string enumData)
{
	int textSize = enumData.size();
	// Remove all spaces in the string
	for (int charIndex = 0; charIndex < textSize; charIndex++)
	{
		if (enumData[charIndex] == ' ')
		{
			enumData.erase(enumData.begin() + charIndex);
			charIndex--;
			textSize--;
		}
	}

	std::vector<EnumValueName> myVector;
	int lastEnumNameEndPos = 0;
	bool foundEgals = false;
	int currentValue = -1;
	for (int charIndex = 0; charIndex < textSize; charIndex++)
	{
		if (enumData[charIndex] == '=') // If the enum is defined with a number
		{
			foundEgals = true;
			int endPos = 0;

			// Find the end of the number
			for (int charIndex2 = charIndex; charIndex2 < textSize; charIndex2++)
			{
				if (enumData[charIndex2] == ',')
				{
					endPos = charIndex2;
					break;
				}
			}
			
			std::string stringValue;
			if(endPos == 0)
				stringValue = enumData.substr(charIndex + 1);
			else
				stringValue = enumData.substr(charIndex + 1, endPos - (charIndex + 1));

			// Convert the string to an int
			currentValue = std::stoi(stringValue);

			// Get the enum name and add it to the map
			EnumValueName enumValueName;
			enumValueName.value = currentValue;
			enumValueName.name = enumData.substr(lastEnumNameEndPos, charIndex - lastEnumNameEndPos);
			myVector.push_back(enumValueName);

			lastEnumNameEndPos = charIndex;
		}
		else if (enumData[charIndex] == ',') // If there is another enum
		{
			if (foundEgals)
			{
				foundEgals = false;
				lastEnumNameEndPos = charIndex + 1;
				continue;
			}

			// The enum value is not defined, use the last one and add one
			currentValue++;

			// Get the enum name and add it to the map
			EnumValueName enumValueName;
			enumValueName.value = currentValue;
			enumValueName.name = enumData.substr(lastEnumNameEndPos, charIndex - lastEnumNameEndPos);
			myVector.push_back(enumValueName);

			// Update indexes
			lastEnumNameEndPos = charIndex + 1;
			charIndex += 1;
		}
	}

	// Since the last enum does not have a ',' at the end, add the last enum value
	if (!foundEgals)
	{
		currentValue++;

		EnumValueName enumValueName;
		enumValueName.value = currentValue;
		enumValueName.name = enumData.substr(lastEnumNameEndPos);
		myVector.push_back(enumValueName);
	}
	return myVector;
}


/**
* Register a enum's strings map into a static map that list's all other enum strings map
*/
template<typename T>
static void* RegisterEnumStringsMap(const std::vector<EnumValueName> newEnumStringsList)
{
	uint64_t type = typeid(T).hash_code();
	std::map<uint64_t, std::vector<EnumValueName>>& enumStringsLists = EnumHelper::GetEnumStringsLists();
	enumStringsLists[type] = newEnumStringsList;

	return nullptr;
}

//TODO:
// Replace enum's strings map by a vector to have two enums string with the same int value
// Try to find another way to call RegisterEnumStringsMap without creating a void* variable
/**
* Create an enum that can be used by the editor
*/
#define ENUM(name, ...) \
    enum class name { __VA_ARGS__ }; \
	static const void* name##Register = RegisterEnumStringsMap<name>(ConvertEnumToMap(#__VA_ARGS__));

/**
 * [Internal]
 */
class API ReflectiveEntry
{
public:
	ReflectiveEntry() = default;
	std::optional<VariableReference> variable;
	uint64_t typeId = 0;
	bool visibleInFileInspector = false;
	bool isPublic = false;
	bool isEnum = false;
};

typedef std::unordered_map<std::string, ReflectiveEntry> ReflectiveData;

class API Reflective
{
public:
	/**
	* Get all child class variables references
	*/
	virtual ReflectiveData GetReflectiveData() = 0;

	/**
	* Called when a variable is updated
	*/
	virtual void OnReflectionUpdated() {};


	template<typename T>
	std::enable_if_t<!std::is_pointer<T>::value && !std::is_enum<T>::value, void>
	static AddVariable(ReflectiveData& map, T& value, const std::string& variableName, bool isPublic)
	{
		uint64_t type = typeid(T).hash_code();
		Reflective::AddReflectionVariable(map, value, variableName, false, isPublic, type, false);
	}

	template<typename T, typename = std::enable_if_t<std::is_enum<T>::value>>
	static void AddVariable(ReflectiveData& map, T& value, const std::string& variableName, bool isPublic)
	{
		uint64_t type = typeid(T).hash_code();
		Reflective::AddReflectionVariable(map, (int&)value, variableName, false, isPublic, type, true);
	}

	template<typename T, typename = std::enable_if_t<std::is_base_of<Component, T>::value>>
	static void AddVariable(ReflectiveData& map, std::weak_ptr<T>& value, const std::string& variableName, bool isPublic)
	{
		uint64_t type = typeid(T).hash_code();
		Reflective::AddReflectionVariable(map, (std::weak_ptr<Component>&)value, variableName, false, isPublic, type, false);
	}

	template<typename T, typename = std::enable_if_t<std::is_base_of<Component, T>::value>>
	static void AddVariable(ReflectiveData& map, std::vector<std::weak_ptr<T>>& value, const std::string& variableName, bool isPublic)
	{
		uint64_t type = typeid(T).hash_code();
		Reflective::AddReflectionVariable(map, (std::vector<std::weak_ptr<Component>>&)value, variableName, false, isPublic, type, false);
	}

	//template<typename T>
	//std::enable_if_t<std::is_base_of<Reflective, T>::value, void>
	/*static void AddVariable(ReflectiveData& map, std::vector<int>& value, const std::string& variableName, bool isPublic)
	{
		uint64_t type = typeid(int).hash_code();
		Reflective::AddReflectionVariable(map, value, variableName, false, isPublic, type, false);
	}*/

	template<typename T>
	std::enable_if_t<std::is_base_of<Reflective, T>::value, void>
		static AddVariable(ReflectiveData& map, std::vector<T>& value, const std::string& variableName, bool isPublic)
	{
		uint64_t type = typeid(T).hash_code();
		Reflective::AddReflectionVariable(map, (std::vector<Reflective>&)value, variableName, false, isPublic, type, false);
	}

	template<typename T>
	std::enable_if_t<!std::is_pointer<T>::value, void>
		static AddVariable(ReflectiveData& map, T& value, const std::string& variableName, bool visibleInFileInspector, bool isPublic)
	{
		uint64_t type = typeid(T).hash_code();
		Reflective::AddReflectionVariable(map, value, variableName, visibleInFileInspector, isPublic, type, false);
	}

private:
	static void AddReflectionVariable(ReflectiveData& map, const VariableReference& variable, const std::string& variableName, bool visibleInFileInspector, bool isPublic, uint64_t id, bool isEnum);
};

