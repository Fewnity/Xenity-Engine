#pragma once

#include <string>

#include "component.h"

#include "lighting/lighting.h"
#include "vectors/vector2.h"
#include "vectors/vector2_int.h"
#include "vectors/vector3.h"
#include "vectors/vector4.h"
#include "graphics/color/color.h"

class MeshRenderer;
class Texture;
class MeshData;
class AudioClip;
class Scene;
class SkyBox;
class Font;
class Shader;
class Material;

class CustomClass : public Reflective
{
public:
	CustomClass();
	ReflectiveData GetReflectiveData();
	float myCustomFloat = 0;
	float myCustomFloat2 = 0;
};

class TestComponent : public Component
{
public:
	TestComponent();
	~TestComponent();

	ReflectiveData GetReflectiveData() override;
	void Start() override;
	void Update() override;

	CustomClass myCustomClass = CustomClass();

	std::weak_ptr<MeshRenderer> myComponent;
	std::weak_ptr<GameObject> myGameObject;
	std::weak_ptr<Transform> myTransform;

	Vector2 vec2 = Vector2(0);
	Vector2Int vec2Int = Vector2Int(0);
	Vector3 vec3 = Vector3(0);
	Vector3 vec3_2 = Vector3(0);
	Vector3 vec3_3 = Vector3(0);
	Vector3* vec3_32 = nullptr;
	Vector4 vec4 = Vector4(0);

	Color color;
	std::shared_ptr <SkyBox> skyBox;

	std::vector <std::shared_ptr<Texture>> textures;
	std::vector <std::shared_ptr<MeshData>> meshData;
	std::vector <std::shared_ptr<AudioClip>> audioClips;
	std::vector <std::shared_ptr<Scene>> scenes;
	std::vector <std::shared_ptr<SkyBox>> skyBoxes;
	std::vector <std::shared_ptr<Font>> fonts;
	std::vector <std::shared_ptr<Shader>> shaders;
	std::vector <std::shared_ptr<Material>> materials;

	std::vector <std::weak_ptr<MeshRenderer>> myComponents;
	std::vector <std::weak_ptr<GameObject>> myGameObjects;
	std::vector <std::weak_ptr<Transform>> myTransforms;

	float myFloat = 0;
	int myInt = 0;
	double myDouble = 0;
	std::string myString = "";
	bool myBool = false;
};

