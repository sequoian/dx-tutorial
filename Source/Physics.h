#pragma once

#include "btBulletDynamicsCommon.h"
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include "WriteLog.h"
#include "Entity.h"
#include "RigidBody.h"
#include "EventBus.h"
#include "ColliderPtr.h"
#include <DirectXMath.h>
using namespace DirectX;

class Physics
{
public:
	~Physics();
	bool StartUp(EventBus* eventBus);
	void ShutDown();
	void RunSimulation(float deltaTime);
	void SetGravity(float gravity);

	ColliderPtr CreateCollisionBox(float x, float y, float z);
	ColliderPtr CreateCollisionSphere(float radius);
	ColliderPtr CreateCollisionCylinder(float x, float y, float z);
	ColliderPtr CreateCollisionCone(float radius, float height);
	ColliderPtr CreateCollisionCapsule(float radius, float height);

	RigidBody CreateDynamicRigidBody(Entity e, ColliderPtr shape, XMVECTOR position, XMVECTOR rotation, float mass = 1.f);
	RigidBody CreateStaticRigidBody(Entity e, ColliderPtr shape, XMVECTOR position, XMVECTOR rotation, bool isTrigger = false);
	RigidBody CreateKinematicRigidBody(Entity e, ColliderPtr shape, XMVECTOR position, XMVECTOR rotation, bool isTrigger = false);
	RigidBody CreateCharacterBody(Entity e, ColliderPtr shape, XMVECTOR position, XMVECTOR rotation);

	void DestroyRigidBody(RigidBody body);

	btCollisionWorld::ClosestRayResultCallback RayCast(XMVECTOR start, XMVECTOR end);
	btCollisionWorld::AllHitsRayResultCallback RayCastAll(XMVECTOR startPos, XMVECTOR endPos);

	static btQuaternion QuatFromDX(XMVECTOR quat);
	static XMVECTOR QuatToDX(btQuaternion quat);
	static btVector3 VecFromDX(XMVECTOR vec);
	static XMVECTOR VecToDX(btVector3 vec);
	static btTransform MatFromDX(XMMATRIX mat);
	static XMMATRIX MatToDX(btTransform mat);

private:
	void SimulationCallback(btDynamicsWorld* world, btScalar timeStep);

private:
	btDefaultCollisionConfiguration* m_collisionConfiguration;
	btCollisionDispatcher* m_dispatcher;
	btBroadphaseInterface* m_overlappingPairCache;
	btSequentialImpulseConstraintSolver* m_solver;
	btDiscreteDynamicsWorld* m_dynamicsWorld;
	float m_gravity;
	EventBus* m_eventBus;
};