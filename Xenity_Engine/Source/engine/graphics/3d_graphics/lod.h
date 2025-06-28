// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <engine/api.h>
#include <engine/component.h>

class MeshRenderer;

/**
* @brief Component to manage Level of Detail (LOD) for a GameObject
*/
class API Lod : public Component
{
public:
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
	void UseLevel(const std::weak_ptr<MeshRenderer>& levelToEnable, const std::weak_ptr<MeshRenderer>& levelToDisable0, const std::weak_ptr<MeshRenderer>& levelToDisable1);

	/**
	* @brief Set all the level to visible or not
	*/
	void SetAllLevel(bool visible);

	std::weak_ptr<MeshRenderer> m_lod0MeshRenderer;
	std::weak_ptr<MeshRenderer> m_lod1MeshRenderer;
	std::weak_ptr<MeshRenderer> m_lod2MeshRenderer;
	float m_lod1Distance = 7;
	float m_lod2Distance = 15;
	float m_culledDistance = 30;
};

