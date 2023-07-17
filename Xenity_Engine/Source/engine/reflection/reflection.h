#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <variant>

class GameObject;
class Transform;
class Color;
class Texture;
class MeshData;
class Reflection;

#define Variable std::variant<int*, double*, float*, bool*, std::string*, Reflection*, void*, std::weak_ptr<GameObject>*, std::weak_ptr<Transform>*, Texture**, MeshData**>

class Reflection
{
public:
	std::unordered_map<std::string, Variable> reflectedVariables;
	virtual void SetReflection() = 0;
};

