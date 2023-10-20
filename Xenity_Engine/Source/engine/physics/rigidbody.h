#pragma once

#include "../component.h"
#include "../vectors/vector3.h"

class Collider;

class API RigidBody : public Component
{
public:
	RigidBody();
	~RigidBody();
	//void SetReflection();

	std::unordered_map<std::string, ReflectionEntry> GetReflection() override;

	void Update() override;
	void Tick();
	Vector3 velocity = Vector3(0, 0, 0);
	float drag = 1;
	float gravityMultiplier = 1.0f;
	std::weak_ptr<Collider> attachedcollider;

private:
};
