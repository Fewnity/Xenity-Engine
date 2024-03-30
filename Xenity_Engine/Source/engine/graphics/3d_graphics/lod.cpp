#include "lod.h"
#include <engine/vectors/vector3.h>

#include <engine/graphics/3d_graphics/mesh_renderer.h>
#include <engine/game_elements/transform.h>
#include <engine/graphics/graphics.h>
#include <engine/graphics/camera.h>

Lod::Lod()
{
	componentName = "Lod";
}

ReflectiveData Lod::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, lod0MeshRenderer, "lod0MeshRenderer", true);
	Reflective::AddVariable(reflectedVariables, lod1MeshRenderer, "lod1MeshRenderer", true);
	Reflective::AddVariable(reflectedVariables, lod1Distance, "lod1Distance", true);
	Reflective::AddVariable(reflectedVariables, lod2MeshRenderer, "lod2MeshRenderer", true);
	Reflective::AddVariable(reflectedVariables, lod2Distance, "lod2Distance", true);
	Reflective::AddVariable(reflectedVariables, culledDistance, "culledDistance", true);
	return reflectedVariables;
}

void Lod::CheckLod()
{
	const float camDis = Vector3::Distance(GetTransform()->GetPosition(), Graphics::usedCamera->GetTransform()->GetPosition());
	if (camDis >= culledDistance) 
	{
		SetAllLevel(false);
	}
	else if (camDis >= lod2Distance)
	{
		UseLevel(lod2MeshRenderer, lod0MeshRenderer, lod1MeshRenderer);
	}
	else if (camDis >= lod1Distance)
	{
		UseLevel(lod1MeshRenderer, lod0MeshRenderer, lod2MeshRenderer);
	}
	else
	{
		UseLevel(lod0MeshRenderer, lod1MeshRenderer, lod2MeshRenderer);
	}
}

void Lod::RemoveReferences()
{
	Graphics::RemoveLod(std::dynamic_pointer_cast<Lod>(shared_from_this()));
}

void Lod::UseLevel(std::weak_ptr<MeshRenderer> levelToEnable, std::weak_ptr<MeshRenderer> levelToDisable0, std::weak_ptr<MeshRenderer> levelToDisable1)
{
	if (levelToEnable.lock())
		levelToEnable.lock()->culled = false;

	if (levelToDisable0.lock())
		levelToDisable0.lock()->culled = true;

	if (levelToDisable1.lock())
		levelToDisable1.lock()->culled = true;
}

void Lod::SetAllLevel(bool visible)
{
	if (lod0MeshRenderer.lock())
		lod0MeshRenderer.lock()->culled = !visible;

	if (lod1MeshRenderer.lock())
		lod1MeshRenderer.lock()->culled = !visible;

	if (lod2MeshRenderer.lock())
		lod2MeshRenderer.lock()->culled = !visible;
}

Lod::~Lod()
{
	SetAllLevel(true);
}