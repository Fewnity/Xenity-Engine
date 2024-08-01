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
#include <engine/tools/math.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <unordered_set>

std::vector<std::weak_ptr<RigidBody>> PhysicsManager::rigidBodies;
std::vector<std::weak_ptr<BoxCollider>> PhysicsManager::boxColliders;

btDynamicsWorld* PhysicsManager::physDynamicsWorld;
btBroadphaseInterface* physBroadphase;
btCollisionDispatcher* physDispatcher;
btConstraintSolver* physSolver;
btDefaultCollisionConfiguration* physCollisionConfiguration;

btCollisionShape* physBoxShape;
btRigidBody* physBoxBody;
btCollisionShape* physGroundShape;
btRigidBody* physGroundBody;
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

void PhysicsManager::GetRotation(const btRigidBody* body, btVector3& rot)
{
	btQuaternion btq = body->getOrientation();

	glm::vec3 eulerChildGlobal0n = glm::degrees(glm::eulerAngles(glm::quat(-btq.w(), btq.x(), btq.y(), btq.z())));

	rot.setX(-eulerChildGlobal0n.x);
	rot.setY(-eulerChildGlobal0n.y);
	rot.setZ(-eulerChildGlobal0n.z);


	//const glm::mat4 matChildRelative = glm::mat4_cast(glm::quat(btq.w(), btq.y(), btq.z(), btq.x()));
	//float x, y, z;
	//glm::extractEulerAngleYXZ(matChildRelative, x, y, z);
	//x = glm::degrees(x);
	//y = glm::degrees(y);
	//z = glm::degrees(z);
}

bool customContactProcessedCallback(btManifoldPoint& cp,
	void* body0, void* body1) {
	btCollisionObject* colObj0 = static_cast<btCollisionObject*>(body0);
	btCollisionObject* colObj1 = static_cast<btCollisionObject*>(body1);

	// You can add your event logic here
	std::cout << "Collision detected between objects: "
		<< colObj0->getUserIndex() << " and " << colObj1->getUserIndex() << std::endl;

	return false; // Return value not used by Bullet
}

bool customContactAddedCallback(btManifoldPoint& cp,
	const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0,
	const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) {
	const btCollisionObject* colObj0 = colObj0Wrap->getCollisionObject();
	const btCollisionObject* colObj1 = colObj1Wrap->getCollisionObject();

	static std::vector<std::pair<btCollisionObject*, btCollisionObject*>> contactedPairs;

	// Create a unique pair identifier
	auto pair = std::make_pair(colObj0, colObj1);
	if (colObj0 > colObj1) {
		pair = std::make_pair(colObj1, colObj0);
	}
	std::cout << "Collision detected between objects: "
		<< colObj0->getUserIndex() << " and " << colObj1->getUserIndex() << std::endl;

	// Check if this pair was already contacted
	//if (contactedPairs.find(pair) == contactedPairs.end()) {
	//	// New contact detected
	//	contactedPairs.insert(pair);
	//	std::cout << "First contact between objects: "
	//		<< colObj0->getUserIndex() << " and " << colObj1->getUserIndex() << std::endl;
	//}

	return false; // Return value not used by Bullet
}

void PhysicsManager::Init()
{
	btVector3 worldAabbMin(-1000, -1000, -1000);
	btVector3 worldAabbMax(1000, 1000, 1000);
	int maxProxies = 32766;

	physCollisionConfiguration = new btDefaultCollisionConfiguration();

	physDispatcher = new btCollisionDispatcher(physCollisionConfiguration);
	physBroadphase = new btAxisSweep3(worldAabbMin, worldAabbMax, maxProxies);

	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
	physSolver = solver;

	physDynamicsWorld = new btDiscreteDynamicsWorld(physDispatcher, physBroadphase, physSolver, physCollisionConfiguration);
	physDynamicsWorld->setGravity(btVector3(0, -10, 0));

	//gContactProcessedCallback = customContactProcessedCallback;
	gContactAddedCallback = customContactAddedCallback;
	//Create the box object
	//{
	//	//Create the box shape
	//	physBoxShape = new btBoxShape(btVector3(1, 1, 1));

	//	//Set mass, initial inertia and position for the box
	//	float mass = 1.0f;
	//	btVector3 inertia(0, 0, 0);
	//	btTransform startTransform;
	//	startTransform.setIdentity();
	//	startTransform.setOrigin(btVector3(0, 10, 0));

	//	//Calculate the inertia
	//	physBoxShape->calculateLocalInertia(mass, inertia);

	//	// Create MotionState and RigidBody object for the box shape
	//	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	//	physBoxBody = new btRigidBody(mass, myMotionState, physBoxShape, inertia);

	//	// Add box body to physics object & activate it
	//	physDynamicsWorld->addRigidBody(physBoxBody);
	//	physBoxBody->activate();

	//	//add to contener
	//	mBodies.push_back(physBoxBody);
	//}

	//{
	//	//Create the box shape
	//	physBoxShape = new btBoxShape(btVector3(1, 1, 1));

	//	//Set mass, initial inertia and position for the box
	//	float mass = 1.0f;
	//	btVector3 inertia(0, 0, 0);

	//	btTransform offsetTransform;

	//	btCompoundShape* compoundShape = new btCompoundShape();
	//	offsetTransform.setIdentity();
	//	//offsetTransform.setOrigin(btVector3(-10, 1, 0)); // Par exemple, un offset de (1, 0, 0)
	//	offsetTransform.setOrigin(btVector3(0, 0, 0)); // Par exemple, un offset de (1, 0, 0)
	//	compoundShape->addChildShape(offsetTransform, physBoxShape);
	//	compoundShape->calculateLocalInertia(mass, inertia);

	//	btTransform startTransform;
	//	startTransform.setIdentity();
	//	startTransform.setOrigin(btVector3(50, 10, 0));
	//	// Create MotionState and RigidBody object for the box shape
	//	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	//	physBoxBody = new btRigidBody(mass, myMotionState, nullptr, inertia);
	//	physBoxBody->setCollisionShape(compoundShape);
	//	// Add box body to physics object & activate it
	//	physDynamicsWorld->addRigidBody(physBoxBody);
	//	physBoxBody->activate();

	//	//compoundShape->

	//	//add to contener
	//	mBodies.push_back(physBoxBody);
	//}
}

void PhysicsManager::Update()
{
	SCOPED_PROFILER("PhysicsManager::Update", scopeBenchmark);

	physDynamicsWorld->stepSimulation(Time::GetDeltaTime());

	//{
	//	btVector3 pos;
	//	GetPosition(mBodies[0], pos);
	//	Debug::Print("Cube 0: " + std::to_string(pos.x()) + " " + std::to_string(pos.y()) + " " + std::to_string(pos.z()));
	//}

	//{
	//	btVector3 pos;
	//	GetPosition(mBodies[1], pos);
	//	Debug::Print("Cube 1: " + std::to_string(pos.x()) + " " + std::to_string(pos.y()) + " " + std::to_string(pos.z()));
	//}

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

	size_t colliderCount = boxColliders.size();
	for (int i = 0; i < colliderCount; i++)
	{
		if (!boxColliders[i].lock())
		{
			boxColliders.erase(boxColliders.begin() + i);
			i--;
			colliderCount--;
		}
	}

	for (int i = 0; i < rigidbodyCount; i++)
	{
		std::shared_ptr<RigidBody> rb = rigidBodies[i].lock();
		rb->Tick();
	}
}

void PhysicsManager::Clear()
{
	for (int i = physDynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = physDynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		physDynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}

	PhysicsManager::rigidBodies.clear();
	PhysicsManager::boxColliders.clear();
}
