// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <variant>
#include <optional>
#include <json_fwd.hpp>

#include <engine/api.h>
#include <engine/tools/template_utils.h>
#include <engine/assertions/assertions.h>

class GameObject;
class Transform;
class Color;
class Reflective;
class Component;
class FileReference;
class Collider;

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
	std::reference_wrapper<std::weak_ptr<Transform>>,

	// Files
	std::reference_wrapper<std::shared_ptr<FileReference>>,

	//List of basic types
	std::reference_wrapper<std::vector<Reflective*>>,
	std::reference_wrapper<std::vector<int>>,
	std::reference_wrapper<std::vector<float>>,
	std::reference_wrapper<std::vector<uint64_t>>,
	std::reference_wrapper<std::vector<double>>,
	std::reference_wrapper<std::vector<std::string>>,

	//List of files
	std::reference_wrapper<std::vector<std::shared_ptr<FileReference>>>,

	// List of components/game elements
	std::reference_wrapper<std::vector<std::weak_ptr<GameObject>>>,
	std::reference_wrapper<std::vector<std::weak_ptr<Transform>>>,
	std::reference_wrapper<std::vector<std::weak_ptr<Component>>>> VariableReference;



class TypeSpawner
{
public:
	virtual ~TypeSpawner() {}
	[[nodiscard]] virtual void* Allocate() const = 0;
};

template<typename T> class TypeSpawnerImpl : public TypeSpawner
{
public:
	/**
	* @brief Allocate a new instance of the type
	*/
	[[nodiscard]] virtual void* Allocate()const { return new T; }
};

/**
 * @brief [Internal]
 */
struct API ReflectiveEntry
{
	std::optional<VariableReference> variable;
	uint64_t typeId = 0;
	TypeSpawner* typeSpawner = nullptr;
	std::string variableName;
	double minSliderValue = 0.0;
	double maxSliderValue = 0.0;
	bool visibleInFileInspector = false;
	bool isPublic = false;
	bool isEnum = false;
	bool isSlider = false;
};

typedef std::vector<ReflectiveEntry> ReflectiveData;

#define BEGIN_REFLECTION() ReflectiveData reflectedVariables
#define ADD_VARIABLE(variable, isPublic) Reflective::AddVariable(reflectedVariables, variable, #variable, isPublic)
#define ADD_VARIABLE_CUSTOM(variable, name, isPublic) Reflective::AddVariable(reflectedVariables, variable, name, isPublic)
#define END_REFLECTION() return reflectedVariables

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
	[[nodiscard]] virtual ReflectiveData GetReflectiveData() = 0;

	/**
	* @brief Called when one or more variables have been updated by the inspector or by the scene manager
	*/
	virtual void OnReflectionUpdated() {};

protected:

	// ----------------------------------------------------------------------------------------
	// ---------------------------------------------------------------------------- BASIC TYPES
	// ----------------------------------------------------------------------------------------

	/**
	* @brief Add a variable to the list of variables (Basic types)
	* @param vector The list of variables
	* @param value The variable value
	* @param variableName The variable name
	* @param isPublic If the variable is public
	*/
	template<typename T>
	std::enable_if_t<!std::is_pointer<T>::value && !std::is_enum<T>::value && !std::is_base_of<Reflective, T>::value, ReflectiveEntry&>
		static AddVariable(ReflectiveData& vector, T& value, const std::string& variableName, const bool isPublic)
	{
		XASSERT(!variableName.empty(), "[Reflective::AddVariable] variableName is empty");

		static const uint64_t type = typeid(T).hash_code();
		ReflectiveEntry& newReflectiveEntry = Reflective::CreateReflectionEntry(vector, std::reference_wrapper<T>(value), variableName, false, isPublic, type, false);
		return newReflectiveEntry;
	}

	// ----------------------------------------------------------------------------------------
	// ---------------------------------------------------------------------------- BASIC REFLECTIVES
	// ----------------------------------------------------------------------------------------

	/**
	* @brief Add a variable to the list of variables (Basic Reflectives)
	* @param vector The list of variables
	* @param value The variable value
	* @param variableName The variable name
	* @param isPublic If the variable is public
	*/
	template<typename T>
	std::enable_if_t<std::is_base_of<Reflective, T>::value, ReflectiveEntry&>
	static AddVariable(ReflectiveData& vector, T& value, const std::string& variableName, const bool isPublic)
	{
		XASSERT(!variableName.empty(), "[Reflective::AddVariable] variableName is empty");

		static const uint64_t type = typeid(T).hash_code();
		ReflectiveEntry& newReflectiveEntry = Reflective::CreateReflectionEntry(vector, std::reference_wrapper<Reflective>(value), variableName, false, isPublic, type, false);
		return newReflectiveEntry;
	}

	/**
	* @brief Add a variable to the list of variables (Basic reflectives list)
	* @param vector The list of variables
	* @param value The variable value
	* @param variableName The variable name
	* @param isPublic If the variable is public
	*/
	template<typename T>
	std::enable_if_t<std::is_base_of<Reflective, T>::value, ReflectiveEntry&>
		static AddVariable(ReflectiveData& vector, std::vector<T*>& value, const std::string& variableName, const bool isPublic)
	{
		XASSERT(!variableName.empty(), "[Reflective::AddVariable] variableName is empty");

		static const uint64_t type = typeid(T).hash_code();
		ReflectiveEntry& newReflectiveEntry = Reflective::CreateReflectionEntry(vector, reinterpret_cast<std::vector<Reflective*>&>(value), variableName, false, isPublic, type, false);
		for (ReflectiveEntry& otherEntry : vector)
		{
			if (otherEntry.variableName == variableName)
			{
				otherEntry.typeSpawner = new TypeSpawnerImpl<T>();
				break;
			}
		}
		return newReflectiveEntry;
	}

	// ----------------------------------------------------------------------------------------
	// ---------------------------------------------------------------------------- ENUMS
	// ----------------------------------------------------------------------------------------

	/**
	* @brief Add a variable to the list of variables (Enums)
	* @param vector The list of variables
	* @param value The variable value
	* @param variableName The variable name
	* @param isPublic If the variable is public
	*/
	template<typename T, typename = std::enable_if_t<std::is_enum<T>::value>>
	static ReflectiveEntry& AddVariable(ReflectiveData& vector, T& value, const std::string& variableName, const bool isPublic)
	{
		XASSERT(!variableName.empty(), "[Reflective::AddVariable] variableName is empty");

		static const uint64_t type = typeid(T).hash_code();
		ReflectiveEntry& newReflectiveEntry = Reflective::CreateReflectionEntry(vector, std::reference_wrapper<int>((int&)value), variableName, false, isPublic, type, true);
		return newReflectiveEntry;
	}

	/**
	* @brief Add a variable to the list of variables (Enums list)
	* @param vector The list of variables
	* @param value The variable value
	* @param variableName The variable name
	* @param isPublic If the variable is public
	*/
	template<typename T, typename = std::enable_if_t<std::is_enum<T>::value>>
	static ReflectiveEntry& AddVariable(ReflectiveData& vector, std::vector<T>& value, const std::string& variableName, const bool isPublic)
	{
		XASSERT(!variableName.empty(), "[Reflective::AddVariable] variableName is empty");

		static const uint64_t type = typeid(T).hash_code();
		ReflectiveEntry& newReflectiveEntry = Reflective::CreateReflectionEntry(vector, reinterpret_cast<std::vector<int>&>(value), variableName, false, isPublic, type, true);
		return newReflectiveEntry;
	}

	// ----------------------------------------------------------------------------------------
	// ---------------------------------------------------------------------------- COMPONENTS
	// ----------------------------------------------------------------------------------------

	/**
	* @brief Add a variable to the list of variables (Components)
	* @param vector The list of variables
	* @param value The variable value
	* @param variableName The variable name
	* @param isPublic If the variable is public
	*/
	template<typename T, typename = std::enable_if_t<std::is_base_of<Component, T>::value>>
	static ReflectiveEntry& AddVariable(ReflectiveData& vector, std::weak_ptr<T>& value, const std::string& variableName, const bool isPublic)
	{
		XASSERT(!variableName.empty(), "[Reflective::AddVariable] variableName is empty");

		static const uint64_t type = typeid(T).hash_code();
		ReflectiveEntry& newReflectiveEntry = Reflective::CreateReflectionEntry(vector, reinterpret_cast<std::weak_ptr<Component>&>(value), variableName, false, isPublic, type, false);
		return newReflectiveEntry;
	}

	/**
	* @brief Add a variable to the list of variables (Components list)
	* @param vector The list of variables
	* @param value The variable value
	* @param variableName The variable name
	* @param isPublic If the variable is public
	*/
	template<typename T, typename = std::enable_if_t<std::is_base_of<Component, T>::value>>
	static ReflectiveEntry& AddVariable(ReflectiveData& vector, std::vector<std::weak_ptr<T>>& value, const std::string& variableName, const bool isPublic)
	{
		XASSERT(!variableName.empty(), "[Reflective::AddVariable] variableName is empty");

		static const uint64_t type = typeid(T).hash_code();
		ReflectiveEntry& newReflectiveEntry = Reflective::CreateReflectionEntry(vector, reinterpret_cast<std::vector<std::weak_ptr<Component>>&>(value), variableName, false, isPublic, type, false);
		return newReflectiveEntry;
	}

	// ----------------------------------------------------------------------------------------
	// ---------------------------------------------------------------------------- FILES
	// ----------------------------------------------------------------------------------------

	/**
	* @brief Add a variable to the list of variables (FileReference)
	* @param vector The list of variables
	* @param value The variable value
	* @param variableName The variable name
	* @param isPublic If the variable is public
	*/
	template<typename T, typename = std::enable_if_t<std::is_base_of<FileReference, T>::value>>
	static ReflectiveEntry& AddVariable(ReflectiveData& vector, std::shared_ptr<T>& value, const std::string& variableName, const bool isPublic)
	{
		XASSERT(!variableName.empty(), "[Reflective::AddVariable] variableName is empty");

		static const uint64_t type = typeid(T).hash_code();
		ReflectiveEntry& newReflectiveEntry = Reflective::CreateReflectionEntry(vector, reinterpret_cast<std::shared_ptr<FileReference>&>(value), variableName, false, isPublic, type, false);
		return newReflectiveEntry;
	}

	/**
	* @brief Add a variable to the list of variables (FileReferences list)
	* @param vector The list of variables
	* @param value The variable value
	* @param variableName The variable name
	* @param isPublic If the variable is public
	*/
	template<typename T, typename = std::enable_if_t<std::is_base_of<FileReference, T>::value>>
	static ReflectiveEntry& AddVariable(ReflectiveData& vector, std::vector<std::shared_ptr<T>>& value, const std::string& variableName, const bool isPublic)
	{
		XASSERT(!variableName.empty(), "[Reflective::AddVariable] variableName is empty");

		static const uint64_t type = typeid(T).hash_code();
		ReflectiveEntry& newReflectiveEntry = Reflective::CreateReflectionEntry(vector, reinterpret_cast<std::vector<std::shared_ptr<FileReference>>&>(value), variableName, false, isPublic, type, false);
		return newReflectiveEntry;
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
	[[nodiscard]] static ReflectiveEntry& CreateReflectionEntry(ReflectiveData& vector, const VariableReference& variable, const std::string& variableName, const bool visibleInFileInspector, const bool isPublic, const uint64_t id, const bool isEnum);
};

