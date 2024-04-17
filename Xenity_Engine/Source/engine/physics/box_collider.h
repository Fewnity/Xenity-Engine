#pragma once
#include <engine/api.h>

#include "collider.h"
#include <engine/vectors/vector3.h>
#include <engine/event_system/event_system.h>
#include <memory>

class RigidBody;

enum class CollisionSide 
{
	NoSide = 0,
	SideX = 1,
	SideY = 2,
	SideZ = 4,
};

class API BoxCollider : public Collider
{
public:
	BoxCollider();
	~BoxCollider();

	void Awake() override;

	Vector3 size = Vector3(1);
	Vector3 offset = Vector3(0);
	std::weak_ptr<RigidBody> currentRigidbody;

	ReflectiveData GetReflectiveData() override;
	void OnReflectionUpdated() override;
	
	/**
	 * @brief [Internal] Check if two box colliders are triggering each other
	 * @param a The first box collider
	 * @param b The second box collider
	 */
	static bool CheckTrigger(const std::shared_ptr<BoxCollider>& a, const std::shared_ptr <BoxCollider>& b);
	
	/**
	 * @brief [Internal] Check if two box colliders are colliding with each other
	 * @param a The first box collider
	 * @param b The second box collider
	 * @param aVelocity The velocity of the first box collider
	 */
	static CollisionSide CheckCollision(const std::shared_ptr <BoxCollider>& a, const std::shared_ptr <BoxCollider>& b, const Vector3& aVelocity);
	
	void OnDrawGizmosSelected() override;

	/**
	* @brief Set the default size of the box collider based on the mesh renderer
	*/
	void SetDefaultSize();

	/**
	* @brief Calculate the bounding box of the box collider
	*/
	void CalculateBoundingBox();


	Event<std::shared_ptr<BoxCollider>> onTriggerEvent;

	Vector3 min;
	Vector3 max;
	bool isTrigger = false;
private:
};
