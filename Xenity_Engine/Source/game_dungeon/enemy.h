#pragma once

#include <engine/monobehaviour.h>
#include <engine/component.h>
#include <engine/physics/rigidbody.h>

class GameObject;

class Enemy : public MonoBehaviour
{
public:
	Enemy();
	void Start() override;
	void Update() override;
	ReflectiveData GetReflectiveData() override;
	void OnDrawGizmos() override;

private:
	std::weak_ptr<RigidBody> rigidbody;
	std::weak_ptr<GameObject> waypoint0;
	std::weak_ptr<GameObject> waypoint1;
	int currentWaypoint = 0;
	float waitCooldown = 0;
	float waitCurrentCooldown = 0;
};
