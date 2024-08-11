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

std::vector<RigidBody*> PhysicsManager::rigidBodies;
std::vector<ColliderInfo> PhysicsManager::colliders;

btDynamicsWorld* PhysicsManager::physDynamicsWorld = nullptr;
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

	physDynamicsWorld = new btDiscreteDynamicsWorld(physDispatcher, physBroadphase, physSolver, physCollisionConfiguration);

	physDynamicsWorld->setGravity(btVector3(0, -20, 0));
	physDynamicsWorld->getSolverInfo().m_numIterations = 4;
}

void PhysicsManager::Stop()
{
	Clear();

	delete physDynamicsWorld;

	delete physSolver;

	delete physBroadphase;

	delete physDispatcher;

	delete physCollisionConfiguration;
}

void PhysicsManager::AddEvent(Collider* collider, Collider* otherCollider, bool isTrigger)
{
	const size_t colliderCount = colliders.size();
	for (size_t i = 0; i < colliderCount; i++)
	{
		if (colliders[i].collider == collider)
		{
			/*ColliderInfo::CollisionInfo collisionInfo;
			collisionInfo.otherCollider = otherCollider;
			collisionInfo.state = CollisionState::FirstFrame;*/
			if (isTrigger)
			{
				auto tc = colliders[i].triggersCollisions.find(otherCollider);
				if (tc != colliders[i].triggersCollisions.end())
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
					colliders[i].triggersCollisions[otherCollider] = CollisionState::FirstFrame;
				}
			}
			else
			{
				auto tc = colliders[i].collisions.find(otherCollider);
				if (tc != colliders[i].collisions.end())
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
					colliders[i].collisions[otherCollider] = CollisionState::FirstFrame;
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
				col0 = reinterpret_cast<Collider*>(rb->bulletTriggerCompoundShape->getChildShape(index0)->getUserPointer());
			else
				col0 = reinterpret_cast<Collider*>(rb->bulletCompoundShape->getChildShape(index0)->getUserPointer());
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
				col1 = reinterpret_cast<Collider*>(rb->bulletTriggerCompoundShape->getChildShape(index1)->getUserPointer());
			else
				col1 = reinterpret_cast<Collider*>(rb->bulletCompoundShape->getChildShape(index1)->getUserPointer());
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
	const size_t goAComponentsCount = goA.components.size();
	const size_t goBComponentsCount = goB.components.size();


	CollisionEvent collisionEvent = CollisionEvent(a, b);
	CollisionEvent collisionEventOther = CollisionEvent(b, a);

	for (size_t i = 0; i < goAComponentsCount; i++)
	{
		std::shared_ptr<Component>& component = goA.components[i];
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
		std::shared_ptr<Component>& component = goB.components[i];
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

	const size_t rigidbodyCount = rigidBodies.size();

	const size_t colliderCount = colliders.size();

	//physDynamicsWorld->stepSimulation(Time::GetDeltaTime(), 1, 0.01666666f / 3.0f);
	physDynamicsWorld->stepSimulation(Time::GetDeltaTime());

	//Debug::Print("------------------------ Tick ------------------------");
	//std::cout << "------------------------ Tick ------------------------" << std::endl;
	for (size_t i = 0; i < rigidbodyCount; i++)
	{
		RigidBody* rb = rigidBodies[i];
		rb->Tick();
	}

	MyContactResultCallback resultCallback;
	for (size_t i = 0; i < rigidbodyCount; i++)
	{
		const RigidBody* rb = rigidBodies[i];
		if (rb->generatesEvents)
		{
			physDynamicsWorld->contactTest(rb->bulletRigidbody, resultCallback);
			physDynamicsWorld->contactTest(rb->bulletTriggerRigidbody, resultCallback);
		}
	}

	for (size_t i = 0; i < colliderCount; i++)
	{
		const ColliderInfo& colliderInfo = colliders[i];
		if (colliderInfo.collider->generateCollisionEvents && colliderInfo.collider->bulletCollisionObject)
		{
			physDynamicsWorld->contactTest(colliderInfo.collider->bulletCollisionObject, resultCallback);
		}
	}

	// Call the collision events
	for (size_t i = 0; i < colliderCount; i++)
	{
		ColliderInfo& colliderInfo = colliders[i];
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
	PhysicsManager::rigidBodies.clear();
	PhysicsManager::colliders.clear();
}

void PhysicsManager::AddRigidBody(RigidBody* rb)
{
	rigidBodies.push_back(rb);
}

void PhysicsManager::RemoveRigidBody(const RigidBody* rb)
{
	const size_t rigidbodyCount = rigidBodies.size();
	for (size_t i = 0; i < rigidbodyCount; i++)
	{
		if (rigidBodies[i] == rb)
		{
			rigidBodies.erase(rigidBodies.begin() + i);
			break;
		}
	}
}

void PhysicsManager::AddCollider(Collider* col)
{
	ColliderInfo colliderInfo;
	colliderInfo.collider = col;
	colliders.push_back(colliderInfo);
}

void PhysicsManager::RemoveCollider(const Collider* col)
{
	const size_t colliderCount = colliders.size();
	for (size_t i = 0; i < colliderCount; i++)
	{
		if (colliders[i].collider == col)
		{
			colliders.erase(colliders.begin() + i);
			break;
		}
	}
}