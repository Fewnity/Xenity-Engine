#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <variant>
#include <vector>

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

#define Variable std::variant<std::reference_wrapper<std::weak_ptr<Component>>, std::reference_wrapper<int>, std::reference_wrapper<uint64_t>, std::reference_wrapper<double>, std::reference_wrapper<float>, /*float*,*/ std::reference_wrapper<bool>, std::reference_wrapper<std::string>, std::reference_wrapper<Reflection>, std::reference_wrapper<std::weak_ptr<GameObject>>, std::reference_wrapper<std::weak_ptr<Transform>>, std::reference_wrapper<Texture*>, std::reference_wrapper<MeshData*>, std::reference_wrapper<AudioClip*>, std::reference_wrapper<Scene*>, std::reference_wrapper<std::vector<Texture*>>>

class Reflection
{
public:
	//std::unordered_map<std::string, Variable> reflectedVariables;
	//virtual void SetReflection() = 0;

	/**
	* Get all child class variables references
	*/
	virtual std::unordered_map<std::string, Variable> GetReflection() = 0;

	/**
	* Called when a variable is updated
	*/
	virtual void OnReflectionUpdated() {};
};

