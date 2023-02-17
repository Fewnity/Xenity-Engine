#pragma once

#include <unordered_map>
#include <string>

class Texture;
class Shader;
#include "../vectors/vector2.h"
#include "../vectors/vector3.h"
#include "../vectors/vector4.h"

class Material
{
public:
	Material();
	~Material();

	void Use();
	void Update();

	Shader* shader = nullptr;
	void SetAttribut(const std::string attribut, const Vector2 value);
	void SetAttribut(const std::string attribut, const Vector3 value);
	void SetAttribut(const std::string attribut, const Vector4 value);
	void SetAttribut(const std::string attribut, Texture* value);
	void SetAttribut(const std::string attribut, const float value);
	void SetAttribut(const std::string attribut, const int value);
	bool updated = false;

private:
	std::unordered_map <std::string, Texture*> uniformsTextures;
	std::unordered_map <std::string, Vector2> uniformsVector2;
	std::unordered_map <std::string, Vector3> uniformsVector3;
	std::unordered_map <std::string, Vector4> uniformsVector4;
	std::unordered_map <std::string, int> uniformsInt;
	std::unordered_map <std::string, float> uniformsFloat;
};

