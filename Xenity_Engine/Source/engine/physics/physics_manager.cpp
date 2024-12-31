// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
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


class MyContactResultCallback : public btCollisionWorld::ContactResultCallback
{
public:
	bool onCollisionEnter(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1)
	{
		STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);

		if (colObj0Wrap->getCollisionObject()->isStaticOrKinematicObject() && colObj1Wrap->getCollisionObject()->isStaticOrKinematicObject())
		{
			return false;
		}


		Collider* col0 = nullptr;
		Collider* col1 = nullptr;

		//std::cout << "------------ Collision detected between objects ------------" << std::endl;
		//if (!colObj0Wrap->getCollisionObject()->isStaticOrKinematicObject())
		if (auto bulletRb = dynamic_cast<const btRigidBody*>(colObj0Wrap->getCollisionObject()))
		{
			RigidBody* rb = reinterpret_cast<RigidBody*>(colObj0Wrap->getCollisionObject()->getUserPointer());
			if (colObj0Wrap->getCollisionObject()->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE)
				col0 = reinterpret_cast<Collider*>(rb->m_bulletTriggerCompoundShape->getChildShape(index0)->getUserPointer());
			else
				col0 = reinterpret_cast<Collider*>(rb->m_bulletCompoundShape->getChildShape(index0)->getUserPointer());
			//std::cout << "Object0: " << rb->GetGameObject()->GetName() << " ToString" << col0->ToString() << std::endl;
		}
		else
		{
			col0 = reinterpret_cast<Collider*>(colObj0Wrap->getCollisionObject()->getUserPointer());
			//std::cout << "Object0: " << col0->GetGameObject()->GetName() << std::endl;
		}
		/*if (colObj0Wrap->getCollisionObject()->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE)
		{
			std::cout << "Object0 is a trigger" << std::endl;
		}*/

		//if (!colObj1Wrap->getCollisionObject()->isStaticOrKinematicObject())
		if(auto bulletRb = dynamic_cast<const btRigidBody*>(colObj1Wrap->getCollisionObject()))
		{
			RigidBody* rb = reinterpret_cast<RigidBody*>(colObj1Wrap->getCollisionObject()->getUserPointer());
			if (colObj1Wrap->getCollisionObject()->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE)
				col1 = reinterpret_cast<Collider*>(rb->m_bulletTriggerCompoundShape->getChildShape(index1)->getUserPointer());
			else
				col1 = reinterpret_cast<Collider*>(rb->m_bulletCompoundShape->getChildShape(index1)->getUserPointer());
			//std::cout << "Object1: " << rb->GetGameObject()->GetName() << " ToString" << col1->ToString() << std::endl;
			//std::cout << "Object1: " << rb->GetGameObject()->GetName() << std::endl;
		}
		else
		{
			col1 = reinterpret_cast<Collider*>(colObj1Wrap->getCollisionObject()->getUserPointer());
			//std::cout << "Object1: " << col1->GetGameObject()->GetName() << std::endl;
		}
		/*if (colObj1Wrap->getCollisionObject()->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE)
		{
			std::cout << "Object1 is a trigger" << std::endl;
		}*/

		//colObj0Wrap->m_collisionObject->
	//	std::cout << "Collision detected between objects " << colObj0Wrap->getCollisionObject() << " and " << colObj1Wrap->getCollisionObject() << std::endl;
		//std::cout << partId0 << " " << index0 << " and " << partId1 << "   " << index1 << std::endl;
		if (col0 && col1 && col0->GetGameObjectRaw() != col1->GetGameObjectRaw())
			PhysicsManager::AddEvent(col0, col1, col0->IsTrigger() || col1->IsTrigger());

		return false;
	}

	btScalar addSingleResult(btManifoldPoint& cp,
		const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0,
		const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) override
	{
		onCollisionEnter(cp, colObj0Wrap, partId0, index0, colObj1Wrap, partId1, index1);
		return 0;
	}
};

void PhysicsManager::CallCollisionEvent(Collider* a, Collider* b, bool isTrigger, int state)
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);

	const CollisionEvent collisionEvent = CollisionEvent(a, b);
	const CollisionEvent collisionEventOther = CollisionEvent(b, a);

	std::shared_ptr<GameObject> aParent = a->GetGameObject();
	while (aParent != nullptr)
	{
		const size_t goAComponentsCount = aParent->m_components.size();
		for (size_t i = 0; i < goAComponentsCount; i++)
		{
			const std::shared_ptr<Component>& component = aParent->m_components[i];
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
		const size_t goBComponentsCount = bParent->m_components.size();
		for (size_t i = 0; i < goBComponentsCount; i++)
		{
			const std::shared_ptr<Component>& component = bParent->m_components[i];
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
		MyContactResultCallback resultCallback;
		for (size_t i = 0; i < rigidbodyCount; i++)
		{
			const RigidBody* rb = s_rigidBodies[i];
			if (rb->m_generatesEvents && rb->IsEnabled() && rb->GetGameObjectRaw()->IsLocalActive())
			{
				if (!rb->m_isEmpty)
				{
					s_physDynamicsWorld->contactTest(rb->m_bulletRigidbody, resultCallback);
				}
				if (rb->m_isTriggerEmpty)
				{
					s_physDynamicsWorld->contactTest(rb->m_bulletTriggerRigidbody, resultCallback);
				}
			}
		}

		for (size_t i = 0; i < colliderCount; i++)
		{
			const ColliderInfo& colliderInfo = s_colliders[i];
			if (colliderInfo.collider->m_generateCollisionEvents && colliderInfo.collider->m_bulletCollisionObject && colliderInfo.collider->IsEnabled() && colliderInfo.collider->GetGameObjectRaw()->IsLocalActive())
			{
				s_physDynamicsWorld->contactTest(colliderInfo.collider->m_bulletCollisionObject, resultCallback);
			}
		}
	}

	{
		SCOPED_PROFILER("PhysicsManager::Update|CallCollisionEvent", scopeBenchmark2);
		// Call the collision events
		for (size_t i = 0; i < colliderCount; i++)
		{
			ColliderInfo& colliderInfo = s_colliders[i];
			std::vector<Collider*> toRemove;

			for (auto& collision : colliderInfo.collisions)
			{
				if (collision.second == CollisionState::FirstFrame)
				{
					CallCollisionEvent(colliderInfo.collider, collision.first, false, 0);
					//std::cout << "OnCollisionEnter: " << colliderInfo.collider->GetGameObject()->GetName() << " " << collision.first->GetGameObject()->GetName() << std::endl;
					collision.second = CollisionState::RequireUpdate;
				}
				else if (collision.second == CollisionState::Updated)
				{
					CallCollisionEvent(colliderInfo.collider, collision.first, false, 1);
					//std::cout << "OnCollisionStay: " << colliderInfo.collider->GetGameObject()->GetName() << " " << collision.first->GetGameObject()->GetName() << std::endl;
					collision.second = CollisionState::RequireUpdate;
				}
				else if (collision.second == CollisionState::RequireUpdate)
				{
					CallCollisionEvent(colliderInfo.collider, collision.first, false, 2);
					//std::cout << "OnCollisionExit: " << colliderInfo.collider->GetGameObject()->GetName() << " " << collision.first->GetGameObject()->GetName() << std::endl;
					toRemove.push_back(collision.first);
				}
			}

			for (auto& collision : colliderInfo.triggersCollisions)
			{
				if (collision.second == CollisionState::FirstFrame)
				{
					CallCollisionEvent(colliderInfo.collider, collision.first, true, 0);
					//std::cout << "OnTriggerEnter: " << colliderInfo.collider->GetGameObject()->GetName() << " " << collision.first->GetGameObject()->GetName() << std::endl;
					collision.second = CollisionState::RequireUpdate;
				}
				else if (collision.second == CollisionState::Updated)
				{
					CallCollisionEvent(colliderInfo.collider, collision.first, true, 1);
					//std::cout << "OnTriggerStay: " << colliderInfo.collider->GetGameObject()->GetName() << " " << collision.first->GetGameObject()->GetName() << std::endl;
					collision.second = CollisionState::RequireUpdate;
				}
				else if (collision.second == CollisionState::RequireUpdate)
				{
					CallCollisionEvent(colliderInfo.collider, collision.first, true, 2);
					//std::cout << "OnTriggerExit: " << colliderInfo.collider->GetGameObject()->GetName() << " " << collision.first->GetGameObject()->GetName() << std::endl;
					toRemove.push_back(collision.first);
				}
			}

			for (auto& remove : toRemove)
			{
				colliderInfo.collisions.erase(remove);
				colliderInfo.triggersCollisions.erase(remove);
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
}