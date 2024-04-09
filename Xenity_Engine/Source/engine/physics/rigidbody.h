#pragma once
#include <engine/api.h>

#include <engine/component.h>
#include <engine/vectors/vector3.h>

class BoxCollider;

class API RigidBody : public Component
{
public:
	RigidBody();
	~RigidBody();

	ReflectiveData GetReflectiveData() override;

	void Update() override;

	/**
	 * [Internal]
	 */
	void Tick();

	Vector3 velocity = Vector3(0, 0, 0);
	float drag = 1;
	bool isStatic = false;
	float bounce = 0.5f;
	float gravityMultiplier = 1.0f;
private:
};
