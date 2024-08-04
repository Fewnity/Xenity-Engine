// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "physics_manager.h"

#include "collider.h"
#include "box_collider.h"
#include "rigidbody.h"
#include <bullet/btBulletDynamicsCommon.h>
#include <engine/time/time.h>
#include <engine/debug/performance.h>
#include <engine/debug/debug.h>
#include <engine/game_elements/gameobject.h>
#include <engine/tools/math.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <unordered_set>

std::vector<std::weak_ptr<RigidBody>> PhysicsManager::rigidBodies;
std::vector<std::weak_ptr<Collider>> PhysicsManager::colliders;

btDynamicsWorld* PhysicsManager::physDynamicsWorld = nullptr;
btBroadphaseInterface* physBroadphase = nullptr;
btCollisionDispatcher* physDispatcher = nullptr;
btConstraintSolver* physSolver = nullptr;
btDefaultCollisionConfiguration* physCollisionConfiguration = nullptr;

std::vector<btRigidBody*> PhysicsManager::mBodies;

void PhysicsManager::GetPosition(const btRigidBody* body, btVector3& pos)
{
	if (body && body->getMotionState())
	{
		const btVector3& p = body->getCenterOfMassPosition();
		pos.setX(p.getX());
		pos.setY(p.getY());
		pos.setZ(p.getZ());
	}
}

void PhysicsManager::Init()
{
	/*btVector3 worldAabbMin(-1000, -1000, -1000);
	btVector3 worldAabbMax(1000, 1000, 1000);
	int maxProxies = 32766;*/

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

class MyContactResultCallback : public btCollisionWorld::ContactResultCallback {
public:
	bool onCollisionEnter(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) 
	{
		if(colObj0Wrap->getCollisionObject()->isStaticOrKinematicObject() && colObj1Wrap->getCollisionObject()->isStaticOrKinematicObject())
		{
			return false;
		}
		std::cout << "------------ Collision detected between objects ------------" << std::endl;
		if (!colObj0Wrap->getCollisionObject()->isStaticOrKinematicObject()) 
		{
			RigidBody* rb = reinterpret_cast<RigidBody*>(colObj0Wrap->getCollisionObject()->getUserPointer());
			std::cout << "Object0: " << rb->GetGameObject()->GetName() << std::endl;
		}
		else 
		{
			Collider* rb = reinterpret_cast<Collider*>(colObj0Wrap->getCollisionObject()->getUserPointer());
			std::cout << "Object0: " << rb->GetGameObject()->GetName() << std::endl;
		}
		if (colObj0Wrap->getCollisionObject()->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE) 
		{
			std::cout << "Object0 is a trigger" << std::endl;
		}

		if (!colObj1Wrap->getCollisionObject()->isStaticOrKinematicObject())
		{
			RigidBody* rb = reinterpret_cast<RigidBody*>(colObj1Wrap->getCollisionObject()->getUserPointer());
			std::cout << "Object1: " << rb->GetGameObject()->GetName() << std::endl;
		}
		else 
		{
			Collider* rb = reinterpret_cast<Collider*>(colObj1Wrap->getCollisionObject()->getUserPointer());
			std::cout << "Object1: " << rb->GetGameObject()->GetName() << std::endl;
		}
		if (colObj1Wrap->getCollisionObject()->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE)
		{
			std::cout << "Object1 is a trigger" << std::endl;
		}

		//colObj0Wrap->m_collisionObject->
		std::cout << "Collision detected between objects " << colObj0Wrap->getCollisionObject() << " and " << colObj1Wrap->getCollisionObject() << std::endl;
		std::cout << partId0 << " " << index0 << " and " << partId1 << "   " << index1 << std::endl;
		
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

void PhysicsManager::Update()
{
	SCOPED_PROFILER("PhysicsManager::Update", scopeBenchmark);

	size_t rigidbodyCount = rigidBodies.size();
	for (int i = 0; i < rigidbodyCount; i++)
	{
		if (!rigidBodies[i].lock())
		{
			rigidBodies.erase(rigidBodies.begin() + i);
			i--;
			rigidbodyCount--;
		}
	}

	physDynamicsWorld->stepSimulation(Time::GetDeltaTime());
	//Debug::Print("------------------------ Tick ------------------------");

	//const size_t rigidbodyCount = rigidBodies.size();
	for (size_t i = 0; i < rigidbodyCount; i++)
	{
		std::shared_ptr<RigidBody> rb = rigidBodies[i].lock();
		rb->Tick();
	}

	MyContactResultCallback resultCallback;
	for (size_t i = 0; i < rigidbodyCount; i++)
	{
		std::shared_ptr<RigidBody> rb = rigidBodies[i].lock();
		if(rb->generatesEvents)
		{
			physDynamicsWorld->contactTest(rb->bulletRigidbody, resultCallback);
		}
	}

	size_t colliderCount = colliders.size();
	for (int i = 0; i < colliderCount; i++)
	{
		if (!colliders[i].lock())
		{
			colliders.erase(colliders.begin() + i);
			i--;
			colliderCount--;
		}
	}
	for (size_t i = 0; i < colliderCount; i++)
	{
		std::shared_ptr<Collider> collider = colliders[i].lock();
		if (collider->generateCollisionEvents && collider->bulletCollisionObject)
		{
			physDynamicsWorld->contactTest(collider->bulletCollisionObject, resultCallback);
		}
	}
	
 	//const btCollisionObjectArray& objectList = physDynamicsWorld->getCollisionObjectArray();

	//const size_t objectCount = objectList.size();
	//for (size_t i = 0; i < objectCount; i++)
	//{
	//	//std::cout << a[i] << std::endl;
	//	physDynamicsWorld->contactTest(objectList[i], resultCallback);
	//}

	//size_t rigidbodyCount = rigidBodies.size();
	//for (int i = 0; i < rigidbodyCount; i++)
	//{
	//	if (!rigidBodies[i].lock())
	//	{
	//		rigidBodies.erase(rigidBodies.begin() + i);
	//		i--;
	//		rigidbodyCount--;
	//	}
	//}

	//size_t colliderCount = boxColliders.size();
	//for (int i = 0; i < colliderCount; i++)
	//{
	//	if (!boxColliders[i].lock())
	//	{
	//		boxColliders.erase(boxColliders.begin() + i);
	//		i--;
	//		colliderCount--;
	//	}
	//}
}

void PhysicsManager::Clear()
{
	/*for (int i = physDynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = physDynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		physDynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}*/

	PhysicsManager::rigidBodies.clear();
	PhysicsManager::colliders.clear();
}
