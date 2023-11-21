#pragma once

#include "../engine/monobehaviour.h"
#include "../engine/component.h"
#include "../engine/physics/rigidbody.h"

class Chest : public MonoBehaviour
{
public:
	Chest();
	void Start() override;
	void Update() override;
	ReflectiveData GetReflectiveData() override;
	std::weak_ptr<RigidBody> rigidbody;

private:
};
