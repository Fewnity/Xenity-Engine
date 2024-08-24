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

std::vector<RigidBody*> PhysicsManager::s_rigidBodies;
std::vector<ColliderInfo> PhysicsManager::s_colliders;
Vector3 PhysicsManager::s_gravity = Vector3(0, -20, 0);

btDynamicsWorld* PhysicsManager::s_physDynamicsWorld = nullptr;
btBroadphaseInterface* physBroadphase = nullptr;
btCollisionDispatcher* physDispatcher = nullptr;
btConstraintSolver* physSolver = nullptr;
btDefaultCollisionConfiguration* physCollisionConfiguration = nullptr;

void PhysicsManager::Init()
{
	physCollisionConfiguration = new btDefaultCollisionConfiguration();

	physDispatcher = new btCollisionDispatcher(physCollisionConfiguration);

	btHashedOverlappingPairCache* pairCache = new btHashedOverlappingPairCache();
	physBroadphase = new btDbvtBroadphase(pairCache);

	physSolver = new btSequentialImpulseConstraintSolver();

	s_physDynamicsWorld = new btDiscreteDynamicsWorld(physDispatcher, physBroadphase, physSolver, physCollisionConfiguration);

	s_physDynamicsWorld->setGravity(btVector3(0, -20, 0));
	s_physDynamicsWorld->getSolverInfo().m_numIterations = 4;
}

void PhysicsManager::Stop()
{
	Clear();

	delete s_physDynamicsWorld;

	delete physSolver;

	delete physBroadphase;

	delete physDispatcher;

	delete physCollisionConfiguration;
}

void PhysicsManager::AddEvent(Collider* collider, Collider* otherCollider, bool isTrigger)
{
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
				//colliders[i].collisions.push_back(collisionInfo);
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
		if (col0 && col1 && col0->GetGameObject() != col1->GetGameObject())
			PhysicsManager::AddEvent(col0, col1, col0->isTrigger || col1->isTrigger);

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
	GameObject& goA = *a->GetGameObject();
	GameObject& goB = *b->GetGameObject();
	const size_t goAComponentsCount = goA.m_components.size();
	const size_t goBComponentsCount = goB.m_components.size();


	CollisionEvent collisionEvent = CollisionEvent(a, b);
	CollisionEvent collisionEventOther = CollisionEvent(b, a);

	for (size_t i = 0; i < goAComponentsCount; i++)
	{
		std::shared_ptr<Component>& component = goA.m_components[i];
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

	for (size_t i = 0; i < goBComponentsCount; i++)
	{
		std::shared_ptr<Component>& component = goB.m_components[i];
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
}

void PhysicsManager::Update()
{
	SCOPED_PROFILER("PhysicsManager::Update", scopeBenchmark);

	const size_t rigidbodyCount = s_rigidBodies.size();

	const size_t colliderCount = s_colliders.size();

	//physDynamicsWorld->stepSimulation(Time::GetDeltaTime(), 1, 0.01666666f / 3.0f);
	s_physDynamicsWorld->stepSimulation(Time::GetDeltaTime());

	//Debug::Print("------------------------ Tick ------------------------");
	//std::cout << "------------------------ Tick ------------------------" << std::endl;
	for (size_t i = 0; i < rigidbodyCount; i++)
	{
		RigidBody* rb = s_rigidBodies[i];
		rb->Tick();
	}

	MyContactResultCallback resultCallback;
	for (size_t i = 0; i < rigidbodyCount; i++)
	{
		const RigidBody* rb = s_rigidBodies[i];
		if (rb->m_generatesEvents)
		{
			s_physDynamicsWorld->contactTest(rb->m_bulletRigidbody, resultCallback);
			s_physDynamicsWorld->contactTest(rb->m_bulletTriggerRigidbody, resultCallback);
		}
	}

	for (size_t i = 0; i < colliderCount; i++)
	{
		const ColliderInfo& colliderInfo = s_colliders[i];
		if (colliderInfo.collider->generateCollisionEvents && colliderInfo.collider->m_bulletCollisionObject)
		{
			s_physDynamicsWorld->contactTest(colliderInfo.collider->m_bulletCollisionObject, resultCallback);
		}
	}

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

void PhysicsManager::Clear()
{
	PhysicsManager::s_rigidBodies.clear();
	PhysicsManager::s_colliders.clear();
}

void PhysicsManager::AddRigidBody(RigidBody* rb)
{
	s_rigidBodies.push_back(rb);
}

void PhysicsManager::RemoveRigidBody(const RigidBody* rb)
{
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
	ColliderInfo colliderInfo;
	colliderInfo.collider = col;
	s_colliders.push_back(colliderInfo);
}

void PhysicsManager::RemoveCollider(const Collider* col)
{
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