#pragma once

#include <engine/api.h>
#include <engine/component.h>

class MeshRenderer;

class Lod : public Component
{
public:
	Lod();
	~Lod();

	ReflectiveData GetReflectiveData() override;

	void CheckLod();
	void RemoveReferences()  override;

private:
	void UseLevel(std::weak_ptr<MeshRenderer> levelToEnable, std::weak_ptr<MeshRenderer> levelToDisable0, std::weak_ptr<MeshRenderer> levelToDisable1);
	void SetAllLevel(bool visible);

	std::weak_ptr<MeshRenderer> lod0MeshRenderer;
	std::weak_ptr<MeshRenderer> lod1MeshRenderer;
	float lod1Distance = 7;
	std::weak_ptr<MeshRenderer> lod2MeshRenderer;
	float lod2Distance = 15;
	float culledDistance = 30;
};

