#pragma once

#include "../component.h"
#include "../vectors/vector3.h"

class API RigidBody : public Component
{
public:
	RigidBody();
	~RigidBody();
	//void SetReflection();

	std::unordered_map<std::string, ReflectionEntry> GetReflection() override;

	void Update() override;
	Vector3 velocity;
	float drag = 1;

private:
};
