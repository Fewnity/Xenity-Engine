// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <string>
#include <memory>

#include <engine/api.h>

class GameObject;
class Vector3;

/*
* @brief Class to spawn a GameObject with a Mesh Renderer filled using shape's mesh data 
*/
class API ShapeSpawner
{
public:

	/**
	* @brief Spawn a cube
	* @return A shared pointer to the cube GameObject
	*/
	static std::shared_ptr <GameObject> SpawnCube();

	/**
	* @brief Spawn a sphere
	* @return A shared pointer to the sphere GameObject
	*/
	static std::shared_ptr <GameObject> SpawnSphere();

	/**
	* @brief Spawn a cone
	* @return A shared pointer to the cone GameObject
	*/
	static std::shared_ptr <GameObject> SpawnCone();

	/**
	* @brief Spawn a donut
	* @return A shared pointer to the donut GameObject
	*/
	static std::shared_ptr <GameObject> SpawnDonut();

	/**
	* @brief Spawn a plane
	* @return A shared pointer to the plane GameObject
	*/
	static std::shared_ptr <GameObject> SpawnPlane();

	/**
	* @brief Spawn a cylinder
	* @return A shared pointer to the cylinder GameObject
	*/
	static std::shared_ptr <GameObject> SpawnCylinder();


private:
	/**
	* Create a GameObject and add a MeshRenderer to it
	* @param gameObjectName The name of the GameObject
	* @param meshFilePath The path to the mesh file
	* @return A shared pointer to the GameObject
	*/
	static std::shared_ptr<GameObject> MakeMesh(const std::string& gameObjectName, const std::string& meshFilePath);

	static Vector3 s_defaultPosition;
	static Vector3 s_defaultRotation;
	static Vector3 s_defaultScale;
	static void SetDefaultValues(const std::shared_ptr <GameObject>& gameObject);
};
