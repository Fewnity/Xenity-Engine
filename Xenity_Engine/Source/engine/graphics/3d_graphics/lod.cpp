// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "lod.h"

#include <engine/math/vector3.h>
#include <engine/graphics/3d_graphics/mesh_renderer.h>
#include <engine/game_elements/transform.h>
#include <engine/graphics/graphics.h>
#include <engine/graphics/camera.h>

ReflectiveData Lod::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, m_lod0MeshRenderer, "lod0MeshRenderer", true);
	Reflective::AddVariable(reflectedVariables, m_lod1MeshRenderer, "lod1MeshRenderer", true);
	Reflective::AddVariable(reflectedVariables, m_lod1Distance, "lod1Distance", true);
	Reflective::AddVariable(reflectedVariables, m_lod2MeshRenderer, "lod2MeshRenderer", true);
	Reflective::AddVariable(reflectedVariables, m_lod2Distance, "lod2Distance", true);
	Reflective::AddVariable(reflectedVariables, m_culledDistance, "culledDistance", true);
	return reflectedVariables;
}

void Lod::CheckLod()
{
	const float camDis = Vector3::Distance(GetTransformRaw()->GetPosition(), Graphics::usedCamera->GetTransformRaw()->GetPosition());
	if (camDis >= m_culledDistance) 
	{
		SetAllLevel(false);
	}
	else if (camDis >= m_lod2Distance)
	{
		UseLevel(m_lod2MeshRenderer, m_lod0MeshRenderer, m_lod1MeshRenderer);
	}
	else if (camDis >= m_lod1Distance)
	{
		UseLevel(m_lod1MeshRenderer, m_lod0MeshRenderer, m_lod2MeshRenderer);
	}
	else
	{
		UseLevel(m_lod0MeshRenderer, m_lod1MeshRenderer, m_lod2MeshRenderer);
	}
}

void Lod::RemoveReferences()
{
	Graphics::RemoveLod(std::dynamic_pointer_cast<Lod>(shared_from_this()));
}

void Lod::UseLevel(const std::weak_ptr<MeshRenderer>& levelToEnable, const std::weak_ptr<MeshRenderer>& levelToDisable0, const std::weak_ptr<MeshRenderer>& levelToDisable1)
{
	// Set levelToEnable as visible and other as not visible
	if (levelToEnable.lock())
	{
		levelToEnable.lock()->m_culled = false;
	}

	if (levelToDisable0.lock())
	{
		levelToDisable0.lock()->m_culled = true;
	}

	if (levelToDisable1.lock())
	{
		levelToDisable1.lock()->m_culled = true;
	}
}

void Lod::SetAllLevel(bool visible)
{
	if (m_lod0MeshRenderer.lock())
	{
		m_lod0MeshRenderer.lock()->m_culled = !visible;
	}

	if (m_lod1MeshRenderer.lock())
	{
		m_lod1MeshRenderer.lock()->m_culled = !visible;
	}

	if (m_lod2MeshRenderer.lock())
	{
		m_lod2MeshRenderer.lock()->m_culled = !visible;
	}
}

Lod::~Lod()
{
	SetAllLevel(true);
}