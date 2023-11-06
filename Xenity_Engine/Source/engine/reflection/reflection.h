#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <variant>
#include <vector>
#include <optional>

class GameObject;
class Transform;
class Color;
class Texture;
class MeshData;
class Reflection;
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
	std::reference_wrapper<Reflection>,
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

	std::reference_wrapper<std::vector<Reflection>>,
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
	std::reference_wrapper<std::vector<std::weak_ptr<Component>>>> Variable;

class API ReflectionEntry
{
public:
	ReflectionEntry() = default;
	std::optional<Variable> variable;
	uint64_t typeId = 0;
	bool visibleInFileInspector = false;
	bool isPublic = false;
};


class API Reflection
{
public:
	/**
	* Get all child class variables references
	*/
	virtual std::unordered_map<std::string, ReflectionEntry> GetReflection() = 0;

	/**
	* Called when a variable is updated
	*/
	virtual void OnReflectionUpdated() {};


	template<typename T>
	std::enable_if_t<!std::is_pointer<T>::value, void>
	static AddVariable(std::unordered_map<std::string, ReflectionEntry>& map, T& value, const std::string& variableName, bool isPublic)
	{
		uint64_t type = typeid(T).hash_code();
		Reflection::AddReflectionVariable(map, value, variableName, false, isPublic, type);
	}


	template<typename T, typename = std::enable_if_t<std::is_base_of<Component, T>::value>>
	static void AddVariable(std::unordered_map<std::string, ReflectionEntry>& map, std::weak_ptr<T>& value, const std::string& variableName, bool isPublic)
	{
		uint64_t type = typeid(T).hash_code();
		Reflection::AddReflectionVariable(map, (std::weak_ptr<Component>&)value, variableName, false, isPublic, type);
	}

	template<typename T, typename = std::enable_if_t<std::is_base_of<Component, T>::value>>
	static void AddVariable(std::unordered_map<std::string, ReflectionEntry>& map, std::vector<std::weak_ptr<T>>& value, const std::string& variableName, bool isPublic)
	{
		uint64_t type = typeid(T).hash_code();
		Reflection::AddReflectionVariable(map, (std::vector<std::weak_ptr<Component>>&)value, variableName, false, isPublic, type);
	}

	//template<typename T>
	//std::enable_if_t<std::is_base_of<Reflection, T>::value, void>
	static void AddVariable(std::unordered_map<std::string, ReflectionEntry>& map, std::vector<int>& value, const std::string& variableName, bool isPublic)
	{
		uint64_t type = typeid(int).hash_code();
		Reflection::AddReflectionVariable(map, value, variableName, false, isPublic, type);
	}

	template<typename T>
	std::enable_if_t<std::is_base_of<Reflection, T>::value, void>
	static AddVariable(std::unordered_map<std::string, ReflectionEntry>& map, std::vector<T>& value, const std::string& variableName, bool isPublic)
	{
		uint64_t type = typeid(T).hash_code();
		Reflection::AddReflectionVariable(map, (std::vector<Reflection>&)value, variableName, false, isPublic, type);
	}

	template<typename T>
	std::enable_if_t<!std::is_pointer<T>::value, void>
	static AddVariable(std::unordered_map<std::string, ReflectionEntry>& map, T& value, const std::string& variableName, bool visibleInFileInspector, bool isPublic)
	{
		uint64_t type = typeid(T).hash_code();
		Reflection::AddReflectionVariable(map, value, variableName, visibleInFileInspector, isPublic, type);
	}

private:
	static void AddReflectionVariable(std::unordered_map<std::string, ReflectionEntry>& map, const Variable& variable, const std::string& variableName, bool visibleInFileInspector, bool isPublic, uint64_t id);
};

