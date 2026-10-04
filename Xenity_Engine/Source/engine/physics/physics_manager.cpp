// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2026 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "physics_manager.h"

#include <iostream>
#include <bullet/btBulletDynamicsCommon.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <engine/time/time.h>
#include <engine/debug/performance.h>
#include <engine/game_elements/gameobject.h>
#include "collider.h"
#include "rigidbody.h"
#include "collision_event.h"
#include <engine/debug/stack_debug_object.h>
#include <engine/constants.h>

std::vector<RigidBody*> PhysicsManager::s_rigidBodies;
std::vector<ColliderInfo> PhysicsManager::s_colliders;
Vector3 PhysicsManager::s_gravity = Vector3(0, DEFAULT_GRAVITY_Y, 0);

btDynamicsWorld* PhysicsManager::s_physDynamicsWorld = nullptr;
btBroadphaseInterface* physBroadphase = nullptr;
btCollisionDispatcher* physDispatcher = nullptr;
btConstraintSolver* physSolver = nullptr;
btDefaultCollisionConfiguration* physCollisionConfiguration = nullptr;

void PhysicsManager::Init()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	physCollisionConfiguration = new btDefaultCollisionConfiguration();

	physDispatcher = new btCollisionDispatcher(physCollisionConfiguration);

	btHashedOverlappingPairCache* pairCache = new btHashedOverlappingPairCache();
	physBroadphase = new btDbvtBroadphase(pairCache);

	physSolver = new btSequentialImpulseConstraintSolver();

	s_physDynamicsWorld = new btDiscreteDynamicsWorld(physDispatcher, physBroadphase, physSolver, physCollisionConfiguration);

	s_physDynamicsWorld->setGravity(btVector3(s_gravity.x, s_gravity.y, s_gravity.z));
	s_physDynamicsWorld->getSolverInfo().m_numIterations = 4;
}

void PhysicsManager::Stop()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	Clear();

	delete s_physDynamicsWorld;

	delete physSolver;

	delete physBroadphase;

	delete physDispatcher;

	delete physCollisionConfiguration;
}

void PhysicsManager::AddEvent(Collider* collider, Collider* otherCollider, bool isTrigger)
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);

	const size_t colliderCount = s_colliders.size();
	for (size_t i = 0; i < colliderCount; i++)
	{
		if (s_colliders[i].collider == collider)
		{
			/*ColliderInfo::CollisionInfo collisionInfo;
			collisionInfo.otherCollider = otherCollider;
			collisionInfo.state = CollisionState::FirstFrame;*/
			if (isTrigger)
			{
				auto tc = s_colliders[i].triggersCollisions.find(otherCollider);
				if (tc != s_colliders[i].triggersCollisions.end())
				{
					//std::cout << "Existing collision: " << collider->GetGameObject()->GetName() << " ToString" << collider->ToString() << std::endl;
					if (tc->second == CollisionState::RequireUpdate)
					{
						tc->second = CollisionState::Updated;
					}
				}
				else
				{
					//std::cout << "First collision: " << collider->GetGameObject()->GetName() << " ToString" << collider->ToString() << std::endl;
					s_colliders[i].triggersCollisions[otherCollider] = CollisionState::FirstFrame;
				}
			}
			else
			{
				auto tc = s_colliders[i].collisions.find(otherCollider);
				if (tc != s_colliders[i].collisions.end())
				{
					//std::cout << "Existing trigger collision: " << collider->GetGameObject()->GetName() << " ToString" << collider->ToString() << std::endl;
					//tc->second = CollisionState::Updated;
					if (tc->second == CollisionState::RequireUpdate)
					{
						tc->second = CollisionState::Updated;
					}
				}
				else
				{
					//std::cout << "First trigger collision: " << collider->GetGameObject()->GetName() << " ToString" << collider->ToString() << std::endl;
					s_colliders[i].collisions[otherCollider] = CollisionState::FirstFrame;
				}
				//m_colliders[i].collisions.push_back(collisionInfo);
			}
			break;
		}
	}

}


Collider* PhysicsManager::GetColliderFromCollisionObject(const btCollisionObject* collisionObject, int childIndex)
{
	if (const btRigidBody* bulletRb = btRigidBody::upcast(collisionObject))
	{
		// Rigidbodies use compound shapes, the child index gives the collider
		const RigidBody* rb = reinterpret_cast<const RigidBody*>(bulletRb->getUserPointer());
		const btCompoundShape* compoundShape = (bulletRb->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE) ? rb->m_bulletTriggerCompoundShape : rb->m_bulletCompoundShape;
		if (childIndex < 0 || childIndex >= compoundShape->getNumChildShapes())
		{
			return nullptr;
		}
		return reinterpret_cast<Collider*>(compoundShape->getChildShape(childIndex)->getUserPointer());
	}
	else
	{
		return reinterpret_cast<Collider*>(collisionObject->getUserPointer());
	}
}

bool PhysicsManager::GeneratesEvents(const btCollisionObject* collisionObject)
{
	if (const btRigidBody* bulletRb = btRigidBody::upcast(collisionObject))
	{
		const RigidBody* rb = reinterpret_cast<const RigidBody*>(bulletRb->getUserPointer());
		return rb->m_generatesEvents && rb->IsEnabled() && rb->GetGameObjectRaw()->IsLocalActive();
	}
	else
	{
		const Collider* collider = reinterpret_cast<const Collider*>(collisionObject->getUserPointer());
		return collider->m_generateCollisionEvents && collider->IsEnabled() && collider->GetGameObjectRaw()->IsLocalActive();
	}
}

void PhysicsManager::CallCollisionEvent(Collider* a, Collider* b, bool isTrigger, int state)
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);

	const CollisionEvent collisionEvent = CollisionEvent(a, b);
	const CollisionEvent collisionEventOther = CollisionEvent(b, a);

	std::shared_ptr<GameObject> aParent = a->GetGameObject();
	while (aParent != nullptr)
	{
		// The size is read at each iteration and the component is copied because the game code can add or remove components during the event
		for (size_t i = 0; i < aParent->m_components.size(); i++)
		{
			const std::shared_ptr<Component> component = aParent->m_components[i];
			if (component)
			{
				if (state == 0)
				{
					if (!isTrigger)
						component->OnCollisionEnter(collisionEvent);
					else
						component->OnTriggerEnter(collisionEvent);
				}
				else if (state == 1)
				{
					if (!isTrigger)
						component->OnCollisionStay(collisionEvent);
					else
						component->OnTriggerStay(collisionEvent);
				}
				else if (state == 2)
				{
					if (!isTrigger)
						component->OnCollisionExit(collisionEvent);
					else
						component->OnTriggerExit(collisionEvent);
				}
			}
		}
		aParent = aParent->GetParent().lock();
	}

	std::shared_ptr<GameObject> bParent = b->GetGameObject();
	while (bParent != nullptr)
	{
		// The size is read at each iteration and the component is copied because the game code can add or remove components during the event
		for (size_t i = 0; i < bParent->m_components.size(); i++)
		{
			const std::shared_ptr<Component> component = bParent->m_components[i];
			if (component)
			{
				if (state == 0)
				{
					if (!isTrigger)
						component->OnCollisionEnter(collisionEventOther);
					else
						component->OnTriggerEnter(collisionEventOther);
				}
				else if (state == 1)
				{
					if (!isTrigger)
						component->OnCollisionStay(collisionEventOther);
					else
						component->OnTriggerStay(collisionEventOther);
				}
				else if (state == 2)
				{
					if (!isTrigger)
						component->OnCollisionExit(collisionEventOther);
					else
						component->OnTriggerExit(collisionEventOther);
				}
			}
		}
		bParent = bParent->GetParent().lock();
	}
}

void PhysicsManager::Update()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	SCOPED_PROFILER("PhysicsManager::Update", scopeBenchmark);

	const size_t rigidbodyCount = s_rigidBodies.size();

	const size_t colliderCount = s_colliders.size();

	{
		SCOPED_PROFILER("PhysicsManager::Update|StepSimulation", scopeBenchmark2);
		//s_physDynamicsWorld->stepSimulation(Time::GetDeltaTime(), 2, Time::GetDeltaTime() / 2); // Increase physics accuracy but trigger won't work properly

		// Slow down the physics simulation if the frame rate is too low
		float timeStep = Time::GetDeltaTime();
		if (timeStep > 0.05f)
		{
			timeStep = 0.05f;
		}
		s_physDynamicsWorld->stepSimulation(timeStep, 0);
	}

	{
		SCOPED_PROFILER("PhysicsManager::Update|RigidBodyTick", scopeBenchmark2);
		for (size_t i = 0; i < rigidbodyCount; i++)
		{
			RigidBody* rb = s_rigidBodies[i];
			rb->Tick();
		}
	}

	{
		SCOPED_PROFILER("PhysicsManager::Update|ContactTest", scopeBenchmark2);
		// Read the contacts already computed by stepSimulation instead of running the narrowphase again with contactTest
		btDispatcher* dispatcher = s_physDynamicsWorld->getDispatcher();
		const int manifoldCount = dispatcher->getNumManifolds();
		for (int i = 0; i < manifoldCount; i++)
		{
			const btPersistentManifold* manifold = dispatcher->getManifoldByIndexInternal(i);
			const int contactCount = manifold->getNumContacts();
			if (contactCount == 0)
			{
				continue;
			}

			const btCollisionObject* object0 = manifold->getBody0();
			const btCollisionObject* object1 = manifold->getBody1();
			if (object0->isStaticOrKinematicObject() && object1->isStaticOrKinematicObject())
			{
				continue;
			}

			const bool object0GeneratesEvents = GeneratesEvents(object0);
			const bool object1GeneratesEvents = GeneratesEvents(object1);
			if (!object0GeneratesEvents && !object1GeneratesEvents)
			{
				continue;
			}

			int lastIndex0 = -2;
			int lastIndex1 = -2;
			for (int j = 0; j < contactCount; j++)
			{
				const btManifoldPoint& point = manifold->getContactPoint(j);
				if (point.m_index0 == lastIndex0 && point.m_index1 == lastIndex1)
				{
					continue;
				}
				lastIndex0 = point.m_index0;
				lastIndex1 = point.m_index1;

				Collider* col0 = GetColliderFromCollisionObject(object0, point.m_index0);
				Collider* col1 = GetColliderFromCollisionObject(object1, point.m_index1);
				if (!col0 || !col1 || col0->GetGameObjectRaw() == col1->GetGameObjectRaw())
				{
					continue;
				}

				// Register the pair only once, CallCollisionEvent notifies both colliders.
				// Use the side that generates events, or the lowest pointer if both do, so the pair stays on the same side between frames
				const bool isTrigger = col0->IsTrigger() || col1->IsTrigger();
				if (object0GeneratesEvents && (!object1GeneratesEvents || col0 < col1))
				{
					AddEvent(col0, col1, isTrigger);
				}
				else
				{
					AddEvent(col1, col0, isTrigger);
				}
			}
		}
	}

	{
		SCOPED_PROFILER("PhysicsManager::Update|CallCollisionEvent", scopeBenchmark2);
		// Call the collision events
		struct PendingCollisionEvent
		{
			Collider* otherCollider;
			bool isTrigger;
			int state;
		};
		std::vector<PendingCollisionEvent> pendingEvents;

		for (size_t i = 0; i < colliderCount && i < s_colliders.size(); i++)
		{
			// First update the collision states without calling game code:
			// the game code can add colliders (s_colliders reallocation) so no reference to s_colliders must be kept while calling events
			pendingEvents.clear();
			Collider* collider = s_colliders[i].collider;
			{
				ColliderInfo& colliderInfo = s_colliders[i];
				std::vector<Collider*> toRemove;

				for (auto& collision : colliderInfo.collisions)
				{
					if (collision.second == CollisionState::FirstFrame)
					{
						pendingEvents.push_back({ collision.first, false, 0 });
						collision.second = CollisionState::RequireUpdate;
					}
					else if (collision.second == CollisionState::Updated)
					{
						pendingEvents.push_back({ collision.first, false, 1 });
						collision.second = CollisionState::RequireUpdate;
					}
					else if (collision.second == CollisionState::RequireUpdate)
					{
						pendingEvents.push_back({ collision.first, false, 2 });
						toRemove.push_back(collision.first);
					}
				}

				for (auto& collision : colliderInfo.triggersCollisions)
				{
					if (collision.second == CollisionState::FirstFrame)
					{
						pendingEvents.push_back({ collision.first, true, 0 });
						collision.second = CollisionState::RequireUpdate;
					}
					else if (collision.second == CollisionState::Updated)
					{
						pendingEvents.push_back({ collision.first, true, 1 });
						collision.second = CollisionState::RequireUpdate;
					}
					else if (collision.second == CollisionState::RequireUpdate)
					{
						pendingEvents.push_back({ collision.first, true, 2 });
						toRemove.push_back(collision.first);
					}
				}

				for (auto& remove : toRemove)
				{
					colliderInfo.collisions.erase(remove);
					colliderInfo.triggersCollisions.erase(remove);
				}
			}

			// Then call the game code
			for (const PendingCollisionEvent& pendingEvent : pendingEvents)
			{
				CallCollisionEvent(collider, pendingEvent.otherCollider, pendingEvent.isTrigger, pendingEvent.state);
			}
		}
	}
}

void PhysicsManager::Clear()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	PhysicsManager::s_rigidBodies.clear();
	PhysicsManager::s_colliders.clear();
}

void PhysicsManager::AddRigidBody(RigidBody* rb)
{
	STACK_DEBUG_OBJECT(STACK_LOW_PRIORITY);

	s_rigidBodies.push_back(rb);
}

void PhysicsManager::RemoveRigidBody(const RigidBody* rb)
{
	STACK_DEBUG_OBJECT(STACK_LOW_PRIORITY);

	const size_t rigidbodyCount = s_rigidBodies.size();
	for (size_t i = 0; i < rigidbodyCount; i++)
	{
		if (s_rigidBodies[i] == rb)
		{
			s_rigidBodies.erase(s_rigidBodies.begin() + i);
			break;
		}
	}
}

void PhysicsManager::AddCollider(Collider* col)
{
	STACK_DEBUG_OBJECT(STACK_LOW_PRIORITY);

	ColliderInfo colliderInfo;
	colliderInfo.collider = col;
	s_colliders.push_back(colliderInfo);
}

void PhysicsManager::RemoveCollider(const Collider* col)
{
	STACK_DEBUG_OBJECT(STACK_LOW_PRIORITY);

	const size_t colliderCount = s_colliders.size();
	for (size_t i = 0; i < colliderCount; i++)
	{
		if (s_colliders[i].collider == col)
		{
			s_colliders.erase(s_colliders.begin() + i);
			break;
		}
	}

	// Remove the collider from the other colliders' collisions to avoid calling events with a destroyed collider
	Collider* colliderKey = const_cast<Collider*>(col);
	for (ColliderInfo& colliderInfo : s_colliders)
	{
		colliderInfo.collisions.erase(colliderKey);
		colliderInfo.triggersCollisions.erase(colliderKey);
	}
}