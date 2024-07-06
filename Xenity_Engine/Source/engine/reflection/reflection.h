// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

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
#include <json.hpp>
#include <engine/tools/template_utils.h>

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
class Icon;

// List of all the types that can be used in the reflection system (visible in the inspector and saved to json)
typedef std::variant <
	// Basic types
	std::reference_wrapper<int>,
	std::reference_wrapper<uint64_t>,
	std::reference_wrapper<double>,
	std::reference_wrapper<float>,
	std::reference_wrapper<bool>,
	std::reference_wrapper<std::string>,
	std::reference_wrapper<nlohmann::json>,
	std::reference_wrapper<Reflective>,

	// components/game elements
	std::reference_wrapper<std::weak_ptr<Component>>,
	std::reference_wrapper<std::weak_ptr<Collider>>,
	std::reference_wrapper<std::weak_ptr<GameObject>>,

	// Files
	std::reference_wrapper<std::weak_ptr<Transform>>,
	std::reference_wrapper<std::shared_ptr<Texture>>,
	std::reference_wrapper<std::shared_ptr<MeshData>>,
	std::reference_wrapper<std::shared_ptr<AudioClip>>,
	std::reference_wrapper<std::shared_ptr<Scene>>,
	std::reference_wrapper<std::shared_ptr<SkyBox>>,
	std::reference_wrapper<std::shared_ptr<Font>>,
	std::reference_wrapper<std::shared_ptr<Shader>>,
	std::reference_wrapper<std::shared_ptr<Material>>,
	std::reference_wrapper<std::shared_ptr<Icon>>,

	//List of basic types
	std::reference_wrapper<std::vector<Reflective*>>,
	std::reference_wrapper<std::vector<int>>,
	std::reference_wrapper<std::vector<float>>,
	std::reference_wrapper<std::vector<uint64_t>>,
	std::reference_wrapper<std::vector<double>>,
	std::reference_wrapper<std::vector<std::string>>,

	//List of files
	std::reference_wrapper<std::vector<std::shared_ptr<Texture>>>,
	std::reference_wrapper<std::vector<std::shared_ptr<MeshData>>>,
	std::reference_wrapper<std::vector<std::shared_ptr<AudioClip>>>,
	std::reference_wrapper<std::vector<std::shared_ptr<Scene>>>,
	std::reference_wrapper<std::vector<std::shared_ptr<SkyBox>>>,
	std::reference_wrapper<std::vector<std::shared_ptr<Font>>>,
	std::reference_wrapper<std::vector<std::shared_ptr<Shader>>>,
	std::reference_wrapper<std::vector<std::shared_ptr<Material>>>,
	std::reference_wrapper<std::vector<std::shared_ptr<Icon>>>,

	// List of components/game elements
	std::reference_wrapper<std::vector<std::weak_ptr<GameObject>>>,
	std::reference_wrapper<std::vector<std::weak_ptr<Transform>>>,
	std::reference_wrapper<std::vector<std::weak_ptr<Component>>>> VariableReference;



class TypeSpawner
{
public:
	virtual ~TypeSpawner() {}
	virtual void* Allocate() const = 0;
};

template<typename T> class TypeSpawnerImpl : public TypeSpawner
{
public:
	/**
	* @brief Allocate a new instance of the type
	*/
	virtual void* Allocate()const { return new T; }
};

/**
 * @brief [Internal]
 */
class API ReflectiveEntry
{
public:
	ReflectiveEntry() = default;
	std::optional<VariableReference> variable;
	uint64_t typeId = 0;
	TypeSpawner* typeSpawner = nullptr;
	std::string variableName;
	bool visibleInFileInspector = false;
	bool isPublic = false;
	bool isEnum = false;
};

typedef std::vector<ReflectiveEntry> ReflectiveData;

/**
* @brief Class to inherit if you want to access child's variables to fill variables from json or save them to json
*/
class API Reflective
{
public:
	virtual ~Reflective() = default;

	/**
	* @brief Get all child class variables references
	*/
	virtual ReflectiveData GetReflectiveData() = 0;

	/**
	* @brief Called when a variable is updated
	*/
	virtual void OnReflectionUpdated() {};

protected:

	/**
	* @brief Add a variable to the list of variables (basic type)
	* @param vector The list of variables
	* @param value The variable value
	* @param variableName The variable name
	* @param isPublic If the variable is public
	*/
	template<typename T>
	std::enable_if_t<!std::is_pointer<T>::value && !std::is_enum<T>::value, void>
	static AddVariable(ReflectiveData& vector, T& value, const std::string& variableName, const bool isPublic)
	{
		const uint64_t type = typeid(T).hash_code();
		Reflective::CreateReflectionEntry(vector, value, variableName, false, isPublic, type, false);
	}

	template<typename T, typename = std::enable_if_t<std::is_enum<T>::value>>
	static void AddVariable(ReflectiveData& vector, std::vector<T>& value, const std::string& variableName, const bool isPublic)
	{
		const uint64_t type = typeid(T).hash_code();
		Reflective::CreateReflectionEntry(vector, reinterpret_cast<std::vector<int>&>(value), variableName, false, isPublic, type, true);
	}

	/**
	* @brief Add a variable to the list of variables (enum)
	* @param vector The list of variables
	* @param value The variable value
	* @param variableName The variable name
	* @param isPublic If the variable is public
	*/
	template<typename T, typename = std::enable_if_t<std::is_enum<T>::value>>
	static void AddVariable(ReflectiveData& vector, T& value, const std::string& variableName, const bool isPublic)
	{
		const uint64_t type = typeid(T).hash_code();
		Reflective::CreateReflectionEntry(vector, reinterpret_cast<int&>(value), variableName, false, isPublic, type, true);
	}

	/**
	* @brief Add a variable to the list of variables (component)
	* @param vector The list of variables
	* @param value The variable value
	* @param variableName The variable name
	* @param isPublic If the variable is public
	*/
	template<typename T, typename = std::enable_if_t<std::is_base_of<Component, T>::value>>
	static void AddVariable(ReflectiveData& vector, std::weak_ptr<T>& value, const std::string& variableName, const bool isPublic)
	{
		const uint64_t type = typeid(T).hash_code();
		Reflective::CreateReflectionEntry(vector, reinterpret_cast<std::weak_ptr<Component>&>(value), variableName, false, isPublic, type, false);
	}
	
	/**
	* @brief Add a variable to the list of variables (component list)
	* @param vector The list of variables
	* @param value The variable value
	* @param variableName The variable name
	* @param isPublic If the variable is public
	*/
	template<typename T, typename = std::enable_if_t<std::is_base_of<Component, T>::value>>
	static void AddVariable(ReflectiveData& vector, std::vector<std::weak_ptr<T>>& value, const std::string& variableName, const bool isPublic)
	{
		const uint64_t type = typeid(T).hash_code();
		Reflective::CreateReflectionEntry(vector, reinterpret_cast<std::vector<std::weak_ptr<Component>>&>(value), variableName, false, isPublic, type, false);
	}

	/**
	* @brief Add a variable to the list of variables (reflective list)
	* @param vector The list of variables
	* @param value The variable value
	* @param variableName The variable name
	* @param isPublic If the variable is public
	*/
	template<typename T>
	std::enable_if_t<std::is_base_of<Reflective, T>::value, void>
	static AddVariable(ReflectiveData& vector, std::vector<T*>& value, const std::string& variableName, const bool isPublic)
	{
		const uint64_t type = typeid(T).hash_code();
		Reflective::CreateReflectionEntry(vector, reinterpret_cast<std::vector<Reflective*>&>(value), variableName, false, isPublic, type, false);
		for (ReflectiveEntry& otherEntry : vector)
		{
			if (otherEntry.variableName == variableName) 
			{
				otherEntry.typeSpawner = new TypeSpawnerImpl<T>();
				break;
			}
		}
	}

	/**
	* @brief Add a variable to the list of variables (basic type)
	* @param map The list of variables
	* @param value The variable value
	* @param variableName The variable name
	* @param visibleInFileInspector If the variable is visible in the file inspector
	* @param isPublic If the variable is public
	*/
	template<typename T>
	std::enable_if_t<!std::is_pointer<T>::value, void>
	static AddVariable(ReflectiveData& map, T& value, const std::string& variableName, const bool visibleInFileInspector, const bool isPublic)
	{
		const uint64_t type = typeid(T).hash_code();
		Reflective::CreateReflectionEntry(map, value, variableName, visibleInFileInspector, isPublic, type, false);
	}

private:
	/**
	* @brief Create a reflection entry
	* @param vector The list of variables
	* @param variable The variable value
	* @param variableName The variable name
	* @param visibleInFileInspector If the variable is visible in the file inspector
	* @param isPublic If the variable is public
	* @param id The variable type id (hash code)
	* @param isEnum If the variable is an enum
	*/
	static void CreateReflectionEntry(ReflectiveData& vector, const VariableReference& variable, const std::string& variableName, const bool visibleInFileInspector, const bool isPublic, const uint64_t id, const bool isEnum);
};

