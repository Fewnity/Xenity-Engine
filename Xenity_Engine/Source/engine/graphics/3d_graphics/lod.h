// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <engine/api.h>
#include <engine/component.h>

class MeshRenderer;

class API Lod : public Component
{
public:
	Lod();
	~Lod();

protected:
	ReflectiveData GetReflectiveData() override;

	void RemoveReferences()  override;

	friend class Graphics;

	/**
	* @brief [Internal] Check the lod
	*/
	void CheckLod();

	/**
	* @brief Use one of the level and disable the others
	*/
	void UseLevel(std::weak_ptr<MeshRenderer> levelToEnable, std::weak_ptr<MeshRenderer> levelToDisable0, std::weak_ptr<MeshRenderer> levelToDisable1);

	/**
	* @brief Set all the level to visible or not
	*/
	void SetAllLevel(bool visible);

	std::weak_ptr<MeshRenderer> lod0MeshRenderer;
	std::weak_ptr<MeshRenderer> lod1MeshRenderer;
	std::weak_ptr<MeshRenderer> lod2MeshRenderer;
	float lod1Distance = 7;
	float lod2Distance = 15;
	float culledDistance = 30;
};

