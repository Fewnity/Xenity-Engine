// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "test_component.h"

#include <memory>

#include <engine/asset_management/asset_manager.h>
#include <engine/debug/debug.h>
#include <engine/game_elements/gameobject.h>
#include <engine/game_elements/transform.h>
#include <engine/graphics/3d_graphics/mesh_renderer.h>
#include <engine/graphics/3d_graphics/mesh_data.h>
#include <engine/graphics/skybox.h>
#include <engine/graphics/texture/texture.h>
#include <engine/graphics/material.h>
#include <engine/graphics/shader/shader.h>
#include <engine/graphics/ui/font.h>
#include <engine/audio/audio_clip.h>
#include <engine/scene_management/scene.h>

TestComponent::TestComponent()
{
	AssetManager::AddReflection(this);
}

TestComponent::~TestComponent()
{
	AssetManager::RemoveReflection(this);
}

ReflectiveData TestComponent::GetReflectiveData()
{
	ReflectiveData reflectedVariables;

	Reflective::AddVariable(reflectedVariables, myCustomClass, "myCustomClass", true);

	Reflective::AddVariable(reflectedVariables, myComponent, "myComponent", true);
	Reflective::AddVariable(reflectedVariables, myGameObject, "myGameObject", true);
	Reflective::AddVariable(reflectedVariables, myTransform, "myTransform", true);

	Reflective::AddVariable(reflectedVariables, vec2, "vec2", true);
	Reflective::AddVariable(reflectedVariables, vec2Int, "vec2Int", true);
	Reflective::AddVariable(reflectedVariables, vec3, "vec3", true);
	Reflective::AddVariable(reflectedVariables, vec3_2, "vec3_2", true);
	Reflective::AddVariable(reflectedVariables, vec3_3, "vec3_3", true);
	Reflective::AddVariable(reflectedVariables, vec4, "vec4", true);
	Reflective::AddVariable(reflectedVariables, quaternion, "quaternion", true);

	Reflective::AddVariable(reflectedVariables, color, "color", true);

	Reflective::AddVariable(reflectedVariables, myFloat, "myFloat", true);
	Reflective::AddVariable(reflectedVariables, myInt, "myInt", true);
	Reflective::AddVariable(reflectedVariables, myDouble, "myDouble", true);
	Reflective::AddVariable(reflectedVariables, myString, "myString", true);
	Reflective::AddVariable(reflectedVariables, myBool, "myBool", true);

	Reflective::AddVariable(reflectedVariables, skyBox, "skyBox", true);

	Reflective::AddVariable(reflectedVariables, textures, "textures", true);
	Reflective::AddVariable(reflectedVariables, meshData, "meshData", true);
	Reflective::AddVariable(reflectedVariables, audioClips, "audioClips", true);
	Reflective::AddVariable(reflectedVariables, scenes, "scenes", true);
	Reflective::AddVariable(reflectedVariables, skyBoxes, "skyBoxes", true);
	Reflective::AddVariable(reflectedVariables, fonts, "fonts", true);
	Reflective::AddVariable(reflectedVariables, shaders, "shaders", true);
	Reflective::AddVariable(reflectedVariables, materials, "materials", true);

	Reflective::AddVariable(reflectedVariables, myComponents, "myComponents", true);
	Reflective::AddVariable(reflectedVariables, myGameObjects, "myGameObjects", true);
	Reflective::AddVariable(reflectedVariables, myTransforms, "myTransforms", true);
	Reflective::AddVariable(reflectedVariables, myEnum, "myEnum", true);
	Reflective::AddVariable(reflectedVariables, myMatos, "myMatos", true);

	Reflective::AddVariable(reflectedVariables, myVectors2, "myVectors2", true);
	Reflective::AddVariable(reflectedVariables, myVectors2Int, "myVectors2Int", true);
	Reflective::AddVariable(reflectedVariables, myVectors3, "myVectors3", true);
	Reflective::AddVariable(reflectedVariables, myVectors4, "myVectors4", true);
	Reflective::AddVariable(reflectedVariables, myCustomClasses, "myCustomClasses", true);
	Reflective::AddVariable(reflectedVariables, myInts, "myInts", true);
	Reflective::AddVariable(reflectedVariables, myFloats, "myFloats", true);
	Reflective::AddVariable(reflectedVariables, myUint64s, "myUint64s", true);
	Reflective::AddVariable(reflectedVariables, myDoubles, "myDoubles", true);
	Reflective::AddVariable(reflectedVariables, myStrings, "myStrings", true);
	Reflective::AddVariable(reflectedVariables, myEnums, "myEnums", true);
	Reflective::AddVariable(reflectedVariables, myMatoslist, "myMatoslist", true);
	Reflective::AddVariable(reflectedVariables, myColors, "myColors", true);

	return reflectedVariables;
}

void TestComponent::Start()
{
	
}

void TestComponent::Update()
{

}

CustomClass::CustomClass()
{
}

ReflectiveData CustomClass::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, myCustomFloat, "myCustomFloat", true);
	Reflective::AddVariable(reflectedVariables, myCustomFloat2, "myCustomFloat2", true);
	return reflectedVariables;
}