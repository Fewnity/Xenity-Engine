#pragma once

#include <unordered_map>
#include <string>

class Texture;
class Shader;
class Camera;

// Vectors includes needed for other platforms
#include <engine/vectors/vector2.h>
#include <engine/vectors/vector3.h>
#include <engine/vectors/vector4.h>
//

#include <engine/file_system/file_reference.h>
#include <engine/reflection/reflection.h>
#include "iDrawableTypes.h"
#include <memory>

class Material : public FileReference, public Reflective
{
public:
	Material();
	~Material();

	// [Internal]
	void Use();

	ReflectiveData GetReflectiveData() override;
	ReflectiveData GetMetaReflectiveData() override;
	void OnReflectionUpdated() override;

	void LoadFileReference() override;

	std::shared_ptr<Shader> shader = nullptr;
	void SetAttribute(const char* attribute, const Vector2& value);
	void SetAttribute(const char* attribute, const Vector3& value);
	void SetAttribute(const char* attribute, const Vector4& value);
	//void SetAttribute(const char* attribut, Texture* value);
	void SetAttribute(const char* attribute, const float value);
	void SetAttribute(const char* attribute, const int value);

	static std::shared_ptr<Material> MakeMaterial();

	// [Internal]
	bool updated = false;
	bool useLighting = false;
private:
	void Update();
	std::weak_ptr<Camera> lastUsedCamera;
	IDrawableTypes lastUpdatedType = IDrawableTypes::Draw_3D;
	//std::unordered_map <const char *, Texture*> uniformsTextures;
	std::unordered_map <const char*, Vector2> uniformsVector2;
	std::unordered_map <const char*, Vector3> uniformsVector3;
	std::unordered_map <const char*, Vector4> uniformsVector4;
	std::unordered_map <const char*, int> uniformsInt;
	std::unordered_map <const char*, float> uniformsFloat;
};

