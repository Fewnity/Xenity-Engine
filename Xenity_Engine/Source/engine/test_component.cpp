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
#include <engine/game_elements/prefab.h>
#include <engine/audio/audio_clip.h>
#include <engine/scene_management/scene.h>

ReflectiveData TestComponent::GetReflectiveData()
{
	ReflectiveData reflectedVariables;

	Reflective::AddVariable(reflectedVariables, myCustomClass, "myCustomClass");

	Reflective::AddVariable(reflectedVariables, myComponent, "myComponent");
	Reflective::AddVariable(reflectedVariables, myGameObject, "myGameObject");
	Reflective::AddVariable(reflectedVariables, myTransform, "myTransform");

	Reflective::AddVariable(reflectedVariables, vec2, "vec2");
	Reflective::AddVariable(reflectedVariables, vec2Int, "vec2Int");
	Reflective::AddVariable(reflectedVariables, vec3, "vec3");
	Reflective::AddVariable(reflectedVariables, vec3_2, "vec3_2");
	Reflective::AddVariable(reflectedVariables, vec3_3, "vec3_3");
	Reflective::AddVariable(reflectedVariables, vec4, "vec4");
	Reflective::AddVariable(reflectedVariables, quaternion, "quaternion");

	Reflective::AddVariable(reflectedVariables, color, "color");

	Reflective::AddVariable(reflectedVariables, myFloat, "myFloat");
	Reflective::AddVariable(reflectedVariables, myInt, "myInt");
	Reflective::AddVariable(reflectedVariables, myDouble, "myDouble");
	Reflective::AddVariable(reflectedVariables, myString, "myString");
	Reflective::AddVariable(reflectedVariables, myBool, "myBool");

	Reflective::AddVariable(reflectedVariables, skyBox, "skyBox");
	Reflective::AddVariable(reflectedVariables, prefab, "prefab");

	Reflective::AddVariable(reflectedVariables, textures, "textures");
	Reflective::AddVariable(reflectedVariables, meshData, "meshData");
	Reflective::AddVariable(reflectedVariables, audioClips, "audioClips");
	Reflective::AddVariable(reflectedVariables, scenes, "scenes");
	Reflective::AddVariable(reflectedVariables, skyBoxes, "skyBoxes");
	Reflective::AddVariable(reflectedVariables, fonts, "fonts");
	Reflective::AddVariable(reflectedVariables, shaders, "shaders");
	Reflective::AddVariable(reflectedVariables, materials, "materials");
	Reflective::AddVariable(reflectedVariables, prefabs, "prefabs");

	Reflective::AddVariable(reflectedVariables, myComponents, "myComponents");
	Reflective::AddVariable(reflectedVariables, myGameObjects, "myGameObjects");
	Reflective::AddVariable(reflectedVariables, myTransforms, "myTransforms");
	Reflective::AddVariable(reflectedVariables, myEnum, "myEnum");
	Reflective::AddVariable(reflectedVariables, myMatos, "myMatos");

	Reflective::AddVariable(reflectedVariables, myVectors2, "myVectors2");
	Reflective::AddVariable(reflectedVariables, myVectors2Int, "myVectors2Int");
	Reflective::AddVariable(reflectedVariables, myVectors3, "myVectors3");
	Reflective::AddVariable(reflectedVariables, myVectors4, "myVectors4");
	Reflective::AddVariable(reflectedVariables, myCustomClasses, "myCustomClasses");
	Reflective::AddVariable(reflectedVariables, myInts, "myInts");
	Reflective::AddVariable(reflectedVariables, myFloats, "myFloats");
	Reflective::AddVariable(reflectedVariables, myUint64s, "myUint64s");
	Reflective::AddVariable(reflectedVariables, myDoubles, "myDoubles");
	Reflective::AddVariable(reflectedVariables, myStrings, "myStrings");
	Reflective::AddVariable(reflectedVariables, myEnums, "myEnums");
	Reflective::AddVariable(reflectedVariables, myMatoslist, "myMatoslist");
	Reflective::AddVariable(reflectedVariables, myColors, "myColors");

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
	Reflective::AddVariable(reflectedVariables, myCustomFloat, "myCustomFloat");
	Reflective::AddVariable(reflectedVariables, myCustomFloat2, "myCustomFloat2");
	return reflectedVariables;
}