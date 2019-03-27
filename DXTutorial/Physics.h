#pragma once

#include "btBulletDynamicsCommon.h"
#include "WriteLog.h"
#include <DirectXMath.h>
using namespace DirectX;

class Physics
{
public:
	~Physics();
	bool StartUp();
	void ShutDown();
	void RunSimulation(float deltaTime);
	void SetGravity(float gravity);
	btCollisionShape* CreateCollisionBox(float x, float y, float z);
	btCollisionShape* CreateCollisionSphere(float radius);
	btCollisionShape* CreateCollisionCylinder(float x, float y, float z);
	btCollisionShape* CreateCollisionCone(float radius, float height);
	btRigidBody* CreateRigidBody(XMVECTOR position, XMVECTOR rotation, float mass, btCollisionShape* shape, bool isKinematic = false, bool isTrigger = false);

	static inline btQuaternion QuatFromDX(XMVECTOR quat);
	static inline XMVECTOR QuatToDX(btQuaternion quat);
	static inline btVector3 VecFromDX(XMVECTOR vec);
	static inline XMVECTOR VecToDX(btVector3 vec);

private:
	btDefaultCollisionConfiguration* m_collisionConfiguration;
	btCollisionDispatcher* m_dispatcher;
	btBroadphaseInterface* m_overlappingPairCache;
	btSequentialImpulseConstraintSolver* m_solver;
	btDiscreteDynamicsWorld* m_dynamicsWorld;
	btAlignedObjectArray<btCollisionShape*> m_collisionShapes;
	float m_gravity;
};

void SimulationCallback(btDynamicsWorld* world, btScalar timeStep);