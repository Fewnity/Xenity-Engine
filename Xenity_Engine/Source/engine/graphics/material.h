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
	Material() = delete;
	Material(std::string name);
	~Material();

	void Use();
	void Update();

	Shader* shader = nullptr;
	void SetAttribut(const char* attribut, const Vector2 value);
	void SetAttribut(const char* attribut, const Vector3 value);
	void SetAttribut(const char* attribut, const Vector4 value);
	//void SetAttribut(const char* attribut, Texture* value);
	void SetAttribut(const char* attribut, const float value);
	void SetAttribut(const char* attribut, const int value);
	bool updated = false;
	std::string name = "";
	bool useLighting = false;

private:
	//std::unordered_map <const char *, Texture*> uniformsTextures;
	std::unordered_map <const char*, Vector2> uniformsVector2;
	std::unordered_map <const char*, Vector3> uniformsVector3;
	std::unordered_map <const char*, Vector4> uniformsVector4;
	std::unordered_map <const char*, int> uniformsInt;
	std::unordered_map <const char*, float> uniformsFloat;
};

