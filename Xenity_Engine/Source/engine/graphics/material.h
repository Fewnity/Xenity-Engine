#pragma once

#include <unordered_map>
#include <string>

class Texture;
class Shader;
class Camera;

#include "../vectors/vector2.h"
#include "../vectors/vector3.h"
#include "../vectors/vector4.h"
#include "../file_system/file_reference.h"
#include "../reflection/reflection.h"
#include "iDrawableTypes.h"
#include <memory>

class Material : public FileReference, public Reflection
{
public:
	Material();
	~Material();

	void Use();

	std::unordered_map<std::string, ReflectionEntry> GetReflection() override;
	std::unordered_map<std::string, ReflectionEntry> GetMetaReflection() override;
	void OnReflectionUpdated() override;

	void LoadFileReference() override;

	std::shared_ptr<Shader> shader = nullptr;
	void SetAttribut(const char* attribut, const Vector2& value);
	void SetAttribut(const char* attribut, const Vector3& value);
	void SetAttribut(const char* attribut, const Vector4& value);
	//void SetAttribut(const char* attribut, Texture* value);
	void SetAttribut(const char* attribut, const float& value);
	void SetAttribut(const char* attribut, const int& value);
	bool updated = false;
	bool useLighting = false;

	static std::shared_ptr<Material> MakeMaterial();
	std::weak_ptr<Camera> lastUsedCamera;
	IDrawableTypes lastUpdatedType = Draw_3D;

private:
	void Update();
	//std::unordered_map <const char *, Texture*> uniformsTextures;
	std::unordered_map <const char*, Vector2> uniformsVector2;
	std::unordered_map <const char*, Vector3> uniformsVector3;
	std::unordered_map <const char*, Vector4> uniformsVector4;
	std::unordered_map <const char*, int> uniformsInt;
	std::unordered_map <const char*, float> uniformsFloat;
};

