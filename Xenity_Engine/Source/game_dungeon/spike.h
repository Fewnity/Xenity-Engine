#pragma once

#include <engine/monobehaviour.h>
#include <engine/component.h>
#include <engine/physics/rigidbody.h>

class Spike : public MonoBehaviour
{
public:
	Spike();
	void Start() override;
	void Update() override;
	ReflectiveData GetReflectiveData() override;
	std::weak_ptr<RigidBody> rigidbody;

private:
};
