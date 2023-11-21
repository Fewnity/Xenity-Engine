#pragma once

#include "../engine/monobehaviour.h"
#include "../engine/component.h"
#include "../engine/physics/rigidbody.h"

class Coin : public MonoBehaviour
{
public:
	Coin();
	void Start() override;
	void Update() override;
	ReflectiveData GetReflectiveData() override;
	std::weak_ptr<RigidBody> rigidbody;
	float rotateSpeed = 10;

private:
};
