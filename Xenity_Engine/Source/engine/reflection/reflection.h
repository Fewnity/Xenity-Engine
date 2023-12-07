#pragma once
#include <engine/api.h>

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

class API ReflectiveEntry
{
public:
	ReflectiveEntry() = default;
	std::optional<VariableReference> variable;
	uint64_t typeId = 0;
	bool visibleInFileInspector = false;
	bool isPublic = false;
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
	std::enable_if_t<!std::is_pointer<T>::value, void>
	static AddVariable(ReflectiveData& map, T& value, const std::string& variableName, bool isPublic)
	{
		uint64_t type = typeid(T).hash_code();
		Reflective::AddReflectionVariable(map, value, variableName, false, isPublic, type);
	}

	template<typename T, typename = std::enable_if_t<std::is_base_of<Component, T>::value>>
	static void AddVariable(ReflectiveData& map, std::weak_ptr<T>& value, const std::string& variableName, bool isPublic)
	{
		uint64_t type = typeid(T).hash_code();
		Reflective::AddReflectionVariable(map, (std::weak_ptr<Component>&)value, variableName, false, isPublic, type);
	}

	template<typename T, typename = std::enable_if_t<std::is_base_of<Component, T>::value>>
	static void AddVariable(ReflectiveData& map, std::vector<std::weak_ptr<T>>& value, const std::string& variableName, bool isPublic)
	{
		uint64_t type = typeid(T).hash_code();
		Reflective::AddReflectionVariable(map, (std::vector<std::weak_ptr<Component>>&)value, variableName, false, isPublic, type);
	}

	//template<typename T>
	//std::enable_if_t<std::is_base_of<Reflective, T>::value, void>
	static void AddVariable(ReflectiveData& map, std::vector<int>& value, const std::string& variableName, bool isPublic)
	{
		uint64_t type = typeid(int).hash_code();
		Reflective::AddReflectionVariable(map, value, variableName, false, isPublic, type);
	}

	template<typename T>
	std::enable_if_t<std::is_base_of<Reflective, T>::value, void>
	static AddVariable(ReflectiveData& map, std::vector<T>& value, const std::string& variableName, bool isPublic)
	{
		uint64_t type = typeid(T).hash_code();
		Reflective::AddReflectionVariable(map, (std::vector<Reflective>&)value, variableName, false, isPublic, type);
	}

	template<typename T>
	std::enable_if_t<!std::is_pointer<T>::value, void>
	static AddVariable(ReflectiveData& map, T& value, const std::string& variableName, bool visibleInFileInspector, bool isPublic)
	{
		uint64_t type = typeid(T).hash_code();
		Reflective::AddReflectionVariable(map, value, variableName, visibleInFileInspector, isPublic, type);
	}

private:
	static void AddReflectionVariable(ReflectiveData& map, const VariableReference& variable, const std::string& variableName, bool visibleInFileInspector, bool isPublic, uint64_t id);
};

