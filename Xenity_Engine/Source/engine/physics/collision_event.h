#pragma once

class Collider;

struct CollisionEvent
{
	CollisionEvent(Collider* collider, Collider* otherCollider)
		: collider(collider), otherCollider(otherCollider)
	{
	}

	Collider* collider;
	Collider* otherCollider;
};