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

typedef std::variant<
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
	std::reference_wrapper<std::vector<int>>,
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

//class EnumInfos
//{
//public:
//	std::vector<std::string>* names = nullptr;
//};
class EnumHelper
{
public:
	static std::map<uint64_t, std::map<int, std::string>>& GetMyVectors() {
		static std::map<uint64_t, std::map<int, std::string>> enumStringsList;
		std::cout << &enumStringsList << std::endl;
		return enumStringsList;
	}
};
//static std::map <uint64_t, std::vector<std::string>> enumStringsList;

static std::map<int, std::string> convertToVector(std::string test)
{
	std::map<int, std::string> myVector;

	int size = test.size();
	int lastEnd = 0;
	bool foundEgals = false;
	for (int i = 0; i < size; i++)
	{
		if (test[i] == ' ')
		{
			test.erase(test.begin() + i);
			i--;
			size--;
		}
	}
	int currentValue = -1;
	for (int i = 0; i < size; i++)
	{
		if (test[i] == '=')
		{
			foundEgals = true;
			int endPos = 0;
			for (int j = i; j < size; j++)
			{
				if (test[j] == ',') 
				{
					endPos = j;
					break;
				}
			}
			std::string t = test.substr(i+1, endPos - (i+1));
			currentValue = std::stoi(t);
			//currentValue++;
			myVector[currentValue] = test.substr(lastEnd, i - lastEnd);

			lastEnd = i;
		}
		if (test[i] == ',')
		{
			if (foundEgals)
			{
				foundEgals = false;
				lastEnd = i + 1;
				continue;
			}
			currentValue++;
			myVector[currentValue] = test.substr(lastEnd, i - lastEnd);
			lastEnd = i + 1;
			//i += 1;
		}
	}
	if (!foundEgals)
	{
		currentValue++;
		myVector[currentValue] = test.substr(lastEnd);
	}
	return myVector;
}


template<typename T>
static void* test(const std::map<int, std::string> ref)
{
	uint64_t type = typeid(T).hash_code();
	{
		std::map<uint64_t, std::map<int, std::string>>& tt = EnumHelper::GetMyVectors();
		tt[type] = ref;
	}

	return nullptr;
}

#define ENUM_MACRO(name, ...) \
    static enum class name { __VA_ARGS__ }; \
	static const void* name##RegisterTrick = test<name>(convertToVector(#__VA_ARGS__));

/**
 * [Internal]
 */
class API ReflectiveEntry
{
public:
	ReflectiveEntry() = default;
	/*~ReflectiveEntry() 
	{
		if (enumInfos)
			delete enumInfos;
	}*/
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
	static void AddVariable(ReflectiveData& map, std::vector<int>& value, const std::string& variableName, bool isPublic)
	{
		uint64_t type = typeid(int).hash_code();
		Reflective::AddReflectionVariable(map, value, variableName, false, isPublic, type, false);
	}

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

