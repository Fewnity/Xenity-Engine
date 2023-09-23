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

//#define Variable std::variant<std::reference_wrapper<std::weak_ptr<Component>>, std::reference_wrapper<int>, std::reference_wrapper<uint64_t>, std::reference_wrapper<double>, std::reference_wrapper<float>, /*float*,*/ std::reference_wrapper<bool>, std::reference_wrapper<std::string>, std::reference_wrapper<Reflection>, std::reference_wrapper<std::weak_ptr<GameObject>>, std::reference_wrapper<std::weak_ptr<Transform>>, std::reference_wrapper<std::shared_ptr<Texture>>, std::reference_wrapper<std::shared_ptr<MeshData>>, std::reference_wrapper<std::shared_ptr<AudioClip>>, std::reference_wrapper<std::shared_ptr<Scene>>, std::reference_wrapper<std::vector<std::shared_ptr<Texture>>>>

typedef std::variant<
	std::reference_wrapper<std::weak_ptr<Component>>,
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
	std::reference_wrapper<std::vector<std::shared_ptr<Texture>>>> Variable;

class ReflectionEntry 
{
public:
	ReflectionEntry() { }
	std::optional<Variable> variable;
	bool visibleInFileInspector = false;
	bool isPublic = false;
};


class Reflection
{
public:
	//std::unordered_map<std::string, Variable> reflectedVariables;
	//virtual void SetReflection() = 0;

	/**
	* Get all child class variables references
	*/
	virtual std::unordered_map<std::string, ReflectionEntry> GetReflection() = 0;

	/**
	* Called when a variable is updated
	*/
	virtual void OnReflectionUpdated() {};

	static void AddReflectionVariable(std::unordered_map<std::string, ReflectionEntry> &map, Variable variable, std::string variableName, bool isPublic);
	static void AddReflectionVariable(std::unordered_map<std::string, ReflectionEntry> &map, Variable variable, std::string variableName, bool visibleInFileInspector, bool isPublic);
};

