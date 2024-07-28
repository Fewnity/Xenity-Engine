// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include "component.h"

#include <engine/lighting/lighting.h>
#include <engine/graphics/color/color.h>

#include <engine/vectors/vector2.h>
#include <engine/vectors/vector2_int.h>
#include <engine/vectors/vector3.h>
#include <engine/vectors/vector4.h>
#include <engine/vectors/quaternion.h>

#include <string>


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

ENUM(Matos, Clavier, Souris, Ecran);
ENUM(Colors, Blue = 5, Red = 145, Orange = 1203, Yellow = 145, Green = 145, Purple = 5);

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
	Quaternion quaternion = Quaternion::Identity();

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

	std::vector <Vector2*> myVectors2;
	std::vector <Vector2Int*> myVectors2Int;
	std::vector <Vector3*> myVectors3;
	std::vector <Vector4*> myVectors4;
	std::vector <CustomClass*> myCustomClasses;
	std::vector <int> myInts;
	std::vector <float> myFloats;
	std::vector <uint64_t> myUint64s;
	std::vector <double> myDoubles;
	std::vector <std::string> myStrings;
	std::vector <Matos> myMatoslist;
	std::vector <Color*> myColors;

	float myFloat = 0;
	int myInt = 0;
	double myDouble = 0;
	std::string myString = "";
	bool myBool = false;
	Colors myEnum = Colors::Red;
	Matos myMatos = Matos::Clavier;
};

