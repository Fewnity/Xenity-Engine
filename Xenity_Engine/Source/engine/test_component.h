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

class CustomClass : public Reflection
{
public:
	CustomClass();
	//void SetReflection();
	std::unordered_map<std::string, ReflectionEntry> GetReflection();
	float myCustomFloat = 0;
	float myCustomFloat2 = 0;
};

class TestComponent : public Component
{
public:
	TestComponent();
	~TestComponent();

	//void SetReflection();
	std::unordered_map<std::string, ReflectionEntry> GetReflection() override;
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

	float myFloat = 0;
	int myInt = 0;
	double myDouble = 0;
	std::string myString = "";
};

