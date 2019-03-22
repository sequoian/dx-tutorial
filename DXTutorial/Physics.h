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
	btRigidBody* CreateRigidBody(XMVECTOR position, XMVECTOR rotation, float mass, btCollisionShape* shape);

	inline btQuaternion QuatFromDX(XMVECTOR quat);
	inline XMVECTOR QuatToDX(btQuaternion quat);
	inline btVector3 VecFromDX(XMVECTOR vec);
	inline XMVECTOR VecToDX(btVector3 vec);

private:
	btDefaultCollisionConfiguration* m_collisionConfiguration;
	btCollisionDispatcher* m_dispatcher;
	btBroadphaseInterface* m_overlappingPairCache;
	btSequentialImpulseConstraintSolver* m_solver;
	btDiscreteDynamicsWorld* m_dynamicsWorld;
	btAlignedObjectArray<btCollisionShape*> m_collisionShapes;
	float m_gravity;
};